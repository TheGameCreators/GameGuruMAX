
#include "pch.h"
#include "Bc7Core.h"
#include "Bc7Tables.h"
#include "Bc7Pca.h"

#include "SnippetInsertRemoveZeroBit.h"
#include "SnippetLevelsBufferHalf.h"

// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/bc7-format-mode-reference#mode-6

namespace Mode6 {

	constexpr int LevelsCapacity = 20;

#if defined(OPTION_COUNTERS)
	static std::atomic_int gComputeSubsetError4, gComputeSubsetError4AG, gComputeSubsetError4AR, gComputeSubsetError4GR, gComputeSubsetError4AGR, gComputeSubsetError4GB, gComputeSubsetError4AGB;
#endif

	static INLINED void DecompressSubset(__m128i mc, int* output, uint64_t data) noexcept
	{
		const __m128i mhalf = _mm_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);

		int y = 4;
		do
		{
			__m128i mratio0 = _mm_loadl_epi64((const __m128i*)&((const uint64_t*)gTableInterpolate4_U8)[data & 0xF]); data >>= 4;
			__m128i mratio1 = _mm_loadl_epi64((const __m128i*)&((const uint64_t*)gTableInterpolate4_U8)[data & 0xF]); data >>= 4;
			__m128i mratio2 = _mm_loadl_epi64((const __m128i*)&((const uint64_t*)gTableInterpolate4_U8)[data & 0xF]); data >>= 4;
			__m128i mratio3 = _mm_loadl_epi64((const __m128i*)&((const uint64_t*)gTableInterpolate4_U8)[data & 0xF]); data >>= 4;

			__m128i mx = _mm_maddubs_epi16(mc, _mm_unpacklo_epi64(mratio0, mratio1));
			__m128i mz = _mm_maddubs_epi16(mc, _mm_unpacklo_epi64(mratio2, mratio3));

			mx = _mm_add_epi16(mx, mhalf);
			mz = _mm_add_epi16(mz, mhalf);

			mx = _mm_srli_epi16(mx, 6);
			mz = _mm_srli_epi16(mz, 6);

			_mm_storeu_si128(reinterpret_cast<__m128i*>(output), _mm_packus_epi16(mx, mz));
			output += 4;

		} while (--y > 0);
	}

	void DecompressBlock(uint8_t input[16], Cell& output) noexcept
	{
		uint64_t data0 = *(const uint64_t*)&input[0];
		uint64_t data1 = *(const uint64_t*)&input[8];

		data0 >>= 7;

		__m128i mc0 = _mm_setzero_si128();

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 4); data0 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 5); data0 >>= 7;

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 2); data0 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 3); data0 >>= 7;

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 6); data0 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 7); data0 >>= 7;

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 0); data0 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 1); data0 >>= 7;

		const __m128i m7 = _mm_set1_epi16(0x7F);
		mc0 = _mm_and_si128(mc0, m7);

		mc0 = _mm_add_epi16(mc0, mc0);

		int pbits = static_cast<int>(data0) | (static_cast<int>(data1 & 1) << 16); data1 >>= 1;
		mc0 = _mm_or_si128(mc0, _mm_shuffle_epi32(_mm_cvtsi32_si128(pbits), 0));

		data1 = InsertZeroBit(data1, 3);

		DecompressSubset(mc0, (int*)output.ImageRows_U8, data1);

		output.BestColor0 = mc0;
		output.BestColor1 = _mm_setzero_si128();
		output.BestColor2 = _mm_setzero_si128();
		output.BestParameter = 0;
		output.BestMode = 6;
	}

	static INLINED void ComposeBlock(uint8_t output[16], __m128i mc0, uint64_t indices) noexcept
	{
		uint64_t data1 = RemoveZeroBit(indices, 3);

		data1 <<= 1; data1 |= _mm_extract_epi16(mc0, 1) & 1;

		uint64_t data0 = _mm_extract_epi16(mc0, 0) & 1;

		mc0 = _mm_srli_epi16(mc0, 1);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 1);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 0);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 7);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 6);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 3);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 2);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 5);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 4);

		data0 <<= 7; data0 |= 1 << 6;

		*(uint64_t*)&output[0] = data0;
		*(uint64_t*)&output[8] = data1;
	}

	static INLINED int ComputeSubsetTransparentError4(const Area& area, const int alpha) noexcept
	{
		int error = static_cast<int>(area.Count - area.Active);
		if (error)
		{
			error *= kAlpha;
			int v = (gTableDeltas4Half_Value8[0][alpha >> 1] >> ((alpha & 1) << 2)) & 0xF;
			error *= v * v;
		}

		return error;
	}

	static INLINED int ComputeSubsetError4(const Area& area, __m128i mc, const __m128i mweights, const __m128i mwater) noexcept
	{
		__m128i merrorBlock = _mm_setzero_si128();

#if defined(OPTION_AVX512)
		const __m512i wweights = _mm512_broadcastq_epi64(mweights);

		const __m512i whalf = _mm512_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);
		__m512i wc = _mm512_broadcastq_epi64(mc);

		__m512i wt0 = *(const __m512i*)&gTableInterpolate4_U8[0];
		__m512i wt1 = *(const __m512i*)&gTableInterpolate4_U8[4];

		wt0 = _mm512_maddubs_epi16(wc, wt0);
		wt1 = _mm512_maddubs_epi16(wc, wt1);

		wt0 = _mm512_add_epi16(wt0, whalf);
		wt1 = _mm512_add_epi16(wt1, whalf);

		wt0 = _mm512_srli_epi16(wt0, 6);
		wt1 = _mm512_srli_epi16(wt1, 6);

		__m512i wt00 = _mm512_inserti64x4(wt0, _mm512_castsi512_si256(wt0), 1);
		__m512i wt01 = _mm512_inserti64x4(wt0, _mm512_extracti64x4_epi64(wt0, 1), 0);
		__m512i wt10 = _mm512_inserti64x4(wt1, _mm512_castsi512_si256(wt1), 1);
		__m512i wt11 = _mm512_inserti64x4(wt1, _mm512_extracti64x4_epi64(wt1, 1), 0);

		__m512i wtx = _mm512_permutex_epi64(wt00, 0x44);
		__m512i wty = _mm512_permutex_epi64(wt00, 0xEE);
		__m512i wtz = _mm512_permutex_epi64(wt10, 0x44);
		__m512i wtw = _mm512_permutex_epi64(wt10, 0xEE);
		__m512i wrx = _mm512_permutex_epi64(wt01, 0x44);
		__m512i wry = _mm512_permutex_epi64(wt01, 0xEE);
		__m512i wrz = _mm512_permutex_epi64(wt11, 0x44);
		__m512i wrw = _mm512_permutex_epi64(wt11, 0xEE);

		int k = static_cast<int>(area.Active);
		const __m512i* p = (const __m512i*)area.DataMask_I16;

		while ((k -= 4) >= 0)
		{
			__m512i wpacked = _mm512_load_epi64(p);
			__m512i wpixel = _mm512_unpacklo_epi64(wpacked, wpacked);

			__m512i wx = _mm512_sub_epi16(wpixel, wtx);
			__m512i wy = _mm512_sub_epi16(wpixel, wty);
			__m512i wz = _mm512_sub_epi16(wpixel, wtz);
			__m512i ww = _mm512_sub_epi16(wpixel, wtw);
			__m512i wxx = _mm512_sub_epi16(wpixel, wrx);
			__m512i wyy = _mm512_sub_epi16(wpixel, wry);
			__m512i wzz = _mm512_sub_epi16(wpixel, wrz);
			__m512i www = _mm512_sub_epi16(wpixel, wrw);

			wx = _mm512_abs_epi16(wx);
			wy = _mm512_abs_epi16(wy);
			wz = _mm512_abs_epi16(wz);
			ww = _mm512_abs_epi16(ww);
			wxx = _mm512_abs_epi16(wxx);
			wyy = _mm512_abs_epi16(wyy);
			wzz = _mm512_abs_epi16(wzz);
			www = _mm512_abs_epi16(www);

			if constexpr (!kDenoise)
			{
				wx = _mm512_adds_epu8(wx, wx);
				wy = _mm512_adds_epu8(wy, wy);
				wz = _mm512_adds_epu8(wz, wz);
				ww = _mm512_adds_epu8(ww, ww);
				wxx = _mm512_adds_epu8(wxx, wxx);
				wyy = _mm512_adds_epu8(wyy, wyy);
				wzz = _mm512_adds_epu8(wzz, wzz);
				www = _mm512_adds_epu8(www, www);
			}

			wx = _mm512_srli_epi16(wx, kDenoiseShift);
			wy = _mm512_srli_epi16(wy, kDenoiseShift);
			wz = _mm512_srli_epi16(wz, kDenoiseShift);
			ww = _mm512_srli_epi16(ww, kDenoiseShift);
			wxx = _mm512_srli_epi16(wxx, kDenoiseShift);
			wyy = _mm512_srli_epi16(wyy, kDenoiseShift);
			wzz = _mm512_srli_epi16(wzz, kDenoiseShift);
			www = _mm512_srli_epi16(www, kDenoiseShift);

			wx = _mm512_mullo_epi16(wx, wx);
			wy = _mm512_mullo_epi16(wy, wy);
			wz = _mm512_mullo_epi16(wz, wz);
			ww = _mm512_mullo_epi16(ww, ww);
			wxx = _mm512_mullo_epi16(wxx, wxx);
			wyy = _mm512_mullo_epi16(wyy, wyy);
			wzz = _mm512_mullo_epi16(wzz, wzz);
			www = _mm512_mullo_epi16(www, www);

			wx = _mm512_madd_epi16(wx, wweights);
			wy = _mm512_madd_epi16(wy, wweights);
			wz = _mm512_madd_epi16(wz, wweights);
			ww = _mm512_madd_epi16(ww, wweights);
			wxx = _mm512_madd_epi16(wxx, wweights);
			wyy = _mm512_madd_epi16(wyy, wweights);
			wzz = _mm512_madd_epi16(wzz, wweights);
			www = _mm512_madd_epi16(www, wweights);

			wx = _mm512_add_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(2, 3, 0, 1)));
			wy = _mm512_add_epi32(wy, _mm512_shuffle_epi32(wy, _MM_SHUFFLE(2, 3, 0, 1)));
			wz = _mm512_add_epi32(wz, _mm512_shuffle_epi32(wz, _MM_SHUFFLE(2, 3, 0, 1)));
			ww = _mm512_add_epi32(ww, _mm512_shuffle_epi32(ww, _MM_SHUFFLE(2, 3, 0, 1)));
			wxx = _mm512_add_epi32(wxx, _mm512_shuffle_epi32(wxx, _MM_SHUFFLE(2, 3, 0, 1)));
			wyy = _mm512_add_epi32(wyy, _mm512_shuffle_epi32(wyy, _MM_SHUFFLE(2, 3, 0, 1)));
			wzz = _mm512_add_epi32(wzz, _mm512_shuffle_epi32(wzz, _MM_SHUFFLE(2, 3, 0, 1)));
			www = _mm512_add_epi32(www, _mm512_shuffle_epi32(www, _MM_SHUFFLE(2, 3, 0, 1)));

			wx = _mm512_min_epi32(_mm512_min_epi32(wx, wy), _mm512_min_epi32(wz, ww));
			wxx = _mm512_min_epi32(_mm512_min_epi32(wxx, wyy), _mm512_min_epi32(wzz, www));
			wx = _mm512_min_epi32(wx, wxx);
			wx = _mm512_min_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(1, 0, 3, 2)));

			__m256i vx = _mm256_add_epi32(_mm512_extracti64x4_epi64(wx, 1), _mm512_castsi512_si256(wx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p++;

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				goto done;
		}

		if (k & 2)
		{
			wtx = _mm512_inserti64x4(wtx, _mm512_castsi512_si256(wrx), 1);
			wty = _mm512_inserti64x4(wty, _mm512_castsi512_si256(wry), 1);
			wtz = _mm512_inserti64x4(wtz, _mm512_castsi512_si256(wrz), 1);
			wtw = _mm512_inserti64x4(wtw, _mm512_castsi512_si256(wrw), 1);

			__m256i vpacked = _mm256_load_si256((const __m256i*)p);
			__m256i vpixel = _mm256_unpacklo_epi64(vpacked, vpacked);
			__m512i wpixel = _mm512_broadcast_i64x4(vpixel);

			__m512i wx = _mm512_sub_epi16(wpixel, wtx);
			__m512i wy = _mm512_sub_epi16(wpixel, wty);
			__m512i wz = _mm512_sub_epi16(wpixel, wtz);
			__m512i ww = _mm512_sub_epi16(wpixel, wtw);

			wx = _mm512_abs_epi16(wx);
			wy = _mm512_abs_epi16(wy);
			wz = _mm512_abs_epi16(wz);
			ww = _mm512_abs_epi16(ww);

			if constexpr (!kDenoise)
			{
				wx = _mm512_adds_epu8(wx, wx);
				wy = _mm512_adds_epu8(wy, wy);
				wz = _mm512_adds_epu8(wz, wz);
				ww = _mm512_adds_epu8(ww, ww);
			}

			wx = _mm512_srli_epi16(wx, kDenoiseShift);
			wy = _mm512_srli_epi16(wy, kDenoiseShift);
			wz = _mm512_srli_epi16(wz, kDenoiseShift);
			ww = _mm512_srli_epi16(ww, kDenoiseShift);

			wx = _mm512_mullo_epi16(wx, wx);
			wy = _mm512_mullo_epi16(wy, wy);
			wz = _mm512_mullo_epi16(wz, wz);
			ww = _mm512_mullo_epi16(ww, ww);

			wx = _mm512_madd_epi16(wx, wweights);
			wy = _mm512_madd_epi16(wy, wweights);
			wz = _mm512_madd_epi16(wz, wweights);
			ww = _mm512_madd_epi16(ww, wweights);

			wx = _mm512_add_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(2, 3, 0, 1)));
			wy = _mm512_add_epi32(wy, _mm512_shuffle_epi32(wy, _MM_SHUFFLE(2, 3, 0, 1)));
			wz = _mm512_add_epi32(wz, _mm512_shuffle_epi32(wz, _MM_SHUFFLE(2, 3, 0, 1)));
			ww = _mm512_add_epi32(ww, _mm512_shuffle_epi32(ww, _MM_SHUFFLE(2, 3, 0, 1)));

			wx = _mm512_min_epi32(_mm512_min_epi32(wx, wy), _mm512_min_epi32(wz, ww));
			__m256i vx = _mm256_min_epi32(_mm512_extracti64x4_epi64(wx, 1), _mm512_castsi512_si256(wx));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p = reinterpret_cast<const __m512i*>(reinterpret_cast<const __m256i*>(p) + 1);
		}

		if (k & 1)
		{
			__m128i mpacked = _mm_load_si128((const __m128i*)p);
			__m512i wpixel = _mm512_broadcastq_epi64(mpacked);

			__m512i wx = _mm512_sub_epi16(wpixel, wt0);
			__m512i wy = _mm512_sub_epi16(wpixel, wt1);

			wx = _mm512_abs_epi16(wx);
			wy = _mm512_abs_epi16(wy);

			if constexpr (!kDenoise)
			{
				wx = _mm512_adds_epu8(wx, wx);
				wy = _mm512_adds_epu8(wy, wy);
			}

			wx = _mm512_srli_epi16(wx, kDenoiseShift);
			wy = _mm512_srli_epi16(wy, kDenoiseShift);

			wx = _mm512_mullo_epi16(wx, wx);
			wy = _mm512_mullo_epi16(wy, wy);

			wx = _mm512_madd_epi16(wx, wweights);
			wy = _mm512_madd_epi16(wy, wweights);

			wx = _mm512_add_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(2, 3, 0, 1)));
			wy = _mm512_add_epi32(wy, _mm512_shuffle_epi32(wy, _MM_SHUFFLE(2, 3, 0, 1)));

			wx = _mm512_min_epi32(wx, wy);
			__m256i vx = _mm256_min_epi32(_mm512_extracti64x4_epi64(wx, 1), _mm512_castsi512_si256(wx));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, _mm_min_epi32(_mm256_extracti128_si256(vx, 1), _mm256_castsi256_si128(vx)));
		}
	done:
#elif defined(OPTION_AVX2)
		const __m256i vweights = _mm256_broadcastq_epi64(mweights);

		const __m256i vhalf = _mm256_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);
		__m256i vc = _mm256_broadcastq_epi64(mc);

		__m256i vtx = *(const __m256i*)&gTableInterpolate4_U8[0];
		__m256i vty = *(const __m256i*)&gTableInterpolate4_U8[2];
		__m256i vtz = *(const __m256i*)&gTableInterpolate4_U8[4];
		__m256i vtw = *(const __m256i*)&gTableInterpolate4_U8[6];

		vtx = _mm256_maddubs_epi16(vc, vtx);
		vty = _mm256_maddubs_epi16(vc, vty);
		vtz = _mm256_maddubs_epi16(vc, vtz);
		vtw = _mm256_maddubs_epi16(vc, vtw);

		vtx = _mm256_add_epi16(vtx, vhalf);
		vty = _mm256_add_epi16(vty, vhalf);
		vtz = _mm256_add_epi16(vtz, vhalf);
		vtw = _mm256_add_epi16(vtw, vhalf);

		vtx = _mm256_srli_epi16(vtx, 6);
		vty = _mm256_srli_epi16(vty, 6);
		vtz = _mm256_srli_epi16(vtz, 6);
		vtw = _mm256_srli_epi16(vtw, 6);

		for (size_t i = 0, n = area.Active; i < n; i++)
		{
			__m128i mpacked = _mm_load_si128(&area.DataMask_I16[i]);
			__m256i vpixel = _mm256_broadcastq_epi64(mpacked);

			__m256i vx = _mm256_sub_epi16(vpixel, vtx);
			__m256i vy = _mm256_sub_epi16(vpixel, vty);
			__m256i vz = _mm256_sub_epi16(vpixel, vtz);
			__m256i vw = _mm256_sub_epi16(vpixel, vtw);

			vx = _mm256_abs_epi16(vx);
			vy = _mm256_abs_epi16(vy);
			vz = _mm256_abs_epi16(vz);
			vw = _mm256_abs_epi16(vw);

			if constexpr (!kDenoise)
			{
				vx = _mm256_adds_epu8(vx, vx);
				vy = _mm256_adds_epu8(vy, vy);
				vz = _mm256_adds_epu8(vz, vz);
				vw = _mm256_adds_epu8(vw, vw);
			}

			vx = _mm256_srli_epi16(vx, kDenoiseShift);
			vy = _mm256_srli_epi16(vy, kDenoiseShift);
			vz = _mm256_srli_epi16(vz, kDenoiseShift);
			vw = _mm256_srli_epi16(vw, kDenoiseShift);

			vx = _mm256_mullo_epi16(vx, vx);
			vy = _mm256_mullo_epi16(vy, vy);
			vz = _mm256_mullo_epi16(vz, vz);
			vw = _mm256_mullo_epi16(vw, vw);

			vx = _mm256_madd_epi16(vx, vweights);
			vy = _mm256_madd_epi16(vy, vweights);
			vz = _mm256_madd_epi16(vz, vweights);
			vw = _mm256_madd_epi16(vw, vweights);

			vx = _mm256_add_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
			vy = _mm256_add_epi32(vy, _mm256_shuffle_epi32(vy, _MM_SHUFFLE(2, 3, 0, 1)));
			vz = _mm256_add_epi32(vz, _mm256_shuffle_epi32(vz, _MM_SHUFFLE(2, 3, 0, 1)));
			vw = _mm256_add_epi32(vw, _mm256_shuffle_epi32(vw, _MM_SHUFFLE(2, 3, 0, 1)));

			vx = _mm256_min_epi32(_mm256_min_epi32(vx, vy), _mm256_min_epi32(vz, vw));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, _mm_min_epi32(_mm256_extracti128_si256(vx, 1), _mm256_castsi256_si128(vx)));

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				break;
		}
#else
		const __m128i mhalf = _mm_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);

		__m128i mtx = gTableInterpolate4_U8[0];
		__m128i mty = gTableInterpolate4_U8[1];
		__m128i mtz = gTableInterpolate4_U8[2];
		__m128i mtw = gTableInterpolate4_U8[3];
		__m128i mrx = gTableInterpolate4_U8[4];
		__m128i mry = gTableInterpolate4_U8[5];
		__m128i mrz = gTableInterpolate4_U8[6];
		__m128i mrw = gTableInterpolate4_U8[7];

		mtx = _mm_maddubs_epi16(mc, mtx);
		mty = _mm_maddubs_epi16(mc, mty);
		mtz = _mm_maddubs_epi16(mc, mtz);
		mtw = _mm_maddubs_epi16(mc, mtw);
		mrx = _mm_maddubs_epi16(mc, mrx);
		mry = _mm_maddubs_epi16(mc, mry);
		mrz = _mm_maddubs_epi16(mc, mrz);
		mrw = _mm_maddubs_epi16(mc, mrw);

		mtx = _mm_add_epi16(mtx, mhalf);
		mty = _mm_add_epi16(mty, mhalf);
		mtz = _mm_add_epi16(mtz, mhalf);
		mtw = _mm_add_epi16(mtw, mhalf);
		mrx = _mm_add_epi16(mrx, mhalf);
		mry = _mm_add_epi16(mry, mhalf);
		mrz = _mm_add_epi16(mrz, mhalf);
		mrw = _mm_add_epi16(mrw, mhalf);

		mtx = _mm_srli_epi16(mtx, 6);
		mty = _mm_srli_epi16(mty, 6);
		mtz = _mm_srli_epi16(mtz, 6);
		mtw = _mm_srli_epi16(mtw, 6);
		mrx = _mm_srli_epi16(mrx, 6);
		mry = _mm_srli_epi16(mry, 6);
		mrz = _mm_srli_epi16(mrz, 6);
		mrw = _mm_srli_epi16(mrw, 6);

		for (size_t i = 0, n = area.Active; i < n; i++)
		{
			__m128i mpacked = _mm_load_si128(&area.DataMask_I16[i]);
			__m128i mpixel = _mm_unpacklo_epi64(mpacked, mpacked);

			__m128i mx = _mm_sub_epi16(mpixel, mtx);
			__m128i my = _mm_sub_epi16(mpixel, mty);
			__m128i mz = _mm_sub_epi16(mpixel, mtz);
			__m128i mw = _mm_sub_epi16(mpixel, mtw);

			mx = _mm_abs_epi16(mx);
			my = _mm_abs_epi16(my);
			mz = _mm_abs_epi16(mz);
			mw = _mm_abs_epi16(mw);

			if constexpr (!kDenoise)
			{
				mx = _mm_adds_epu8(mx, mx);
				my = _mm_adds_epu8(my, my);
				mz = _mm_adds_epu8(mz, mz);
				mw = _mm_adds_epu8(mw, mw);
			}

			mx = _mm_srli_epi16(mx, kDenoiseShift);
			my = _mm_srli_epi16(my, kDenoiseShift);
			mz = _mm_srli_epi16(mz, kDenoiseShift);
			mw = _mm_srli_epi16(mw, kDenoiseShift);

			mx = _mm_mullo_epi16(mx, mx);
			my = _mm_mullo_epi16(my, my);
			mz = _mm_mullo_epi16(mz, mz);
			mw = _mm_mullo_epi16(mw, mw);

			mx = _mm_madd_epi16(mx, mweights);
			my = _mm_madd_epi16(my, mweights);
			mz = _mm_madd_epi16(mz, mweights);
			mw = _mm_madd_epi16(mw, mweights);

			mx = _mm_add_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(2, 3, 0, 1)));
			my = _mm_add_epi32(my, _mm_shuffle_epi32(my, _MM_SHUFFLE(2, 3, 0, 1)));
			mz = _mm_add_epi32(mz, _mm_shuffle_epi32(mz, _MM_SHUFFLE(2, 3, 0, 1)));
			mw = _mm_add_epi32(mw, _mm_shuffle_epi32(mw, _MM_SHUFFLE(2, 3, 0, 1)));

			mx = _mm_min_epi32(mx, my);
			mz = _mm_min_epi32(mz, mw);
			__m128i mx0 = _mm_min_epi32(mx, mz);

			mx = _mm_sub_epi16(mpixel, mrx);
			my = _mm_sub_epi16(mpixel, mry);
			mz = _mm_sub_epi16(mpixel, mrz);
			mw = _mm_sub_epi16(mpixel, mrw);

			mx = _mm_abs_epi16(mx);
			my = _mm_abs_epi16(my);
			mz = _mm_abs_epi16(mz);
			mw = _mm_abs_epi16(mw);

			if constexpr (!kDenoise)
			{
				mx = _mm_adds_epu8(mx, mx);
				my = _mm_adds_epu8(my, my);
				mz = _mm_adds_epu8(mz, mz);
				mw = _mm_adds_epu8(mw, mw);
			}

			mx = _mm_srli_epi16(mx, kDenoiseShift);
			my = _mm_srli_epi16(my, kDenoiseShift);
			mz = _mm_srli_epi16(mz, kDenoiseShift);
			mw = _mm_srli_epi16(mw, kDenoiseShift);

			mx = _mm_mullo_epi16(mx, mx);
			my = _mm_mullo_epi16(my, my);
			mz = _mm_mullo_epi16(mz, mz);
			mw = _mm_mullo_epi16(mw, mw);

			mx = _mm_madd_epi16(mx, mweights);
			my = _mm_madd_epi16(my, mweights);
			mz = _mm_madd_epi16(mz, mweights);
			mw = _mm_madd_epi16(mw, mweights);

			mx = _mm_add_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(2, 3, 0, 1)));
			my = _mm_add_epi32(my, _mm_shuffle_epi32(my, _MM_SHUFFLE(2, 3, 0, 1)));
			mz = _mm_add_epi32(mz, _mm_shuffle_epi32(mz, _MM_SHUFFLE(2, 3, 0, 1)));
			mw = _mm_add_epi32(mw, _mm_shuffle_epi32(mw, _MM_SHUFFLE(2, 3, 0, 1)));

			mx = _mm_min_epi32(mx, my);
			mz = _mm_min_epi32(mz, mw);
			mx = _mm_min_epi32(mx, mz);
			mx = _mm_min_epi32(mx, mx0);
			mx = _mm_min_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, mx);

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				break;
		}
#endif

		return _mm_cvtsi128_si32(merrorBlock);
	}

	template<int shuffle>
	static INLINED int ComputeSubsetError4Pair(const Area& area, __m128i mc, const __m128i mweights, const __m128i mwater) noexcept
	{
		__m128i merrorBlock = _mm_setzero_si128();

#if defined(OPTION_AVX512)
		const __m512i wweights = _mm512_broadcastq_epi64(mweights);

		const __m512i whalf = _mm512_set1_epi16(32);

		mc = _mm_shuffle_epi32(mc, shuffle);
		mc = _mm_packus_epi16(mc, mc);
		__m512i wc = _mm512_broadcastq_epi64(mc);

		__m512i wt = *(const __m512i*)gTableInterpolate4GR_U8;

		wt = _mm512_maddubs_epi16(wc, wt);

		wt = _mm512_add_epi16(wt, whalf);

		wt = _mm512_srli_epi16(wt, 6);

		__m512i wt0 = _mm512_inserti64x4(wt, _mm512_castsi512_si256(wt), 1);
		__m512i wt1 = _mm512_inserti64x4(wt, _mm512_extracti64x4_epi64(wt, 1), 0);

		__m512i wtx = _mm512_permutex_epi64(wt0, 0x44);
		__m512i wty = _mm512_permutex_epi64(wt0, 0xEE);
		__m512i wtz = _mm512_permutex_epi64(wt1, 0x44);
		__m512i wtw = _mm512_permutex_epi64(wt1, 0xEE);

		int k = static_cast<int>(area.Active);
		const __m512i* p = (const __m512i*)area.DataMask_I16;

		while ((k -= 4) >= 0)
		{
			__m512i wpacked = _mm512_load_epi64(p);
			__m512i wpixel = _mm512_shufflelo_epi16(wpacked, shuffle);
			wpixel = _mm512_unpacklo_epi64(wpixel, wpixel);

			__m512i wx = _mm512_sub_epi16(wpixel, wtx);
			__m512i wy = _mm512_sub_epi16(wpixel, wty);
			__m512i wz = _mm512_sub_epi16(wpixel, wtz);
			__m512i ww = _mm512_sub_epi16(wpixel, wtw);

			wx = _mm512_abs_epi16(wx);
			wy = _mm512_abs_epi16(wy);
			wz = _mm512_abs_epi16(wz);
			ww = _mm512_abs_epi16(ww);

			if constexpr (!kDenoise)
			{
				wx = _mm512_adds_epu8(wx, wx);
				wy = _mm512_adds_epu8(wy, wy);
				wz = _mm512_adds_epu8(wz, wz);
				ww = _mm512_adds_epu8(ww, ww);
			}

			wx = _mm512_srli_epi16(wx, kDenoiseShift);
			wy = _mm512_srli_epi16(wy, kDenoiseShift);
			wz = _mm512_srli_epi16(wz, kDenoiseShift);
			ww = _mm512_srli_epi16(ww, kDenoiseShift);

			wx = _mm512_mullo_epi16(wx, wx);
			wy = _mm512_mullo_epi16(wy, wy);
			wz = _mm512_mullo_epi16(wz, wz);
			ww = _mm512_mullo_epi16(ww, ww);

			wx = _mm512_madd_epi16(wx, wweights);
			wy = _mm512_madd_epi16(wy, wweights);
			wz = _mm512_madd_epi16(wz, wweights);
			ww = _mm512_madd_epi16(ww, wweights);

			wx = _mm512_min_epi32(_mm512_min_epi32(wx, wy), _mm512_min_epi32(wz, ww));
			wx = _mm512_min_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(2, 3, 0, 1)));
			wx = _mm512_min_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(1, 0, 3, 2)));

			__m256i vx = _mm256_add_epi32(_mm512_extracti64x4_epi64(wx, 1), _mm512_castsi512_si256(wx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p++;

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				goto done;
		}

		if (k & 2)
		{
			__m256i vpacked = _mm256_load_si256((const __m256i*)p);
			__m256i vpixel = _mm256_shufflelo_epi16(vpacked, shuffle);
			vpixel = _mm256_unpacklo_epi64(vpixel, vpixel);

			__m256i vx = _mm256_sub_epi16(vpixel, _mm512_castsi512_si256(wtx));
			__m256i vy = _mm256_sub_epi16(vpixel, _mm512_castsi512_si256(wty));
			__m256i vz = _mm256_sub_epi16(vpixel, _mm512_castsi512_si256(wtz));
			__m256i vw = _mm256_sub_epi16(vpixel, _mm512_castsi512_si256(wtw));

			vx = _mm256_abs_epi16(vx);
			vy = _mm256_abs_epi16(vy);
			vz = _mm256_abs_epi16(vz);
			vw = _mm256_abs_epi16(vw);

			if constexpr (!kDenoise)
			{
				vx = _mm256_adds_epu8(vx, vx);
				vy = _mm256_adds_epu8(vy, vy);
				vz = _mm256_adds_epu8(vz, vz);
				vw = _mm256_adds_epu8(vw, vw);
			}

			vx = _mm256_srli_epi16(vx, kDenoiseShift);
			vy = _mm256_srli_epi16(vy, kDenoiseShift);
			vz = _mm256_srli_epi16(vz, kDenoiseShift);
			vw = _mm256_srli_epi16(vw, kDenoiseShift);

			vx = _mm256_mullo_epi16(vx, vx);
			vy = _mm256_mullo_epi16(vy, vy);
			vz = _mm256_mullo_epi16(vz, vz);
			vw = _mm256_mullo_epi16(vw, vw);

			vx = _mm256_madd_epi16(vx, _mm512_castsi512_si256(wweights));
			vy = _mm256_madd_epi16(vy, _mm512_castsi512_si256(wweights));
			vz = _mm256_madd_epi16(vz, _mm512_castsi512_si256(wweights));
			vw = _mm256_madd_epi16(vw, _mm512_castsi512_si256(wweights));

			vx = _mm256_min_epi32(_mm256_min_epi32(vx, vy), _mm256_min_epi32(vz, vw));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p = reinterpret_cast<const __m512i*>(reinterpret_cast<const __m256i*>(p) + 1);
		}

		if (k & 1)
		{
			__m128i mpacked = _mm_load_si128((const __m128i*)p);
			__m128i mpixel = _mm_shufflelo_epi16(mpacked, shuffle);
			__m256i vpixel = _mm256_broadcastq_epi64(mpixel);

			__m256i vx = _mm256_sub_epi16(vpixel, _mm512_castsi512_si256(wt0));
			__m256i vy = _mm256_sub_epi16(vpixel, _mm512_castsi512_si256(wt1));

			vx = _mm256_abs_epi16(vx);
			vy = _mm256_abs_epi16(vy);

			if constexpr (!kDenoise)
			{
				vx = _mm256_adds_epu8(vx, vx);
				vy = _mm256_adds_epu8(vy, vy);
			}

			vx = _mm256_srli_epi16(vx, kDenoiseShift);
			vy = _mm256_srli_epi16(vy, kDenoiseShift);

			vx = _mm256_mullo_epi16(vx, vx);
			vy = _mm256_mullo_epi16(vy, vy);

			vx = _mm256_madd_epi16(vx, _mm512_castsi512_si256(wweights));
			vy = _mm256_madd_epi16(vy, _mm512_castsi512_si256(wweights));

			vx = _mm256_min_epi32(vx, vy);
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, _mm_min_epi32(_mm256_extracti128_si256(vx, 1), _mm256_castsi256_si128(vx)));
		}
	done:
#elif defined(OPTION_AVX2)
		const __m256i vweights = _mm256_broadcastq_epi64(mweights);

		const __m256i vhalf = _mm256_set1_epi16(32);

		mc = _mm_shuffle_epi32(mc, shuffle);
		mc = _mm_packus_epi16(mc, mc);
		__m256i vc = _mm256_broadcastq_epi64(mc);

		__m256i vt0 = *(const __m256i*)&gTableInterpolate4GR_U8[0];
		__m256i vt1 = *(const __m256i*)&gTableInterpolate4GR_U8[2];

		vt0 = _mm256_maddubs_epi16(vc, vt0);
		vt1 = _mm256_maddubs_epi16(vc, vt1);

		vt0 = _mm256_add_epi16(vt0, vhalf);
		vt1 = _mm256_add_epi16(vt1, vhalf);

		vt0 = _mm256_srli_epi16(vt0, 6);
		vt1 = _mm256_srli_epi16(vt1, 6);

		__m256i vtx = _mm256_permute4x64_epi64(vt0, 0x44);
		__m256i vty = _mm256_permute4x64_epi64(vt0, 0xEE);
		__m256i vtz = _mm256_permute4x64_epi64(vt1, 0x44);
		__m256i vtw = _mm256_permute4x64_epi64(vt1, 0xEE);

		int k = static_cast<int>(area.Active);
		const __m256i* p = (const __m256i*)area.DataMask_I16;

		while ((k -= 2) >= 0)
		{
			__m256i vpacked = _mm256_load_si256(p);
			__m256i vpixel = _mm256_shufflelo_epi16(vpacked, shuffle);
			vpixel = _mm256_unpacklo_epi64(vpixel, vpixel);

			__m256i vx = _mm256_sub_epi16(vpixel, vtx);
			__m256i vy = _mm256_sub_epi16(vpixel, vty);
			__m256i vz = _mm256_sub_epi16(vpixel, vtz);
			__m256i vw = _mm256_sub_epi16(vpixel, vtw);

			vx = _mm256_abs_epi16(vx);
			vy = _mm256_abs_epi16(vy);
			vz = _mm256_abs_epi16(vz);
			vw = _mm256_abs_epi16(vw);

			if constexpr (!kDenoise)
			{
				vx = _mm256_adds_epu8(vx, vx);
				vy = _mm256_adds_epu8(vy, vy);
				vz = _mm256_adds_epu8(vz, vz);
				vw = _mm256_adds_epu8(vw, vw);
			}

			vx = _mm256_srli_epi16(vx, kDenoiseShift);
			vy = _mm256_srli_epi16(vy, kDenoiseShift);
			vz = _mm256_srli_epi16(vz, kDenoiseShift);
			vw = _mm256_srli_epi16(vw, kDenoiseShift);

			vx = _mm256_mullo_epi16(vx, vx);
			vy = _mm256_mullo_epi16(vy, vy);
			vz = _mm256_mullo_epi16(vz, vz);
			vw = _mm256_mullo_epi16(vw, vw);

			vx = _mm256_madd_epi16(vx, vweights);
			vy = _mm256_madd_epi16(vy, vweights);
			vz = _mm256_madd_epi16(vz, vweights);
			vw = _mm256_madd_epi16(vw, vweights);

			vx = _mm256_min_epi32(_mm256_min_epi32(vx, vy), _mm256_min_epi32(vz, vw));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p++;

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				goto done;
		}

		if (k & 1)
		{
			__m128i mpacked = _mm_load_si128((const __m128i*)p);
			__m128i mpixel = _mm_shufflelo_epi16(mpacked, shuffle);
			__m256i vpixel = _mm256_broadcastq_epi64(mpixel);

			__m256i vx = _mm256_sub_epi16(vpixel, vt0);
			__m256i vy = _mm256_sub_epi16(vpixel, vt1);

			vx = _mm256_abs_epi16(vx);
			vy = _mm256_abs_epi16(vy);

			if constexpr (!kDenoise)
			{
				vx = _mm256_adds_epu8(vx, vx);
				vy = _mm256_adds_epu8(vy, vy);
			}

			vx = _mm256_srli_epi16(vx, kDenoiseShift);
			vy = _mm256_srli_epi16(vy, kDenoiseShift);

			vx = _mm256_mullo_epi16(vx, vx);
			vy = _mm256_mullo_epi16(vy, vy);

			vx = _mm256_madd_epi16(vx, vweights);
			vy = _mm256_madd_epi16(vy, vweights);

			vx = _mm256_min_epi32(vx, vy);
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, _mm_min_epi32(_mm256_extracti128_si256(vx, 1), _mm256_castsi256_si128(vx)));
		}
	done:
#else
		const __m128i mhalf = _mm_set1_epi16(32);

		mc = _mm_shuffle_epi32(mc, shuffle);
		mc = _mm_packus_epi16(mc, mc);

		__m128i mtx = gTableInterpolate4GR_U8[0];
		__m128i mty = gTableInterpolate4GR_U8[1];
		__m128i mtz = gTableInterpolate4GR_U8[2];
		__m128i mtw = gTableInterpolate4GR_U8[3];

		mtx = _mm_maddubs_epi16(mc, mtx);
		mty = _mm_maddubs_epi16(mc, mty);
		mtz = _mm_maddubs_epi16(mc, mtz);
		mtw = _mm_maddubs_epi16(mc, mtw);

		mtx = _mm_add_epi16(mtx, mhalf);
		mty = _mm_add_epi16(mty, mhalf);
		mtz = _mm_add_epi16(mtz, mhalf);
		mtw = _mm_add_epi16(mtw, mhalf);

		mtx = _mm_srli_epi16(mtx, 6);
		mty = _mm_srli_epi16(mty, 6);
		mtz = _mm_srli_epi16(mtz, 6);
		mtw = _mm_srli_epi16(mtw, 6);

		for (size_t i = 0, n = area.Active; i < n; i++)
		{
			__m128i mpacked = _mm_load_si128(&area.DataMask_I16[i]);
			__m128i mpixel = _mm_shufflelo_epi16(mpacked, shuffle);
			mpixel = _mm_unpacklo_epi64(mpixel, mpixel);

			__m128i mx = _mm_sub_epi16(mpixel, mtx);
			__m128i my = _mm_sub_epi16(mpixel, mty);
			__m128i mz = _mm_sub_epi16(mpixel, mtz);
			__m128i mw = _mm_sub_epi16(mpixel, mtw);

			mx = _mm_abs_epi16(mx);
			my = _mm_abs_epi16(my);
			mz = _mm_abs_epi16(mz);
			mw = _mm_abs_epi16(mw);

			if constexpr (!kDenoise)
			{
				mx = _mm_adds_epu8(mx, mx);
				my = _mm_adds_epu8(my, my);
				mz = _mm_adds_epu8(mz, mz);
				mw = _mm_adds_epu8(mw, mw);
			}

			mx = _mm_srli_epi16(mx, kDenoiseShift);
			my = _mm_srli_epi16(my, kDenoiseShift);
			mz = _mm_srli_epi16(mz, kDenoiseShift);
			mw = _mm_srli_epi16(mw, kDenoiseShift);

			mx = _mm_mullo_epi16(mx, mx);
			my = _mm_mullo_epi16(my, my);
			mz = _mm_mullo_epi16(mz, mz);
			mw = _mm_mullo_epi16(mw, mw);

			mx = _mm_madd_epi16(mx, mweights);
			my = _mm_madd_epi16(my, mweights);
			mz = _mm_madd_epi16(mz, mweights);
			mw = _mm_madd_epi16(mw, mweights);

			mx = _mm_min_epi32(mx, my);
			mz = _mm_min_epi32(mz, mw);
			mx = _mm_min_epi32(mx, mz);
			mx = _mm_min_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(2, 3, 0, 1)));
			mx = _mm_min_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, mx);

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				break;
		}
#endif

		return _mm_cvtsi128_si32(merrorBlock);
	}

	static INLINED BlockError ComputeSubsetTable4(const Area& area, __m128i mc, uint64_t& indices) noexcept
	{
		const __m128i mhalf = _mm_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);

		__m128i mtx = gTableInterpolate4_U8[0];
		__m128i mty = gTableInterpolate4_U8[1];
		__m128i mtz = gTableInterpolate4_U8[2];
		__m128i mtw = gTableInterpolate4_U8[3];
		__m128i mrx = gTableInterpolate4_U8[4];
		__m128i mry = gTableInterpolate4_U8[5];
		__m128i mrz = gTableInterpolate4_U8[6];
		__m128i mrw = gTableInterpolate4_U8[7];

		mtx = _mm_maddubs_epi16(mc, mtx);
		mty = _mm_maddubs_epi16(mc, mty);
		mtz = _mm_maddubs_epi16(mc, mtz);
		mtw = _mm_maddubs_epi16(mc, mtw);
		mrx = _mm_maddubs_epi16(mc, mrx);
		mry = _mm_maddubs_epi16(mc, mry);
		mrz = _mm_maddubs_epi16(mc, mrz);
		mrw = _mm_maddubs_epi16(mc, mrw);

		mtx = _mm_add_epi16(mtx, mhalf);
		mty = _mm_add_epi16(mty, mhalf);
		mtz = _mm_add_epi16(mtz, mhalf);
		mtw = _mm_add_epi16(mtw, mhalf);
		mrx = _mm_add_epi16(mrx, mhalf);
		mry = _mm_add_epi16(mry, mhalf);
		mrz = _mm_add_epi16(mrz, mhalf);
		mrw = _mm_add_epi16(mrw, mhalf);

		mtx = _mm_srli_epi16(mtx, 6);
		mty = _mm_srli_epi16(mty, 6);
		mtz = _mm_srli_epi16(mtz, 6);
		mtw = _mm_srli_epi16(mtw, 6);
		mrx = _mm_srli_epi16(mrx, 6);
		mry = _mm_srli_epi16(mry, 6);
		mrz = _mm_srli_epi16(mrz, 6);
		mrw = _mm_srli_epi16(mrw, 6);

		Modulations state;
		_mm_store_si128((__m128i*)&state.Values_I16[0], mtx);
		_mm_store_si128((__m128i*)&state.Values_I16[2], mty);
		_mm_store_si128((__m128i*)&state.Values_I16[4], mtz);
		_mm_store_si128((__m128i*)&state.Values_I16[6], mtw);
		_mm_store_si128((__m128i*)&state.Values_I16[8], mrx);
		_mm_store_si128((__m128i*)&state.Values_I16[10], mry);
		_mm_store_si128((__m128i*)&state.Values_I16[12], mrz);
		_mm_store_si128((__m128i*)&state.Values_I16[14], mrw);

		int error = ComputeSubsetTable4(area, gWeightsAGRB, state);

		for (size_t i = 0; i < 16; i++)
		{
			uint64_t index = static_cast<uint32_t>(state.Best[i]);
			indices |= index << (area.Indices[i] << 2);
		}

		int errorAlpha = 0;
		for (size_t i = 0; i < 16; i++)
		{
			int da = *(const uint16_t*)&state.Values_I16[state.Best[i]] - *(const uint16_t*)&area.DataMask_I16[i];

			da = (da < 0) ? -da : da;
			if constexpr (!kDenoise)
			{
				da = (da > 0x7F) ? 0x7F : da;
			}
			else
			{
				da >>= kDenoise;
			}

			errorAlpha += da * da;
		}

		return BlockError(errorAlpha * kAlpha, error);
	}

	void FinalPackBlock(uint8_t output[16], Cell& input) noexcept
	{
		Area& area = input.Area1;

		__m128i mc = input.BestColor0;

		uint64_t indices = 0;
		input.Error = ComputeSubsetTable4(area, mc, indices);
		AreaReduceTable4(mc, indices);

		ComposeBlock(output, mc, indices);
	}

	static INLINED int CompressSubsetFast(const Area& area, __m128i& mc, int water) noexcept
	{
		mc = area.Bounds_U16;

		const __m128i m1 = _mm_set1_epi16(1);
		__m128i mpbits = _mm_shuffle_epi32(_mm_and_si128(m1, mc), 0);

		const __m128i mh7 = _mm_set1_epi16(0xFE);
		mc = _mm_and_si128(mc, mh7);

		mc = _mm_or_si128(mc, mpbits);

#if defined(OPTION_COUNTERS)
		gComputeSubsetError4++;
#endif

		const int ea = ComputeSubsetTransparentError4(area, _mm_extract_epi16(_mm_packus_epi16(mc, mc), 0));
		if (ea >= water)
			return water;

		return ComputeSubsetError4(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water - ea)) + ea;
	}

	void CompressBlockFast(Cell& input) noexcept
	{
		const int denoiseStep = input.DenoiseStep;

		__m128i mc = _mm_setzero_si128();

		int error = denoiseStep;
		if (error < input.Error.Total)
		{
			Area& area = input.Area1;

			error += CompressSubsetFast(area, mc, input.Error.Total - error);
		}

		if (input.Error.Total > error)
		{
			input.Error.Total = error - denoiseStep;

			input.BestColor0 = mc;
			input.BestMode = 6;
		}
	}

	class Subset final
	{
	public:
		LevelsBufferHalf<LevelsCapacity> ch0, ch1, ch2, ch3;

		ALWAYS_INLINED Subset() noexcept = default;

		template<int pbits>
		INLINED bool InitLevels(const Area& area, const int water) noexcept
		{
			if (area.IsOpaque)
			{
				ch0.SetZeroError(0xFFFF);
			}
			else
			{
				ch0.ComputeChannelLevelsReduced<7, pbits, false, gTableDeltas4Half_Value8>(area, 0, kAlpha, water);
			}
			int min0 = ch0.MinErr;
			if (min0 >= water)
				return false;

			ch1.ComputeChannelLevelsReduced<7, pbits, true, gTableDeltas4Half_Value8>(area, 1, kGreen, water - min0);
			int min1 = ch1.MinErr;
			if (min0 + min1 >= water)
				return false;

			ch2.ComputeChannelLevelsReduced<7, pbits, true, gTableDeltas4Half_Value8>(area, 2, kRed, water - min0 - min1);
			int min2 = ch2.MinErr;
			if (min0 + min1 + min2 >= water)
				return false;

			ch3.ComputeChannelLevelsReduced<7, pbits, true, gTableDeltas4Half_Value8>(area, 3, kBlue, water - min0 - min1 - min2);
			int min3 = ch3.MinErr;
			if (min0 + min1 + min2 + min3 >= water)
				return false;

			return true;
		}

		INLINED int TryVariants(const Area& area, __m128i& best_color, int water) noexcept
		{
			int min0 = ch0.MinErr;
			int min1 = ch1.MinErr;
			int min2 = ch2.MinErr;
			int min3 = ch3.MinErr;
			if (min0 + min1 + min2 + min3 >= water)
				return water;

			int n0 = ch0.Count;
			int n1 = ch1.Count;
			int n2 = ch2.Count;
			int n3 = ch3.Count;

			int memAR[LevelsCapacity];
			int memAGB[LevelsCapacity];

			for (int i0 = 0; i0 < n0; i0++)
			{
				int e0 = ch0.Err[i0].Error;
				if (e0 + min1 + min2 + min3 >= water)
					break;

				int c0 = ch0.Err[i0].Color;

				const int ea = ComputeSubsetTransparentError4(area, c0);

				for (int i = 0; i < n2; i++)
				{
					memAR[i] = -1;
				}

				for (int i1 = 0; i1 < n1; i1++)
				{
					int e1 = ch1.Err[i1].Error + e0;
					if (e1 + min2 + min3 >= water)
						break;

					int c1 = ch1.Err[i1].Color;

					if (!area.IsOpaque)
					{
						__m128i mc = _mm_setzero_si128();
						mc = _mm_insert_epi16(mc, c0, 0);
						mc = _mm_insert_epi16(mc, c1, 1);
						mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
						gComputeSubsetError4AG++;
#endif
						e1 = ComputeSubsetError4Pair<_MM_SHUFFLE(1, 0, 1, 0)>(area, mc, gWeightsAGAG, _mm_cvtsi32_si128(water - ea - min2 - min3)) + ea;
						if (e1 + min2 + min3 >= water)
							continue;
					}

					for (int i = 0; i < n3; i++)
					{
						memAGB[i] = -1;
					}

					for (int i2 = 0; i2 < n2; i2++)
					{
						int e2 = ch2.Err[i2].Error + e1;
						if (e2 + min3 >= water)
							break;

						int c2 = ch2.Err[i2].Color;

						if (!area.IsOpaque)
						{
							int ear = memAR[i2];
							if (ear < 0)
							{
								__m128i mc = _mm_setzero_si128();
								mc = _mm_insert_epi16(mc, c0, 0);
								mc = _mm_insert_epi16(mc, c2, 2);
								mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
								gComputeSubsetError4AR++;
#endif
								ear = ComputeSubsetError4Pair<_MM_SHUFFLE(2, 0, 2, 0)>(area, mc, gWeightsARAR, _mm_cvtsi32_si128(water - ea - min1 - min3)) + ea;
								memAR[i2] = ear;
							}
							if (ear + min1 + min3 >= water)
								continue;
						}

						{
							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, c0, 0);
							mc = _mm_insert_epi16(mc, c1, 1);
							mc = _mm_insert_epi16(mc, c2, 2);
							mc = _mm_cvtepu8_epi16(mc);

							if (area.IsOpaque)
							{
#if defined(OPTION_COUNTERS)
								gComputeSubsetError4GR++;
#endif
								e2 = ComputeSubsetError4Pair<_MM_SHUFFLE(2, 1, 2, 1)>(area, mc, gWeightsGRGR, _mm_cvtsi32_si128(water - ea - min3)) + ea;
							}
							else
							{
#if defined(OPTION_COUNTERS)
								gComputeSubsetError4AGR++;
#endif
								e2 = ComputeSubsetError4(area, mc, gWeightsAGR, _mm_cvtsi32_si128(water - ea - min3)) + ea;
							}
							if (e2 + min3 >= water)
								continue;
						}

						for (int i3 = 0; i3 < n3; i3++)
						{
							int e3 = ch3.Err[i3].Error + e2;
							if (e3 >= water)
								break;

							int c3 = ch3.Err[i3].Color;

							int eagb = memAGB[i3];
							if (eagb < 0)
							{
								__m128i mc = _mm_setzero_si128();
								mc = _mm_insert_epi16(mc, c0, 0);
								mc = _mm_insert_epi16(mc, c1, 1);
								mc = _mm_insert_epi16(mc, c3, 3);
								mc = _mm_cvtepu8_epi16(mc);

								if (area.IsOpaque)
								{
#if defined(OPTION_COUNTERS)
									gComputeSubsetError4GB++;
#endif
									eagb = ComputeSubsetError4Pair<_MM_SHUFFLE(3, 1, 3, 1)>(area, mc, gWeightsGBGB, _mm_cvtsi32_si128(water - ea - min2)) + ea;
								}
								else
								{
#if defined(OPTION_COUNTERS)
									gComputeSubsetError4AGB++;
#endif
									eagb = ComputeSubsetError4(area, mc, gWeightsAGB, _mm_cvtsi32_si128(water - ea - min2)) + ea;
								}
								memAGB[i3] = eagb;
							}
							if (eagb + min2 >= water)
								continue;

							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, c0, 0);
							mc = _mm_insert_epi16(mc, c1, 1);
							mc = _mm_insert_epi16(mc, c2, 2);
							mc = _mm_insert_epi16(mc, c3, 3);
							mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
							gComputeSubsetError4++;
#endif
							int err = ComputeSubsetError4(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water - ea)) + ea;

							if (water > err)
							{
								water = err;

								best_color = mc;
							}
						}
					}
				}
			}

			return water;
		}
	};

	static INLINED int CompressSubset(const Area& area, __m128i& mc, int water)
	{
		Subset subset3;
		if (subset3.InitLevels<0x0101>(area, water))
		{
			water = subset3.TryVariants(area, mc, water);
		}

		if (!area.IsOpaque)
		{
			Subset subset0;
			if (subset0.InitLevels<0>(area, water))
			{
				water = subset0.TryVariants(area, mc, water);
			}

			Subset subset1;
			if (subset1.InitLevels<1>(area, water))
			{
				water = subset1.TryVariants(area, mc, water);
			}

			Subset subset2;
			if (subset2.InitLevels<1 << 8>(area, water))
			{
				water = subset2.TryVariants(area, mc, water);
			}
		}

		return water;
	}

	void CompressBlock(Cell& input) noexcept
	{
		Area& area = input.Area1;

		__m128i mc = input.BestColor0;

		int error = CompressSubset(area, mc, input.Error.Total);

		if (input.Error.Total > error)
		{
			input.Error.Total = error;

			input.BestColor0 = mc;
			//input.BestMode = 6;
		}
	}

	static INLINED int EstimateBest(Area& area) noexcept
	{
#if defined(OPTION_PCA)
		if (area.IsOpaque)
		{
			return PrincipalComponentAnalysis3(area);
		}

		return PrincipalComponentAnalysis4(area);
#else
		(void)area;
		return 0;
#endif
	}

	void CompressBlockFull(Cell& input) noexcept
	{
		if (input.PersonalMode == 6)
			return;

		const int denoiseStep = input.DenoiseStep;

		Area& area = input.Area1;

		int line = EstimateBest(area);
		if (line < input.Error.Total - denoiseStep)
		{
			__m128i mc = _mm_setzero_si128();

			int error = CompressSubset(area, mc, input.Error.Total - denoiseStep);

			if (input.Error.Total > error + denoiseStep)
			{
				input.Error.Total = error;

				input.BestColor0 = mc;
				input.BestMode = 6;
			}
		}
	}

	void PrintCounters() noexcept
	{
#if defined(OPTION_COUNTERS)
		PRINTF("[Mode 6]\tAG4 = %i, AR4 = %i, GR4 = %i, AGR4 = %i, GB4 = %i, AGB4 = %i, AGRB4 = %i",
			gComputeSubsetError4AG.load(), gComputeSubsetError4AR.load(),
			gComputeSubsetError4GR.load(), gComputeSubsetError4AGR.load(),
			gComputeSubsetError4GB.load(), gComputeSubsetError4AGB.load(),
			gComputeSubsetError4.load());
#endif
	}

} // namespace Mode6
