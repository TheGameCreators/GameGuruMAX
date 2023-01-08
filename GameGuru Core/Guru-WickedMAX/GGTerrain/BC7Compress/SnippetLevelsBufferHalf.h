#pragma once

#include "pch.h"
#include "Bc7Core.h"
#include "SnippetStoreNodeShort.h"

#if defined(OPTION_COUNTERS)
inline std::atomic_int gEstimateHalf;

extern std::atomic_int gLevels[17];
#endif

template<int MaxSize>
class LevelsBufferHalf final
{
public:
	int MinErr = kBlockMaximalAlphaError + kBlockMaximalColorError;
	int Count = 0;

	Node Err[MaxSize];

	static ALWAYS_INLINED int Min(int x, int y) noexcept
	{
		return (x < y) ? x : y;
	}

	static ALWAYS_INLINED int Max(int x, int y) noexcept
	{
		return (x > y) ? x : y;
	}

	ALWAYS_INLINED LevelsBufferHalf() noexcept = default;

	ALWAYS_INLINED void SetZeroError(int color) noexcept
	{
		MinErr = 0;
		Count = 1;

		Err[0].Error = 0;
		Err[0].Color = color;
	}

	template<int bits, int pbits, bool transparent, const uint8_t table[0x100][1 << 1 * (2 * (bits + 1) - 1)]>
	NOTINLINED void ComputeChannelLevelsReduced(const Area& area, const size_t offset, const int weight, const int water) noexcept
	{
		const uint8_t* values[16];

		size_t count;
		if constexpr (transparent)
		{
			count = area.Active;

			if (!count)
			{
				SetZeroError(pbits << (7 - bits));
				return;
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
		}

		int top = (water + weight - 1) / weight;
		if (!top)
			return;

		int d = _mm_cvtsi128_si32(_mm_cvttps_epi32(_mm_sqrt_ss(_mm_cvtepi32_ps(_mm_cvtsi32_si128(top)))));
		d -= int(d * d >= top);
		if constexpr (kDenoise)
		{
			d <<= kDenoise;
			d += (1 << kDenoise) - 1;
		}

		constexpr int tailmask = (1 << (8 - bits)) - 1;
		constexpr int shift = bits + 1;

		int L = ((const short*)&area.MinMax_U16)[offset + offset + 0];
		int H = ((const short*)&area.MinMax_U16)[offset + offset + 1];

		const bool reverse = (L != ((const short*)&area.Bounds_U16)[offset + offset + 0]);

		if constexpr (((pbits >> 8) == (pbits & 1)))
		{
			if (L == H)
			{
				if ((((L >> shift) ^ L) & ((1 << (8 - shift)) - 1)) == 0)
				{
					if (((L >> (8 - shift)) & 1) == (pbits & 1))
					{
						SetZeroError((H << 8) + L);
						return;
					}
				}
			}
		}

#if defined(OPTION_COUNTERS)
		gLevels[count]++;
#endif

		int LH = Min(L + d, 255);
		int HL = Max(H - d, 0);

		LH = Min(H - (H >> shift) + tailmask, LH - (LH >> shift)) & ~tailmask;
		HL = Max(L - (L >> shift), HL - (HL >> shift) + tailmask) & ~tailmask;

		int HH = 0x100;

		LH >>= 8 - shift;
		HL >>= 8 - shift;
		HH >>= 8 - shift;

		top = Min(top, (0xF * 0xF) * 16 + 1);

#if defined(OPTION_COUNTERS)
		gEstimateHalf++;
#endif

		NodeShort nodes1[(1 << shift) * (1 << shift)];
		NodeShort nodes2[(1 << shift) * (1 << shift)];
		NodeShort* nodesPtr = nodes1;

		const __m128i mtop = _mm_shuffle_epi32(_mm_shufflelo_epi16(_mm_cvtsi32_si128(top), 0), 0);

		int zeroes = 0;
		{
			const int pH = reverse ? (pbits & 1) : (pbits >> 8);
			const int pL = reverse ? (pbits >> 8) : (pbits & 1);

			HL += pH;

			for (int iH = HL; iH < HH; iH += 2)
			{
				int cH = (iH << shift);

				zeroes += Estimate32PStep8Short(nodesPtr, values, count, cH, LH + cH, mtop, pL);
				if (zeroes >= MaxSize)
					break;
			}
		}

		NodeShort* sorted = nodes1;
		int nodesCount = int(nodesPtr - sorted);
		if (zeroes >= MaxSize)
		{
			const NodeShort* r = sorted;
			NodeShort* w = nodes2;
			do
			{
				const NodeShort val = *r++;
				*w = val;

				int zero = (val.ColorError & 0xFFFF0000) == 0;
				zeroes -= zero;
				w += static_cast<uint32_t>(zero);

			} while (zeroes > 0);

			sorted = nodes2;

			MinErr = 0;
			Count = MaxSize;
		}
		else
		{
			if (nodesCount)
			{
				sorted = radix_sort(sorted, nodes2, (uint32_t)nodesCount);

				MinErr = (sorted[0].ColorError >> 16) * weight;
				Count = Min(nodesCount, MaxSize);
			}
		}

		if (reverse)
		{
			for (int i = 0, n = Count; i < n; i++)
			{
				uint32_t ce = sorted[i].ColorError;
				int e = ce >> 16;
				int c = ce & 0xFFFFu;

				{
					int cL = (c >> shift) << (8 - shift);
					int cH = (c & ((1 << shift) - 1)) << (8 - shift);

					c = ((cH + (cH >> shift)) << 8) + cL + (cL >> shift);
				}

				Err[i].Error = e * weight;
				Err[i].Color = c;
			}
		}
		else
		{
			for (int i = 0, n = Count; i < n; i++)
			{
				uint32_t ce = sorted[i].ColorError;
				int e = ce >> 16;
				int c = ce & 0xFFFFu;

				if constexpr (shift != 8)
				{
					int cH = (c >> shift) << (8 - shift);
					int cL = (c & ((1 << shift) - 1)) << (8 - shift);

					c = ((cH + (cH >> shift)) << 8) + cL + (cL >> shift);
				}

				Err[i].Error = e * weight;
				Err[i].Color = c;
			}
		}
	}

private:
#if defined(OPTION_AVX512)

	static INLINED int Estimate32PStep8Short(NodeShort*& nodesPtr, const uint8_t* values[16], const size_t count, const int cH, int hL, const __m128i mtop, const int pL) noexcept
	{
		const __m512i wtop = _mm512_broadcastw_epi16(mtop);

		const __m128i mshift = _mm_cvtsi32_si128(pL << 2);
		const __m256i vmask = _mm256_set1_epi8(0xF);

		__m512i wzeroes = _mm512_setzero_si512();

		int c = cH;
		for (;;)
		{
			__m512i wsum = _mm512_setzero_si512();
			__m512i wsum1 = _mm512_setzero_si512();

			int k = static_cast<int>(count);
			const uint8_t** p = values;

			while ((k -= 2) >= 0)
			{
				auto value = p[0];
				auto value1 = p[1];

				const __m256i* p0 = (const __m256i*)&value[c >> 1];
				const __m256i* p1 = (const __m256i*)&value1[c >> 1];

				__m256i vdelta = _mm256_load_si256(p0);
				__m256i vdelta1 = _mm256_load_si256(p1);

				vdelta = _mm256_and_si256(_mm256_srl_epi16(vdelta, mshift), vmask);
				vdelta1 = _mm256_and_si256(_mm256_srl_epi16(vdelta1, mshift), vmask);

				__m512i wadd = _mm512_cvtepu8_epi16(vdelta);
				__m512i wadd1 = _mm512_cvtepu8_epi16(vdelta1);

				wadd = _mm512_mullo_epi16(wadd, wadd);
				wadd1 = _mm512_mullo_epi16(wadd1, wadd1);

				wsum = _mm512_add_epi16(wsum, wadd);
				wsum1 = _mm512_add_epi16(wsum1, wadd1);

				p += 2;
			}

			wsum = _mm512_add_epi16(wsum, wsum1);

			if (k & 1)
			{
				auto value = p[0];

				const __m256i* p0 = (const __m256i*)&value[c >> 1];

				__m256i vdelta = _mm256_load_si256(p0);

				vdelta = _mm256_and_si256(_mm256_srl_epi16(vdelta, mshift), vmask);

				__m512i wadd = _mm512_cvtepu8_epi16(vdelta);

				wadd = _mm512_mullo_epi16(wadd, wadd);

				wsum = _mm512_add_epi16(wsum, wadd);
			}

			uint32_t flags = _mm512_cmp_epi16_mask(wtop, wsum, _MM_CMPINT_GT);

			const __m512i wbottom = _mm512_movm_epi16(_mm512_cmp_epi16_mask(_mm512_setzero_si512(), wsum, _MM_CMPINT_EQ));

			const int tail = Max(0, (c | 63) - (hL | 7));
			flags &= ~0u >> (tail >> 1);

			wzeroes = _mm512_sub_epi16(wzeroes, wbottom);

			if (flags & 0xFFFFu)
			{
				nodesPtr = Store16<2>(nodesPtr, _mm512_castsi512_si256(wsum), flags & 0xFFFFu, c + pL);
			}

			flags >>= 16;

			if (flags)
			{
				nodesPtr = Store16<2>(nodesPtr, _mm512_extracti32x8_epi32(wsum, 1), flags, c + pL + 32);
			}

			c += 64;
			if (c <= hL)
				continue;

			{
				alignas(8) static constexpr int8_t gTail[8][8] =
				{
					{ 0, 0, 0, 0, 0, 0, 0, 0 },
					{ 0, 0, 0, 0, 0, 0, 0, -1 },
					{ 0, 0, 0, 0, 0, 0, -1, -1 },
					{ 0, 0, 0, 0, 0, -1, -1, -1 },
					{ 0, 0, 0, 0, -1, -1, -1, -1 },
					{ 0, 0, 0, -1, -1, -1, -1, -1 },
					{ 0, 0, -1, -1, -1, -1, -1, -1 },
					{ 0, -1, -1, -1, -1, -1, -1, -1 }
				};

				const __m128i mtail = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(reinterpret_cast<const uint8_t*>(gTail) + static_cast<uint32_t>(tail)));

				wzeroes = _mm512_add_epi16(wzeroes, _mm512_and_epi32(wbottom, _mm512_cvtepi8_epi64(mtail)));

				__m256i vzeroes = _mm256_add_epi16(_mm512_castsi512_si256(wzeroes), _mm512_extracti32x8_epi32(wzeroes, 1));
				__m128i mzeroes = _mm_add_epi16(_mm256_castsi256_si128(vzeroes), _mm256_extracti128_si256(vzeroes, 1));
				mzeroes = _mm_add_epi16(mzeroes, _mm_shuffle_epi32(mzeroes, _MM_SHUFFLE(1, 0, 3, 2)));
				mzeroes = _mm_add_epi16(mzeroes, _mm_shufflelo_epi16(mzeroes, _MM_SHUFFLE(1, 0, 3, 2)));
				mzeroes = _mm_add_epi16(mzeroes, _mm_shufflelo_epi16(mzeroes, _MM_SHUFFLE(2, 3, 0, 1)));

				return _mm_extract_epi16(mzeroes, 0);
			}
		}
	}

#elif defined(OPTION_AVX2)

	static INLINED int Estimate32PStep8Short(NodeShort*& nodesPtr, const uint8_t* values[16], const size_t count, const int cH, int hL, const __m128i mtop, const int pL) noexcept
	{
		const __m256i vtop = _mm256_broadcastw_epi16(mtop);

		const __m128i mshift = _mm_cvtsi32_si128(pL << 2);
		const __m128i mmask = _mm_set1_epi8(0xF);

		__m256i vzeroes = _mm256_setzero_si256();

		int c = cH;
		for (;;)
		{
			__m256i vsum = _mm256_setzero_si256();

			for (size_t i = 0; i < count; i++)
			{
				auto value = values[i];

				const __m128i* p = (const __m128i*)&value[c >> 1];

				__m128i mdelta = _mm_load_si128(p);
				mdelta = _mm_and_si128(_mm_srl_epi16(mdelta, mshift), mmask);

				__m256i vadd = _mm256_cvtepu8_epi16(mdelta);

				vadd = _mm256_mullo_epi16(vadd, vadd);

				vsum = _mm256_add_epi16(vsum, vadd);
			}

			uint32_t flags = static_cast<uint32_t>(_mm256_movemask_epi8(_mm256_cmpgt_epi16(vtop, vsum)));

			const __m256i vbottom = _mm256_cmpeq_epi16(_mm256_setzero_si256(), vsum);

			const int tail = Max(0, (c | 31) - (hL | 7));
			flags &= ~0u >> tail;

			vzeroes = _mm256_sub_epi16(vzeroes, vbottom);

			if (flags & 0xFFFFu)
			{
				nodesPtr = Store8<2>(nodesPtr, _mm256_castsi256_si128(vsum), flags, c + pL);
			}

			flags >>= 16;

			if (flags)
			{
				nodesPtr = Store8<2>(nodesPtr, _mm256_extracti128_si256(vsum, 1), flags, c + pL + 16);
			}

			c += 32;
			if (c <= hL)
				continue;

			{
				alignas(8) static constexpr int16_t gTail[4][4] =
				{
					{ 0, 0, 0, 0 },
					{ 0, 0, 0, -1 },
					{ 0, 0, -1, -1 },
					{ 0, -1, -1, -1 }
				};

				const __m128i mtail = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(reinterpret_cast<const uint8_t*>(gTail) + static_cast<uint32_t>(tail)));

				vzeroes = _mm256_add_epi16(vzeroes, _mm256_and_si256(vbottom, _mm256_cvtepi8_epi32(mtail)));

				__m128i mzeroes = _mm_add_epi16(_mm256_castsi256_si128(vzeroes), _mm256_extracti128_si256(vzeroes, 1));
				mzeroes = _mm_add_epi16(mzeroes, _mm_shuffle_epi32(mzeroes, _MM_SHUFFLE(1, 0, 3, 2)));
				mzeroes = _mm_add_epi16(mzeroes, _mm_shufflelo_epi16(mzeroes, _MM_SHUFFLE(1, 0, 3, 2)));
				mzeroes = _mm_add_epi16(mzeroes, _mm_shufflelo_epi16(mzeroes, _MM_SHUFFLE(2, 3, 0, 1)));

				return _mm_extract_epi16(mzeroes, 0);
			}
		}
	}

#else

	static INLINED int Estimate32PStep8Short(NodeShort*& nodesPtr, const uint8_t* values[16], const size_t count, const int cH, int hL, const __m128i mtop, const int pL) noexcept
	{
		const __m128i mshift = _mm_cvtsi32_si128(pL << 2);
		const __m128i mmask = _mm_set1_epi8(0xF);

		__m128i mzeroes = _mm_setzero_si128();

		int c = cH;
		for (;;)
		{
			__m128i msum = _mm_setzero_si128();

			for (size_t i = 0; i < count; i++)
			{
				auto value = values[i];

				const __m128i* p = (const __m128i*)&value[c >> 1];

				__m128i mdelta = _mm_loadl_epi64(p);
				mdelta = _mm_and_si128(_mm_srl_epi16(mdelta, mshift), mmask);

				__m128i madd = _mm_cvtepu8_epi16(mdelta);

				madd = _mm_mullo_epi16(madd, madd);

				msum = _mm_add_epi16(msum, madd);
			}

			uint32_t flags = static_cast<uint32_t>(_mm_movemask_epi8(_mm_cmpgt_epi16(mtop, msum)));

			const __m128i mbottom = _mm_cmpeq_epi16(_mm_setzero_si128(), msum);

			const int tail = Max(0, (c | 15) - (hL | 7));
			flags &= 0xFFFFu >> tail;

			mzeroes = _mm_sub_epi16(mzeroes, mbottom);

			if (flags)
			{
				nodesPtr = Store8<2>(nodesPtr, msum, flags, c + pL);
			}

			c += 16;
			if (c <= hL)
				continue;

			{
				alignas(8) static constexpr int32_t gTail[2][2] =
				{
					{ 0, 0 },
					{ 0, -1 }
				};

				const __m128i mtail = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(reinterpret_cast<const uint8_t*>(gTail) + static_cast<uint32_t>(tail)));

				mzeroes = _mm_add_epi16(mzeroes, _mm_and_si128(mbottom, _mm_shuffle_epi32(mtail, _MM_SHUFFLE(1, 1, 0, 0))));

				mzeroes = _mm_add_epi16(mzeroes, _mm_shuffle_epi32(mzeroes, _MM_SHUFFLE(1, 0, 3, 2)));
				mzeroes = _mm_add_epi16(mzeroes, _mm_shufflelo_epi16(mzeroes, _MM_SHUFFLE(1, 0, 3, 2)));
				mzeroes = _mm_add_epi16(mzeroes, _mm_shufflelo_epi16(mzeroes, _MM_SHUFFLE(2, 3, 0, 1)));

				return _mm_extract_epi16(mzeroes, 0);
			}
		}
	}

#endif
};
