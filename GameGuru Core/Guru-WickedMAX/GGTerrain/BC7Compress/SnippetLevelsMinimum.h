#pragma once

#include "pch.h"
#include "Bc7Core.h"

#if defined(OPTION_COUNTERS)
inline std::atomic_int gMinimumFull, gMinimumShort;
#endif

namespace LevelsMinimum {

	static ALWAYS_INLINED int Min(int x, int y) noexcept
	{
		return (x < y) ? x : y;
	}

	static ALWAYS_INLINED int Max(int x, int y) noexcept
	{
		return (x > y) ? x : y;
	}

#if defined(OPTION_AVX512)

	static INLINED void Estimate32Short(__m512i& wbest, const uint8_t* values[16], const size_t count, const int c) noexcept
	{
		__m512i wsum = _mm512_setzero_si512();

		for (size_t i = 0; i < count; i++)
		{
			auto value = values[i];

			const __m256i* p = (const __m256i*)&value[c];

			__m256i vdelta = _mm256_load_si256(p);

			__m512i wadd = _mm512_cvtepu8_epi16(vdelta);

			wadd = _mm512_mullo_epi16(wadd, wadd);

			wsum = _mm512_adds_epu16(wsum, wadd);
		}

		wbest = _mm512_min_epu16(wbest, wsum);
	}

#elif defined(OPTION_AVX2)

	static INLINED void Estimate32Short(__m256i& vbest, const uint8_t* values[16], const size_t count, const int c) noexcept
	{
		__m256i vsum0 = _mm256_setzero_si256();
		__m256i vsum1 = _mm256_setzero_si256();

		__m256i vzero = _mm256_setzero_si256();

		for (size_t i = 0; i < count; i++)
		{
			auto value = values[i];

			const __m256i* p = (const __m256i*)&value[c];

			__m256i vdelta = _mm256_load_si256(p);

			__m256i vadd0 = _mm256_unpacklo_epi8(vdelta, vzero);
			__m256i vadd1 = _mm256_unpackhi_epi8(vdelta, vzero);

			vadd0 = _mm256_mullo_epi16(vadd0, vadd0);
			vadd1 = _mm256_mullo_epi16(vadd1, vadd1);

			vsum0 = _mm256_adds_epu16(vsum0, vadd0);
			vsum1 = _mm256_adds_epu16(vsum1, vadd1);
		}

		vbest = _mm256_min_epu16(vbest, _mm256_min_epu16(vsum0, vsum1));
	}

#else

	static INLINED void Estimate32Short(__m128i& mbest, const uint8_t* values[16], const size_t count, const int c) noexcept
	{
		__m128i msum0 = _mm_setzero_si128();
		__m128i msum1 = _mm_setzero_si128();
		__m128i msum2 = _mm_setzero_si128();
		__m128i msum3 = _mm_setzero_si128();

		__m128i mzero = _mm_setzero_si128();

		for (size_t i = 0; i < count; i++)
		{
			auto value = values[i];

			const __m128i* p = (const __m128i*)&value[c];

			__m128i mdelta0 = _mm_load_si128(&p[0]);
			__m128i mdelta1 = _mm_load_si128(&p[1]);

			__m128i madd0 = _mm_cvtepu8_epi16(mdelta0);
			__m128i madd1 = _mm_unpackhi_epi8(mdelta0, mzero);
			__m128i madd2 = _mm_cvtepu8_epi16(mdelta1);
			__m128i madd3 = _mm_unpackhi_epi8(mdelta1, mzero);

			madd0 = _mm_mullo_epi16(madd0, madd0);
			madd1 = _mm_mullo_epi16(madd1, madd1);
			madd2 = _mm_mullo_epi16(madd2, madd2);
			madd3 = _mm_mullo_epi16(madd3, madd3);

			msum0 = _mm_adds_epu16(msum0, madd0);
			msum1 = _mm_adds_epu16(msum1, madd1);
			msum2 = _mm_adds_epu16(msum2, madd2);
			msum3 = _mm_adds_epu16(msum3, madd3);
		}

		mbest = _mm_min_epu16(mbest, _mm_min_epu16(_mm_min_epu16(msum0, msum1), _mm_min_epu16(msum2, msum3)));
	}

#endif

	template<int bits, bool transparent, const uint8_t table[0x100][1 << 2 * bits], const uint16_t tower[0x100][1 << bits]>
	NOTINLINED int EstimateChannelLevelsReduced(const Area& area, const size_t offset, const int weight, const int water) noexcept
	{
		const uint8_t* values[16];
		const uint16_t* cuts[16];

		size_t count;
		if constexpr (transparent)
		{
			count = area.Active;

			if (!count)
			{
				return 0;
			}
		}
		else
		{
			count = area.Count;
		}

		for (size_t i = 0; i < count; i++)
		{
			size_t value = ((const uint16_t*)&area.DataMask_I16[i])[offset];

			values[i] = table[value];
			cuts[i] = tower[value];
		}

		int top = (water + weight - 1) / weight;
		if (!top)
			return 0;

		int d = _mm_cvtsi128_si32(_mm_cvttps_epi32(_mm_sqrt_ss(_mm_cvtepi32_ps(_mm_cvtsi32_si128(top)))));
		d -= int(d * d >= top);
		if constexpr (kDenoise)
		{
			d <<= kDenoise;
			d += (1 << kDenoise) - 1;
		}

		constexpr int tailmask = (1 << (8 - bits)) - 1;
		constexpr int shift = bits;

		int L = ((const short*)&area.MinMax_U16)[offset + offset + 0];
		int H = ((const short*)&area.MinMax_U16)[offset + offset + 1];

		if (L == H)
		{
			// Residual is always (0 or 1) * count
			return 0;
		}

		if constexpr (bits == 7)
		{
			// Mode 1
			if (H - L <= 5)
			{
				return 0;
			}
		}

		if constexpr (bits == 8)
		{
			// Mode 3
			if (H - L <= 3)
			{
				return 0;
			}
		}

		int LH = Min(L + d, 255);
		int HL = Max(H - d, 0);

		LH = Min(H - (H >> shift) + tailmask, LH - (LH >> shift)) & ~tailmask;
		HL = Max(L - (L >> shift), HL - (HL >> shift) + tailmask) & ~tailmask;

		int HH = 0x100;

		LH >>= 8 - shift;
		HL >>= 8 - shift;
		HH >>= 8 - shift;

		if (top > 0xFFFF)
		{
#if defined(OPTION_COUNTERS)
			gMinimumFull++;
#endif
			top = 0xFFFF;
		}
		else
		{
#if defined(OPTION_COUNTERS)
			gMinimumShort++;
#endif
		}

		alignas(64) uint16_t rows[1 << bits];

#if defined(OPTION_AVX512)
		for (int iH = HL & ~31; iH < HH; iH += 32)
		{
			__m512i wcut = _mm512_setzero_si512();

			for (size_t i = 0; i < count; i++)
			{
				auto value = cuts[i];

				const __m512i* p = (const __m512i*)&value[iH];

				__m512i wadd = _mm512_load_epi64(p);

				wcut = _mm512_adds_epu16(wcut, wadd);
			}

			_mm512_store_epi64((__m512i*)&rows[iH], wcut);
		}
#elif defined(OPTION_AVX2)
		for (int iH = HL & ~15; iH < HH; iH += 16)
		{
			__m256i vcut = _mm256_setzero_si256();

			for (size_t i = 0; i < count; i++)
			{
				auto value = cuts[i];

				const __m256i* p = (const __m256i*)&value[iH];

				__m256i vadd = _mm256_load_si256(p);

				vcut = _mm256_adds_epu16(vcut, vadd);
			}

			_mm256_store_si256((__m256i*)&rows[iH], vcut);
		}
#else
		for (int iH = HL & ~7; iH < HH; iH += 8)
		{
			__m128i mcut = _mm_setzero_si128();

			for (size_t i = 0; i < count; i++)
			{
				auto value = cuts[i];

				const __m128i* p = (const __m128i*)&value[iH];

				__m128i madd = _mm_load_si128(p);

				mcut = _mm_adds_epu16(mcut, madd);
			}

			_mm_store_si128((__m128i*)&rows[iH], mcut);
		}
#endif

		int best = top;

		for (int iH = HL; iH < HH; iH++)
		{
			if (rows[iH] >= best)
				continue;

#if defined(OPTION_AVX512)
			__m512i wbest = _mm512_set1_epi16(-1);
#elif defined(OPTION_AVX2)
			__m256i vbest = _mm256_set1_epi16(-1);
#else
			__m128i mbest = _mm_set1_epi16(-1);
#endif

			int cH = (iH << shift);

			for (int iL = cH, hL = Min(LH, iH) + cH; iL <= hL; iL += 32)
			{
#if defined(OPTION_AVX512)
				Estimate32Short(wbest, values, count, iL);
#elif defined(OPTION_AVX2)
				Estimate32Short(vbest, values, count, iL);
#else
				Estimate32Short(mbest, values, count, iL);
#endif
			}

#if defined(OPTION_AVX512)
			__m256i vbest = _mm256_min_epu16(_mm512_extracti64x4_epi64(wbest, 1), _mm512_castsi512_si256(wbest));
			__m128i mbest = _mm_min_epu16(_mm256_extracti128_si256(vbest, 1), _mm256_castsi256_si128(vbest));
#elif defined(OPTION_AVX2)
			__m128i mbest = _mm_min_epu16(_mm256_extracti128_si256(vbest, 1), _mm256_castsi256_si128(vbest));
#endif

			best = Min(best, _mm_extract_epi16(_mm_minpos_epu16(mbest), 0));
		}

		return best * weight;
	}

} // namespace LevelsMinimum
