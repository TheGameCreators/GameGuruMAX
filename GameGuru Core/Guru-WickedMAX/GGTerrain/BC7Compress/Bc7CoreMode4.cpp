
#include "pch.h"
#include "Bc7Core.h"
#include "Bc7Tables.h"

#include "SnippetInsertRemoveZeroBit.h"
#include "SnippetHorizontalSum4.h"
#include "SnippetLevelsBuffer.h"

// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/bc7-format-mode-reference#mode-4

namespace Mode4 {

	constexpr int LevelsCapacity = 32;

#if defined(OPTION_COUNTERS)
	static std::atomic_int gComputeSubsetError23[4], gComputeSubsetError23AG[4], gComputeSubsetError23AR[4], gComputeSubsetError23AGR[4], gComputeSubsetError23AGB[4];
	static std::atomic_int gComputeSubsetError32[4], gComputeSubsetError32AG[4], gComputeSubsetError32AR[4], gComputeSubsetError32AGR[4], gComputeSubsetError32AGB[4];
#endif

	static ALWAYS_INLINED __m128i GetRotationShuffle(int rotation) noexcept
	{
		__m128i mrot = _mm_set_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);

		if (rotation & 2)
		{
			if (rotation & 1)
			{
				mrot = _mm_shuffle_epi32(mrot, _MM_SHUFFLE(0, 2, 1, 3));
			}
			else
			{
				mrot = _mm_shuffle_epi32(mrot, _MM_SHUFFLE(3, 2, 0, 1));
			}
		}
		else
		{
			if (rotation & 1)
			{
				mrot = _mm_shuffle_epi32(mrot, _MM_SHUFFLE(3, 0, 1, 2));
			}
		}

		return mrot;
	}

	static ALWAYS_INLINED __m128i GetRotationShuffleNarrow(int rotation) noexcept
	{
		__m128i mrot = _mm_set_epi8(15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);

		if (rotation & 2)
		{
			if (rotation & 1)
			{
				mrot = _mm_shufflelo_epi16(mrot, _MM_SHUFFLE(0, 2, 1, 3));
				mrot = _mm_shufflehi_epi16(mrot, _MM_SHUFFLE(0, 2, 1, 3));
			}
			else
			{
				mrot = _mm_shufflelo_epi16(mrot, _MM_SHUFFLE(3, 2, 0, 1));
				mrot = _mm_shufflehi_epi16(mrot, _MM_SHUFFLE(3, 2, 0, 1));
			}
		}
		else
		{
			if (rotation & 1)
			{
				mrot = _mm_shufflelo_epi16(mrot, _MM_SHUFFLE(3, 0, 1, 2));
				mrot = _mm_shufflehi_epi16(mrot, _MM_SHUFFLE(3, 0, 1, 2));
			}
		}

		return mrot;
	}

	static INLINED void DecompressSubset(__m128i mc, int* output, uint64_t data2, uint64_t data3, const int rotation) noexcept
	{
		const __m128i msel = (rotation & 4) ? _mm_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0) : _mm_set_epi16(0, 0, 0, -1, 0, 0, 0, -1);
		const __m128i mrot = GetRotationShuffleNarrow(rotation);
		const __m128i mhalf = _mm_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);

		for (size_t i = 0; i < 16; i++)
		{
			__m128i mratio3 = _mm_loadl_epi64((const __m128i*)&((const uint64_t*)gTableInterpolate3_U8)[data3 & 7]); data3 >>= 3;
			__m128i mratio2 = _mm_loadl_epi64((const __m128i*)&((const uint64_t*)gTableInterpolate2_U8)[data2 & 3]); data2 >>= 2;
			__m128i mratio = _mm_blendv_epi8(mratio2, mratio3, msel);

			__m128i mx = _mm_maddubs_epi16(mc, mratio);
			mx = _mm_add_epi16(mx, mhalf);
			mx = _mm_srli_epi16(mx, 6);

			mx = _mm_shuffle_epi8(mx, mrot);

			output[i] = _mm_cvtsi128_si32(_mm_packus_epi16(mx, mx));
		}
	}

	void DecompressBlock(uint8_t input[16], Cell& output) noexcept
	{
		uint64_t data0 = *(const uint64_t*)&input[0];
		uint64_t data1 = *(const uint64_t*)&input[8];

		data0 >>= 5;

		int rotation = static_cast<int>(data0 & 7); data0 >>= 3;

		__m128i mc0 = _mm_setzero_si128();

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 4); data0 >>= 5;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 5); data0 >>= 5;

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 2); data0 >>= 5;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 3); data0 >>= 5;

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 6); data0 >>= 5;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 7); data0 >>= 5;

		mc0 = _mm_slli_epi16(mc0, 3);

		const __m128i m8 = _mm_set1_epi16(0xFF);
		mc0 = _mm_and_si128(mc0, m8);

		mc0 = _mm_or_si128(mc0, _mm_srli_epi16(mc0, 5));

		__m128i ma0 = _mm_setzero_si128();

		ma0 = _mm_insert_epi16(ma0, static_cast<int>(data0), 0); data0 >>= 6;
		ma0 = _mm_insert_epi16(ma0, static_cast<int>(data0), 1); data0 >>= 6;

		ma0 = _mm_slli_epi16(ma0, 2);

		ma0 = _mm_and_si128(ma0, m8);

		mc0 = _mm_or_si128(mc0, ma0);
		ma0 = _mm_srli_epi16(ma0, 6);
		mc0 = _mm_or_si128(mc0, ma0);

		uint64_t data2 = data0 + (data1 << 14);
		uint64_t data3 = data1 >> (31 - 14);

		data2 = InsertZeroBit(data2, 1);
		data3 = InsertZeroBit(data3, 2);

		DecompressSubset(mc0, (int*)output.ImageRows_U8, data2, data3, rotation);

		output.BestColor0 = mc0;
		output.BestColor1 = _mm_setzero_si128();
		output.BestColor2 = _mm_setzero_si128();
		output.BestParameter = rotation;
		output.BestMode = 4;
	}

	static INLINED void ComposeBlock(uint8_t output[16], __m128i mc0, uint64_t data2, uint64_t data3, const int rotation) noexcept
	{
		uint64_t data1 = RemoveZeroBit(data3, 2);
		data1 <<= (64 - 47); data1 |= data2 >> (32 - 17);
		uint64_t data0 = RemoveZeroBit(data2, 1) & ((1uLL << 14) - 1uLL);

		mc0 = _mm_srli_epi16(mc0, 2);

		data0 <<= 6; data0 |= _mm_extract_epi16(mc0, 1);
		data0 <<= 6; data0 |= _mm_extract_epi16(mc0, 0);

		mc0 = _mm_srli_epi16(mc0, 1);

		data0 <<= 5; data0 |= _mm_extract_epi16(mc0, 7);
		data0 <<= 5; data0 |= _mm_extract_epi16(mc0, 6);

		data0 <<= 5; data0 |= _mm_extract_epi16(mc0, 3);
		data0 <<= 5; data0 |= _mm_extract_epi16(mc0, 2);

		data0 <<= 5; data0 |= _mm_extract_epi16(mc0, 5);
		data0 <<= 5; data0 |= _mm_extract_epi16(mc0, 4);

		data0 <<= 3; data0 |= rotation;

		data0 <<= 5; data0 |= 1 << 4;

		*(uint64_t*)&output[0] = data0;
		*(uint64_t*)&output[8] = data1;
	}

	static INLINED int ComputeSubsetError23(const Area& area, __m128i mc, __m128i mweights, const __m128i mwater, const int rotation) noexcept
	{
		__m128i merrorBlock = _mm_setzero_si128();

#if defined(OPTION_AVX512)
		const __m512i wrot = _mm512_broadcast_i32x4(GetRotationShuffleNarrow(rotation));
		const __m512i whalf = _mm512_set1_epi16(32);
		const __m512i wweights = _mm512_shuffle_epi8(_mm512_broadcastq_epi64(mweights), wrot);

		mc = _mm_packus_epi16(mc, mc);
		__m512i wc = _mm512_broadcastq_epi64(mc);

		const __m512i wmask3 = _mm512_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0);
		const __m512i wweights3 = _mm512_and_epi32(wweights, wmask3);
		const __m512i wweights1 = _mm512_andnot_epi32(wmask3, wweights);

		__m512i wt = *(const __m512i*)gTableInterpolate2n3_U8;

		wt = _mm512_maddubs_epi16(wc, wt);

		wt = _mm512_add_epi16(wt, whalf);

		wt = _mm512_srli_epi16(wt, 6);

		__m512i wt0 = _mm512_inserti64x4(wt, _mm512_castsi512_si256(wt), 1);
		__m512i wt1 = _mm512_inserti64x4(wt, _mm512_extracti64x4_epi64(wt, 1), 0);

		__m512i wtx = _mm512_permutex_epi64(wt0, 0x44);
		__m512i wty = _mm512_permutex_epi64(wt0, 0xEE);
		__m512i wtz = _mm512_permutex_epi64(wt1, 0x44);
		__m512i wtw = _mm512_permutex_epi64(wt1, 0xEE);

		const __m512i* p = (const __m512i*)area.DataMask_I16;

		size_t i = 0;
		for (size_t k = area.Active; i < k; i += 4)
		{
			__m512i wpacked = _mm512_load_epi64(p);
			wpacked = _mm512_shuffle_epi8(wpacked, wrot);

			__m512i wpixel = _mm512_unpacklo_epi64(wpacked, wpacked);
			__m512i wmask = _mm512_unpackhi_epi64(wpacked, wpacked);

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

			wx = _mm512_and_epi32(wx, wmask);
			wy = _mm512_and_epi32(wy, wmask);
			wz = _mm512_and_epi32(wz, wmask);
			ww = _mm512_and_epi32(ww, wmask);

			__m512i wa = _mm512_min_epi16(_mm512_min_epi16(wx, wy), _mm512_min_epi16(wz, ww));

			wx = _mm512_madd_epi16(wx, wweights3);
			wy = _mm512_madd_epi16(wy, wweights3);
			wa = _mm512_madd_epi16(wa, wweights1);

			wx = _mm512_add_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(2, 3, 0, 1)));
			wy = _mm512_add_epi32(wy, _mm512_shuffle_epi32(wy, _MM_SHUFFLE(2, 3, 0, 1)));

			wx = _mm512_min_epi32(wx, wy);
			wa = _mm512_min_epi32(wa, _mm512_shuffle_epi32(wa, _MM_SHUFFLE(1, 0, 3, 2)));
			wx = _mm512_min_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(1, 0, 3, 2)));

			wx = _mm512_add_epi32(wx, wa);

			__m256i vx = _mm256_add_epi32(_mm512_extracti64x4_epi64(wx, 1), _mm512_castsi512_si256(wx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p++;

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				goto done;
		}
#elif defined(OPTION_AVX2)
		const __m256i vrot = _mm256_broadcastsi128_si256(GetRotationShuffleNarrow(rotation));
		const __m256i vhalf = _mm256_set1_epi16(32);
		const __m256i vweights = _mm256_shuffle_epi8(_mm256_broadcastq_epi64(mweights), vrot);

		mc = _mm_packus_epi16(mc, mc);
		__m256i vc = _mm256_broadcastq_epi64(mc);

		const __m256i vmask3 = _mm256_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0);
		const __m256i vweights3 = _mm256_and_si256(vweights, vmask3);
		const __m256i vweights1 = _mm256_andnot_si256(vmask3, vweights);

		__m256i vt0 = *(const __m256i*)&gTableInterpolate2n3_U8[0];
		__m256i vt1 = *(const __m256i*)&gTableInterpolate2n3_U8[2];

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

		const __m256i* p = (const __m256i*)area.DataMask_I16;

		size_t i = 0;
		for (size_t k = area.Active; i < k; i += 2)
		{
			__m256i vpacked = _mm256_load_si256(p);
			vpacked = _mm256_shuffle_epi8(vpacked, vrot);
			__m256i vpixel = _mm256_unpacklo_epi64(vpacked, vpacked);
			__m256i vmask = _mm256_unpackhi_epi64(vpacked, vpacked);

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

			vx = _mm256_and_si256(vx, vmask);
			vy = _mm256_and_si256(vy, vmask);
			vz = _mm256_and_si256(vz, vmask);
			vw = _mm256_and_si256(vw, vmask);

			__m256i va = _mm256_min_epi16(_mm256_min_epi16(vx, vy), _mm256_min_epi16(vz, vw));

			vx = _mm256_madd_epi16(vx, vweights3);
			vy = _mm256_madd_epi16(vy, vweights3);
			va = _mm256_madd_epi16(va, vweights1);

			vx = _mm256_add_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
			vy = _mm256_add_epi32(vy, _mm256_shuffle_epi32(vy, _MM_SHUFFLE(2, 3, 0, 1)));

			vx = _mm256_min_epi32(vx, vy);
			va = _mm256_min_epi32(va, _mm256_shuffle_epi32(va, _MM_SHUFFLE(1, 0, 3, 2)));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

			vx = _mm256_add_epi32(vx, va);

			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p++;

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				goto done;
		}
#else
		const __m128i mrot = GetRotationShuffleNarrow(rotation);
		const __m128i mhalf = _mm_set1_epi16(32);
		mweights = _mm_shuffle_epi8(mweights, mrot);

		mc = _mm_packus_epi16(mc, mc);

		const __m128i mmask3 = _mm_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0);
		const __m128i mweights3 = _mm_and_si128(mweights, mmask3);
		const __m128i mweights1 = _mm_andnot_si128(mmask3, mweights);

		__m128i mtx = gTableInterpolate2n3_U8[0];
		__m128i mty = gTableInterpolate2n3_U8[1];
		__m128i mtz = gTableInterpolate2n3_U8[2];
		__m128i mtw = gTableInterpolate2n3_U8[3];

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

		size_t i = 0;
		for (size_t k = area.Active; i < k; i++)
		{
			__m128i mpacked = _mm_load_si128(&area.DataMask_I16[i]);
			mpacked = _mm_shuffle_epi8(mpacked, mrot);
			__m128i mpixel = _mm_unpacklo_epi64(mpacked, mpacked);
			__m128i mmask = _mm_unpackhi_epi64(mpacked, mpacked);

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

			mx = _mm_and_si128(mx, mmask);
			my = _mm_and_si128(my, mmask);
			mz = _mm_and_si128(mz, mmask);
			mw = _mm_and_si128(mw, mmask);

			__m128i ma = _mm_min_epi16(_mm_min_epi16(mx, my), _mm_min_epi16(mz, mw));

			mx = _mm_madd_epi16(mx, mweights3);
			my = _mm_madd_epi16(my, mweights3);
			ma = _mm_madd_epi16(ma, mweights1);

			mx = _mm_add_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(2, 3, 0, 1)));
			my = _mm_add_epi32(my, _mm_shuffle_epi32(my, _MM_SHUFFLE(2, 3, 0, 1)));

			mx = _mm_min_epi32(mx, my);
			ma = _mm_min_epi32(ma, _mm_shuffle_epi32(ma, _MM_SHUFFLE(1, 0, 3, 2)));
			mx = _mm_min_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, ma);
			merrorBlock = _mm_add_epi32(merrorBlock, mx);

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				goto done;
		}
#endif

		if (i < 16)
		{
#if defined(OPTION_AVX512)
			__m128i mx = _mm_min_epi16(_mm512_castsi512_si128(wtx), _mm512_castsi512_si128(wtw));

			__m128i mweight = _mm_or_si128(_mm512_castsi512_si128(wweights3), _mm512_castsi512_si128(wweights1));

			mx = _mm_shuffle_epi8(mx, _mm512_castsi512_si128(wrot));

			mweight = _mm_shuffle_epi8(mweight, _mm512_castsi512_si128(wrot));
#elif defined(OPTION_AVX2)
			__m128i mx = _mm_min_epi16(_mm256_castsi256_si128(vtx), _mm256_castsi256_si128(vtw));

			__m128i mweight = _mm_or_si128(_mm256_castsi256_si128(vweights3), _mm256_castsi256_si128(vweights1));

			mx = _mm_shuffle_epi8(mx, _mm256_castsi256_si128(vrot));

			mweight = _mm_shuffle_epi8(mweight, _mm256_castsi256_si128(vrot));
#else
			__m128i mx = _mm_min_epi16(mtx, mtw);

			__m128i mweight = _mm_or_si128(mweights3, mweights1);

			mx = _mm_shuffle_epi8(mx, mrot);

			mweight = _mm_shuffle_epi8(mweight, mrot);
#endif

			mx = _mm_min_epi16(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(1, 0, 3, 2)));

			mweight = _mm_mullo_epi16(mweight, _mm_cvtsi32_si128(static_cast<int>(16 - i)));

			if constexpr (!kDenoise)
			{
				mx = _mm_adds_epu8(mx, mx);
			}

			mx = _mm_srli_epi16(mx, kDenoiseShift);

			mx = _mm_mullo_epi16(mx, mx);

			mx = _mm_madd_epi16(mx, mweight);

			merrorBlock = _mm_add_epi32(merrorBlock, mx);
		}

	done:
		return _mm_cvtsi128_si32(merrorBlock);
	}

	static INLINED int ComputeSubsetError32(const Area& area, __m128i mc, __m128i mweights, const __m128i mwater, const int rotation) noexcept
	{
		__m128i merrorBlock = _mm_setzero_si128();

#if defined(OPTION_AVX512)
		const __m512i wrot = _mm512_broadcast_i32x4(GetRotationShuffleNarrow(rotation));
		const __m512i whalf = _mm512_set1_epi16(32);
		const __m512i wweights = _mm512_shuffle_epi8(_mm512_broadcastq_epi64(mweights), wrot);

		mc = _mm_packus_epi16(mc, mc);
		__m512i wc = _mm512_broadcastq_epi64(mc);

		const __m512i wmask3 = _mm512_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0);
		const __m512i wweights3 = _mm512_and_epi32(wweights, wmask3);
		const __m512i wweights1 = _mm512_andnot_epi32(wmask3, wweights);

		__m512i wt = *(const __m512i*)gTableInterpolate3n2_U8;

		wt = _mm512_maddubs_epi16(wc, wt);

		wt = _mm512_add_epi16(wt, whalf);

		wt = _mm512_srli_epi16(wt, 6);

		__m512i wt0 = _mm512_inserti64x4(wt, _mm512_castsi512_si256(wt), 1);
		__m512i wt1 = _mm512_inserti64x4(wt, _mm512_extracti64x4_epi64(wt, 1), 0);

		__m512i wtx = _mm512_permutex_epi64(wt0, 0x44);
		__m512i wty = _mm512_permutex_epi64(wt0, 0xEE);
		__m512i wtz = _mm512_permutex_epi64(wt1, 0x44);
		__m512i wtw = _mm512_permutex_epi64(wt1, 0xEE);

		const __m512i* p = (const __m512i*)area.DataMask_I16;

		size_t i = 0;
		for (size_t k = area.Active; i < k; i += 4)
		{
			__m512i wpacked = _mm512_load_epi64(p);
			wpacked = _mm512_shuffle_epi8(wpacked, wrot);

			__m512i wpixel = _mm512_unpacklo_epi64(wpacked, wpacked);
			__m512i wmask = _mm512_unpackhi_epi64(wpacked, wpacked);

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

			wx = _mm512_and_epi32(wx, wmask);
			wy = _mm512_and_epi32(wy, wmask);
			wz = _mm512_and_epi32(wz, wmask);
			ww = _mm512_and_epi32(ww, wmask);

			__m512i wa = _mm512_min_epi16(wx, wy);

			wx = _mm512_madd_epi16(wx, wweights3);
			wy = _mm512_madd_epi16(wy, wweights3);
			wz = _mm512_madd_epi16(wz, wweights3);
			ww = _mm512_madd_epi16(ww, wweights3);
			wa = _mm512_madd_epi16(wa, wweights1);

			wx = _mm512_add_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(2, 3, 0, 1)));
			wy = _mm512_add_epi32(wy, _mm512_shuffle_epi32(wy, _MM_SHUFFLE(2, 3, 0, 1)));
			wz = _mm512_add_epi32(wz, _mm512_shuffle_epi32(wz, _MM_SHUFFLE(2, 3, 0, 1)));
			ww = _mm512_add_epi32(ww, _mm512_shuffle_epi32(ww, _MM_SHUFFLE(2, 3, 0, 1)));

			wx = _mm512_min_epi32(_mm512_min_epi32(wx, wy), _mm512_min_epi32(wz, ww));
			wa = _mm512_min_epi32(wa, _mm512_shuffle_epi32(wa, _MM_SHUFFLE(1, 0, 3, 2)));
			wx = _mm512_min_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(1, 0, 3, 2)));

			wx = _mm512_add_epi32(wx, wa);

			__m256i vx = _mm256_add_epi32(_mm512_extracti64x4_epi64(wx, 1), _mm512_castsi512_si256(wx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p++;

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				goto done;
		}
#elif defined(OPTION_AVX2)
		const __m256i vrot = _mm256_broadcastsi128_si256(GetRotationShuffleNarrow(rotation));
		const __m256i vhalf = _mm256_set1_epi16(32);
		const __m256i vweights = _mm256_shuffle_epi8(_mm256_broadcastq_epi64(mweights), vrot);

		mc = _mm_packus_epi16(mc, mc);
		__m256i vc = _mm256_broadcastq_epi64(mc);

		const __m256i vmask3 = _mm256_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0);
		const __m256i vweights3 = _mm256_and_si256(vweights, vmask3);
		const __m256i vweights1 = _mm256_andnot_si256(vmask3, vweights);

		__m256i vt0 = *(const __m256i*)&gTableInterpolate3n2_U8[0];
		__m256i vt1 = *(const __m256i*)&gTableInterpolate3n2_U8[2];

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

		const __m256i* p = (const __m256i*)area.DataMask_I16;

		size_t i = 0;
		for (size_t k = area.Active; i < k; i += 2)
		{
			__m256i vpacked = _mm256_load_si256(p);
			vpacked = _mm256_shuffle_epi8(vpacked, vrot);
			__m256i vpixel = _mm256_unpacklo_epi64(vpacked, vpacked);
			__m256i vmask = _mm256_unpackhi_epi64(vpacked, vpacked);

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

			vx = _mm256_and_si256(vx, vmask);
			vy = _mm256_and_si256(vy, vmask);
			vz = _mm256_and_si256(vz, vmask);
			vw = _mm256_and_si256(vw, vmask);

			__m256i va = _mm256_min_epi16(vx, vy);

			vx = _mm256_madd_epi16(vx, vweights3);
			vy = _mm256_madd_epi16(vy, vweights3);
			vz = _mm256_madd_epi16(vz, vweights3);
			vw = _mm256_madd_epi16(vw, vweights3);
			va = _mm256_madd_epi16(va, vweights1);

			vx = _mm256_add_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
			vy = _mm256_add_epi32(vy, _mm256_shuffle_epi32(vy, _MM_SHUFFLE(2, 3, 0, 1)));
			vz = _mm256_add_epi32(vz, _mm256_shuffle_epi32(vz, _MM_SHUFFLE(2, 3, 0, 1)));
			vw = _mm256_add_epi32(vw, _mm256_shuffle_epi32(vw, _MM_SHUFFLE(2, 3, 0, 1)));

			vx = _mm256_min_epi32(_mm256_min_epi32(vx, vy), _mm256_min_epi32(vz, vw));
			va = _mm256_min_epi32(va, _mm256_shuffle_epi32(va, _MM_SHUFFLE(1, 0, 3, 2)));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

			vx = _mm256_add_epi32(vx, va);

			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p++;

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				goto done;
		}
#else
		const __m128i mrot = GetRotationShuffleNarrow(rotation);
		const __m128i mhalf = _mm_set1_epi16(32);
		mweights = _mm_shuffle_epi8(mweights, mrot);

		mc = _mm_packus_epi16(mc, mc);

		const __m128i mmask3 = _mm_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0);
		const __m128i mweights3 = _mm_and_si128(mweights, mmask3);
		const __m128i mweights1 = _mm_andnot_si128(mmask3, mweights);

		__m128i mtx = gTableInterpolate3n2_U8[0];
		__m128i mty = gTableInterpolate3n2_U8[1];
		__m128i mtz = gTableInterpolate3n2_U8[2];
		__m128i mtw = gTableInterpolate3n2_U8[3];

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

		size_t i = 0;
		for (size_t k = area.Active; i < k; i++)
		{
			__m128i mpacked = _mm_load_si128(&area.DataMask_I16[i]);
			mpacked = _mm_shuffle_epi8(mpacked, mrot);
			__m128i mpixel = _mm_unpacklo_epi64(mpacked, mpacked);
			__m128i mmask = _mm_unpackhi_epi64(mpacked, mpacked);

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

			mx = _mm_and_si128(mx, mmask);
			my = _mm_and_si128(my, mmask);
			mz = _mm_and_si128(mz, mmask);
			mw = _mm_and_si128(mw, mmask);

			__m128i ma = _mm_min_epi16(mx, my);

			mx = _mm_madd_epi16(mx, mweights3);
			my = _mm_madd_epi16(my, mweights3);
			mz = _mm_madd_epi16(mz, mweights3);
			mw = _mm_madd_epi16(mw, mweights3);
			ma = _mm_madd_epi16(ma, mweights1);

			mx = _mm_add_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(2, 3, 0, 1)));
			my = _mm_add_epi32(my, _mm_shuffle_epi32(my, _MM_SHUFFLE(2, 3, 0, 1)));
			mz = _mm_add_epi32(mz, _mm_shuffle_epi32(mz, _MM_SHUFFLE(2, 3, 0, 1)));
			mw = _mm_add_epi32(mw, _mm_shuffle_epi32(mw, _MM_SHUFFLE(2, 3, 0, 1)));

			mx = _mm_min_epi32(_mm_min_epi32(mx, my), _mm_min_epi32(mz, mw));
			ma = _mm_min_epi32(ma, _mm_shuffle_epi32(ma, _MM_SHUFFLE(1, 0, 3, 2)));
			mx = _mm_min_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(1, 0, 3, 2)));

			merrorBlock = _mm_add_epi32(merrorBlock, ma);
			merrorBlock = _mm_add_epi32(merrorBlock, mx);

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				goto done;
		}
#endif

		if (i < 16)
		{
#if defined(OPTION_AVX512)
			__m128i mx = _mm_min_epi16(_mm512_castsi512_si128(wtx), _mm512_castsi512_si128(wtw));

			__m128i mweight = _mm_or_si128(_mm512_castsi512_si128(wweights3), _mm512_castsi512_si128(wweights1));

			mx = _mm_shuffle_epi8(mx, _mm512_castsi512_si128(wrot));

			mweight = _mm_shuffle_epi8(mweight, _mm512_castsi512_si128(wrot));
#elif defined(OPTION_AVX2)
			__m128i mx = _mm_min_epi16(_mm256_castsi256_si128(vtx), _mm256_castsi256_si128(vtw));

			__m128i mweight = _mm_or_si128(_mm256_castsi256_si128(vweights3), _mm256_castsi256_si128(vweights1));

			mx = _mm_shuffle_epi8(mx, _mm256_castsi256_si128(vrot));

			mweight = _mm_shuffle_epi8(mweight, _mm256_castsi256_si128(vrot));
#else
			__m128i mx = _mm_min_epi16(mtx, mtw);

			__m128i mweight = _mm_or_si128(mweights3, mweights1);

			mx = _mm_shuffle_epi8(mx, mrot);

			mweight = _mm_shuffle_epi8(mweight, mrot);
#endif

			mx = _mm_min_epi16(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(1, 0, 3, 2)));

			mweight = _mm_mullo_epi16(mweight, _mm_cvtsi32_si128(static_cast<int>(16 - i)));

			if constexpr (!kDenoise)
			{
				mx = _mm_adds_epu8(mx, mx);
			}

			mx = _mm_srli_epi16(mx, kDenoiseShift);

			mx = _mm_mullo_epi16(mx, mx);

			mx = _mm_madd_epi16(mx, mweight);

			merrorBlock = _mm_add_epi32(merrorBlock, mx);
		}

	done:
		return _mm_cvtsi128_si32(merrorBlock);
	}

	static INLINED BlockError ComputeSubsetTable23(const Area& area, __m128i mc, uint64_t& indices2, uint64_t& indices3, const int rotation) noexcept
	{
		const __m128i mrot = GetRotationShuffleNarrow(rotation);
		const __m128i mhalf = _mm_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);

		const __m128i mmask3 = _mm_shuffle_epi8(_mm_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0), mrot);

		mc = _mm_shuffle_epi8(mc, mrot);

		int errorAlpha = 0;
		int error3;
		{
			__m128i mtx = gTableInterpolate2_U8[0];
			__m128i mty = gTableInterpolate2_U8[1];

			mtx = _mm_maddubs_epi16(mc, mtx);
			mty = _mm_maddubs_epi16(mc, mty);

			mtx = _mm_add_epi16(mtx, mhalf);
			mty = _mm_add_epi16(mty, mhalf);

			mtx = _mm_srli_epi16(mtx, 6);
			mty = _mm_srli_epi16(mty, 6);

			Modulations state3;
			_mm_store_si128((__m128i*)&state3.Values_I16[0], _mm_and_si128(mmask3, mtx));
			_mm_store_si128((__m128i*)&state3.Values_I16[2], _mm_and_si128(mmask3, mty));

			const __m128i mweights3 = _mm_and_si128(mmask3, gWeightsAGRB);

			error3 = ComputeSubsetTable2(area, mweights3, state3);

			for (size_t i = 0, n = area.Count; i < n; i++)
			{
				uint64_t indexColor = static_cast<uint32_t>(state3.Best[i]);
				indices2 |= indexColor << (area.Indices[i] << 1);
			}

			if (rotation != 0)
			{
				for (size_t i = 0, n = area.Count; i < n; i++)
				{
					int da = *(const uint16_t*)&state3.Values_I16[state3.Best[i]] - *(const uint16_t*)&area.DataMask_I16[i];

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
			}
		}

		int error1;
		{
			__m128i mtx = gTableInterpolate3_U8[0];
			__m128i mty = gTableInterpolate3_U8[1];
			__m128i mtz = gTableInterpolate3_U8[2];
			__m128i mtw = gTableInterpolate3_U8[3];

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

			Modulations state1;
			_mm_store_si128((__m128i*)&state1.Values_I16[0], _mm_andnot_si128(mmask3, mtx));
			_mm_store_si128((__m128i*)&state1.Values_I16[2], _mm_andnot_si128(mmask3, mty));
			_mm_store_si128((__m128i*)&state1.Values_I16[4], _mm_andnot_si128(mmask3, mtz));
			_mm_store_si128((__m128i*)&state1.Values_I16[6], _mm_andnot_si128(mmask3, mtw));

			const __m128i mweights1 = _mm_andnot_si128(mmask3, gWeightsAGRB);

			error1 = ComputeSubsetTable3(area, mweights1, state1);

			for (size_t i = 0, n = area.Count; i < n; i++)
			{
				uint64_t indexAlpha = static_cast<uint32_t>(state1.Best[i]);
				indices3 |= indexAlpha << (area.Indices[i] * 3);
			}

			if (rotation == 0)
			{
				for (size_t i = 0, n = area.Count; i < n; i++)
				{
					int da = *(const uint16_t*)&state1.Values_I16[state1.Best[i]] - *(const uint16_t*)&area.DataMask_I16[i];

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
			}
		}

		return BlockError(errorAlpha * kAlpha, error3 + error1);
	}

	static INLINED BlockError ComputeSubsetTable32(const Area& area, __m128i mc, uint64_t& indices2, uint64_t& indices3, const int rotation) noexcept
	{
		const __m128i mrot = GetRotationShuffleNarrow(rotation);
		const __m128i mhalf = _mm_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);

		const __m128i mmask3 = _mm_shuffle_epi8(_mm_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0), mrot);

		mc = _mm_shuffle_epi8(mc, mrot);

		int errorAlpha = 0;
		int error3;
		{
			__m128i mtx = gTableInterpolate3_U8[0];
			__m128i mty = gTableInterpolate3_U8[1];
			__m128i mtz = gTableInterpolate3_U8[2];
			__m128i mtw = gTableInterpolate3_U8[3];

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

			Modulations state3;
			_mm_store_si128((__m128i*)&state3.Values_I16[0], _mm_and_si128(mmask3, mtx));
			_mm_store_si128((__m128i*)&state3.Values_I16[2], _mm_and_si128(mmask3, mty));
			_mm_store_si128((__m128i*)&state3.Values_I16[4], _mm_and_si128(mmask3, mtz));
			_mm_store_si128((__m128i*)&state3.Values_I16[6], _mm_and_si128(mmask3, mtw));

			const __m128i mweights3 = _mm_and_si128(mmask3, gWeightsAGRB);

			error3 = ComputeSubsetTable3(area, mweights3, state3);

			for (size_t i = 0, n = area.Count; i < n; i++)
			{
				uint64_t indexColor = static_cast<uint32_t>(state3.Best[i]);
				indices3 |= indexColor << (area.Indices[i] * 3);
			}

			if (rotation != 0 + 4)
			{
				for (size_t i = 0, n = area.Count; i < n; i++)
				{
					int da = *(const uint16_t*)&state3.Values_I16[state3.Best[i]] - *(const uint16_t*)&area.DataMask_I16[i];

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
			}
		}

		int error1;
		{
			__m128i mtx = gTableInterpolate2_U8[0];
			__m128i mty = gTableInterpolate2_U8[1];

			mtx = _mm_maddubs_epi16(mc, mtx);
			mty = _mm_maddubs_epi16(mc, mty);

			mtx = _mm_add_epi16(mtx, mhalf);
			mty = _mm_add_epi16(mty, mhalf);

			mtx = _mm_srli_epi16(mtx, 6);
			mty = _mm_srli_epi16(mty, 6);

			Modulations state1;
			_mm_store_si128((__m128i*)&state1.Values_I16[0], _mm_andnot_si128(mmask3, mtx));
			_mm_store_si128((__m128i*)&state1.Values_I16[2], _mm_andnot_si128(mmask3, mty));

			const __m128i mweights1 = _mm_andnot_si128(mmask3, gWeightsAGRB);

			error1 = ComputeSubsetTable2(area, mweights1, state1);

			for (size_t i = 0, n = area.Count; i < n; i++)
			{
				uint64_t indexAlpha = static_cast<uint32_t>(state1.Best[i]);
				indices2 |= indexAlpha << (area.Indices[i] << 1);
			}

			if (rotation == 0 + 4)
			{
				for (size_t i = 0, n = area.Count; i < n; i++)
				{
					int da = *(const uint16_t*)&state1.Values_I16[state1.Best[i]] - *(const uint16_t*)&area.DataMask_I16[i];

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
			}
		}

		return BlockError(errorAlpha * kAlpha, error3 + error1);
	}

	void FinalPackBlock(uint8_t output[16], Cell& input) noexcept
	{
		const int rotation = static_cast<int>(input.BestParameter);

		Area& area = input.Area1;

		__m128i mc = input.BestColor0;

		uint64_t indices2 = 0, indices3 = 0;
		if (rotation & 4)
		{
			input.Error = ComputeSubsetTable32(area, mc, indices2, indices3, rotation);

			__m128i malpha = mc;
			AreaReduceTable2(area, malpha, indices2);

			__m128i mcolor = mc;
			AreaReduceTable3(area, mcolor, indices3);

			mc = _mm_blend_epi16(mcolor, malpha, 0x03);
		}
		else
		{
			input.Error = ComputeSubsetTable23(area, mc, indices2, indices3, rotation);

			__m128i mcolor = mc;
			AreaReduceTable2(area, mcolor, indices2);

			__m128i malpha = mc;
			AreaReduceTable3(area, malpha, indices3);

			mc = _mm_blend_epi16(mcolor, malpha, 0x03);
		}

		ComposeBlock(output, mc, indices2, indices3, rotation);
	}

	static INLINED int CompressSubsetFast(const Area& area, __m128i& mc, int water, const int rotation) noexcept
	{
		mc = area.Bounds_U16;

		const __m128i mrot = GetRotationShuffle(rotation);
		mc = _mm_shuffle_epi8(mc, mrot);

		const __m128i mh6 = _mm_set1_epi16(0xFC);
		__m128i ma = _mm_and_si128(mc, mh6);

		const __m128i mh5 = _mm_set1_epi16(0xF8);
		mc = _mm_and_si128(mc, mh5);

		ma = _mm_or_si128(ma, _mm_srli_epi16(ma, 6));
		mc = _mm_or_si128(mc, _mm_srli_epi16(mc, 5));

		mc = _mm_blend_epi16(mc, ma, 3);

		if (rotation & 4)
		{
#if defined(OPTION_COUNTERS)
			gComputeSubsetError32[rotation - 4]++;
#endif
			return ComputeSubsetError32(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water), rotation);
		}
		else
		{
#if defined(OPTION_COUNTERS)
			gComputeSubsetError23[rotation]++;
#endif
			return ComputeSubsetError23(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water), rotation);
		}
	}

	void CompressBlockFast(Cell& input) noexcept
	{
		const int denoiseStep = input.DenoiseStep;

		for (int rotationIndex = 0; rotationIndex < 8; rotationIndex++)
		{
			const int rotation = gRotationsMode4[rotationIndex];

			__m128i mc = _mm_setzero_si128();

			int error = denoiseStep;
			if (error < input.Error.Total)
			{
				Area& area = input.Area1;

				error += CompressSubsetFast(area, mc, input.Error.Total - error, rotation);
			}

			if (input.Error.Total > error)
			{
				input.Error.Total = error - denoiseStep;

				input.BestColor0 = mc;
				input.BestParameter = rotation;
				input.BestMode = 4;

				if (error <= denoiseStep)
					break;
			}
		}
	}

	class Subset23 final
	{
	public:
		LevelsBuffer<LevelsCapacity> chA, chG, chR, chB;

		ALWAYS_INLINED Subset23() noexcept = default;

		INLINED bool InitLevels(const Area& area, const int water, const int rotation) noexcept
		{
			if (area.IsOpaque)
			{
				chA.SetZeroError(0xFFFF);
			}
			else
			{
				if (rotation == 0)
				{
					chA.ComputeChannelLevelsReduced<6, -1, false, gTableDeltas3_Value6, true>(area, 0, kAlpha, water);
				}
				else
				{
					chA.ComputeChannelLevelsReduced<5, -1, false, gTableDeltas2_Value5>(area, 0, kAlpha, water);
				}
			}
			int minA = chA.MinErr;
			if (minA >= water)
				return false;

			if (rotation == 2)
			{
				chG.ComputeChannelLevelsReduced<6, -1, true, gTableDeltas3_Value6, true>(area, 1, kGreen, water - minA);
			}
			else
			{
				chG.ComputeChannelLevelsReduced<5, -1, true, gTableDeltas2_Value5>(area, 1, kGreen, water - minA);
			}
			int minG = chG.MinErr;
			if (minA + minG >= water)
				return false;

			if (rotation == 1)
			{
				chR.ComputeChannelLevelsReduced<6, -1, true, gTableDeltas3_Value6, true>(area, 2, kRed, water - minA - minG);
			}
			else
			{
				chR.ComputeChannelLevelsReduced<5, -1, true, gTableDeltas2_Value5>(area, 2, kRed, water - minA - minG);
			}
			int minR = chR.MinErr;
			if (minA + minG + minR >= water)
				return false;

			if (rotation == 3)
			{
				chB.ComputeChannelLevelsReduced<6, -1, true, gTableDeltas3_Value6, true>(area, 3, kBlue, water - minA - minG - minR);
			}
			else
			{
				chB.ComputeChannelLevelsReduced<5, -1, true, gTableDeltas2_Value5>(area, 3, kBlue, water - minA - minG - minR);
			}
			int minB = chB.MinErr;
			if (minA + minG + minR + minB >= water)
				return false;

			return true;
		}

		template<int rotation>
		INLINED int TryVariants(const Area& area, __m128i& best_color, int water) noexcept
		{
			int minA = chA.MinErr;
			int minG = chG.MinErr;
			int minR = chR.MinErr;
			int minB = chB.MinErr;
			if (minA + minG + minR + minB >= water)
				return water;

			int nA = (rotation == 0) ? 1 : chA.Count;
			int nG = (rotation == 2) ? 1 : chG.Count;
			int nR = (rotation == 1) ? 1 : chR.Count;
			int nB = (rotation == 3) ? 1 : chB.Count;

			const __m128i mrot = GetRotationShuffleNarrow(rotation);

			int memAR[LevelsCapacity];
			int memAGB[LevelsCapacity];

			for (int iA = 0; iA < nA; iA++)
			{
				int eA = chA.Err[iA].Error;
				if (eA + minG + minR + minB >= water)
					break;

				int cA = chA.Err[iA].Color;

				for (int i = 0; i < nR; i++)
				{
					memAR[i] = -1;
				}

				for (int iG = 0; iG < nG; iG++)
				{
					int eG = chG.Err[iG].Error + eA;
					if (eG + minR + minB >= water)
						break;

					int cG = chG.Err[iG].Color;

					if constexpr ((rotation == 1) || (rotation == 3))
					{
						__m128i mc = _mm_setzero_si128();
						mc = _mm_insert_epi16(mc, cA, 0);
						mc = _mm_insert_epi16(mc, cG, 1);
						mc = _mm_shuffle_epi8(mc, mrot);
						mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
						gComputeSubsetError23AG[rotation]++;
#endif
						eG = ComputeSubsetError23(area, mc, gWeightsAG, _mm_cvtsi32_si128(water - minR - minB), rotation);
						if (eG + minR + minB >= water)
							continue;
					}

					for (int i = 0; i < nB; i++)
					{
						memAGB[i] = -1;
					}

					for (int iR = 0; iR < nR; iR++)
					{
						int eR = chR.Err[iR].Error + eG;
						if (eR + minB >= water)
							break;

						int cR = chR.Err[iR].Color;

						if constexpr ((rotation == 2) || (rotation == 3))
						{
							int ear = memAR[iR];
							if (ear < 0)
							{
								__m128i mc = _mm_setzero_si128();
								mc = _mm_insert_epi16(mc, cA, 0);
								mc = _mm_insert_epi16(mc, cR, 2);
								mc = _mm_shuffle_epi8(mc, mrot);
								mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
								gComputeSubsetError23AR[rotation]++;
#endif
								ear = ComputeSubsetError23(area, mc, gWeightsAR, _mm_cvtsi32_si128(water - minG - minB), rotation);
								memAR[iR] = ear;
							}
							if (ear + minG + minB >= water)
								continue;

							if constexpr (rotation == 2)
							{
								eR = ear + chG.Err[iG].Error;
							}
						}

						if constexpr ((rotation == 0) || (rotation == 3))
						{
							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, cA, 0);
							mc = _mm_insert_epi16(mc, cG, 1);
							mc = _mm_insert_epi16(mc, cR, 2);
							mc = _mm_shuffle_epi8(mc, mrot);
							mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
							gComputeSubsetError23AGR[rotation]++;
#endif
							eR = ComputeSubsetError23(area, mc, gWeightsAGR, _mm_cvtsi32_si128(water - minB), rotation);
							if (eR + minB >= water)
								continue;
						}

						for (int iB = 0; iB < nB; iB++)
						{
							int eB = chB.Err[iB].Error + eR;
							if (eB >= water)
								break;

							int cB = chB.Err[iB].Color;

							if constexpr (rotation != 3)
							{
								int eagb = memAGB[iB];
								if (eagb < 0)
								{
									__m128i mc = _mm_setzero_si128();
									mc = _mm_insert_epi16(mc, cA, 0);
									mc = _mm_insert_epi16(mc, cG, 1);
									mc = _mm_insert_epi16(mc, cB, 3);
									mc = _mm_shuffle_epi8(mc, mrot);
									mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
									gComputeSubsetError23AGB[rotation]++;
#endif
									eagb = ComputeSubsetError23(area, mc, gWeightsAGB, _mm_cvtsi32_si128(water - minR), rotation);
									memAGB[iB] = eagb;
								}
								if (eagb + minR >= water)
									continue;

								if constexpr (rotation == 1)
								{
									eB = eagb + chR.Err[iR].Error;
								}
							}

							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, cA, 0);
							mc = _mm_insert_epi16(mc, cG, 1);
							mc = _mm_insert_epi16(mc, cR, 2);
							mc = _mm_insert_epi16(mc, cB, 3);
							mc = _mm_shuffle_epi8(mc, mrot);
							mc = _mm_cvtepu8_epi16(mc);

							if constexpr ((rotation == 0) || (rotation == 2))
							{
#if defined(OPTION_COUNTERS)
								gComputeSubsetError23[rotation]++;
#endif
								eB = ComputeSubsetError23(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water), rotation);
							}

							if (water > eB)
							{
								water = eB;

								best_color = mc;
							}
						}
					}
				}
			}

			return water;
		}
	};

	class Subset32 final
	{
	public:
		LevelsBuffer<LevelsCapacity> chA, chG, chR, chB;

		ALWAYS_INLINED Subset32() noexcept = default;

		INLINED bool InitLevels(const Area& area, const int water, const int rotation) noexcept
		{
			if (area.IsOpaque)
			{
				chA.SetZeroError(0xFFFF);
			}
			else
			{
				if (rotation == 0 + 4)
				{
					chA.ComputeChannelLevelsReduced<6, -1, false, gTableDeltas2_Value6, true>(area, 0, kAlpha, water);
				}
				else
				{
					chA.ComputeChannelLevelsReduced<5, -1, false, gTableDeltas3_Value5>(area, 0, kAlpha, water);
				}
			}
			int minA = chA.MinErr;
			if (minA >= water)
				return false;

			if (rotation == 2 + 4)
			{
				chG.ComputeChannelLevelsReduced<6, -1, true, gTableDeltas2_Value6, true>(area, 1, kGreen, water - minA);
			}
			else
			{
				chG.ComputeChannelLevelsReduced<5, -1, true, gTableDeltas3_Value5>(area, 1, kGreen, water - minA);
			}
			int minG = chG.MinErr;
			if (minA + minG >= water)
				return false;

			if (rotation == 1 + 4)
			{
				chR.ComputeChannelLevelsReduced<6, -1, true, gTableDeltas2_Value6, true>(area, 2, kRed, water - minA - minG);
			}
			else
			{
				chR.ComputeChannelLevelsReduced<5, -1, true, gTableDeltas3_Value5>(area, 2, kRed, water - minA - minG);
			}
			int minR = chR.MinErr;
			if (minA + minG + minR >= water)
				return false;

			if (rotation == 3 + 4)
			{
				chB.ComputeChannelLevelsReduced<6, -1, true, gTableDeltas2_Value6, true>(area, 3, kBlue, water - minA - minG - minR);
			}
			else
			{
				chB.ComputeChannelLevelsReduced<5, -1, true, gTableDeltas3_Value5>(area, 3, kBlue, water - minA - minG - minR);
			}
			int minB = chB.MinErr;
			if (minA + minG + minR + minB >= water)
				return false;

			return true;
		}

		template<int rotation>
		INLINED int TryVariants(const Area& area, __m128i& best_color, int water) noexcept
		{
			int minA = chA.MinErr;
			int minG = chG.MinErr;
			int minR = chR.MinErr;
			int minB = chB.MinErr;
			if (minA + minG + minR + minB >= water)
				return water;

			int nA = (rotation == 0) ? 1 : chA.Count;
			int nG = (rotation == 2) ? 1 : chG.Count;
			int nR = (rotation == 1) ? 1 : chR.Count;
			int nB = (rotation == 3) ? 1 : chB.Count;

			const __m128i mrot = GetRotationShuffleNarrow(rotation);

			int memAR[LevelsCapacity];
			int memAGB[LevelsCapacity];

			for (int iA = 0; iA < nA; iA++)
			{
				int eA = chA.Err[iA].Error;
				if (eA + minG + minR + minB >= water)
					break;

				int cA = chA.Err[iA].Color;

				for (int i = 0; i < nR; i++)
				{
					memAR[i] = -1;
				}

				for (int iG = 0; iG < nG; iG++)
				{
					int eG = chG.Err[iG].Error + eA;
					if (eG + minR + minB >= water)
						break;

					int cG = chG.Err[iG].Color;

					if constexpr ((rotation == 1) || (rotation == 3))
					{
						__m128i mc = _mm_setzero_si128();
						mc = _mm_insert_epi16(mc, cA, 0);
						mc = _mm_insert_epi16(mc, cG, 1);
						mc = _mm_shuffle_epi8(mc, mrot);
						mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
						gComputeSubsetError32AG[rotation]++;
#endif
						eG = ComputeSubsetError32(area, mc, gWeightsAG, _mm_cvtsi32_si128(water - minR - minB), rotation);
						if (eG + minR + minB >= water)
							continue;
					}

					for (int i = 0; i < nB; i++)
					{
						memAGB[i] = -1;
					}

					for (int iR = 0; iR < nR; iR++)
					{
						int eR = chR.Err[iR].Error + eG;
						if (eR + minB >= water)
							break;

						int cR = chR.Err[iR].Color;

						if constexpr ((rotation == 2) || (rotation == 3))
						{
							int ear = memAR[iR];
							if (ear < 0)
							{
								__m128i mc = _mm_setzero_si128();
								mc = _mm_insert_epi16(mc, cA, 0);
								mc = _mm_insert_epi16(mc, cR, 2);
								mc = _mm_shuffle_epi8(mc, mrot);
								mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
								gComputeSubsetError32AR[rotation]++;
#endif
								ear = ComputeSubsetError32(area, mc, gWeightsAR, _mm_cvtsi32_si128(water - minG - minB), rotation);
								memAR[iR] = ear;
							}
							if (ear + minG + minB >= water)
								continue;

							if constexpr (rotation == 2)
							{
								eR = ear + chG.Err[iG].Error;
							}
						}

						if constexpr ((rotation == 0) || (rotation == 3))
						{
							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, cA, 0);
							mc = _mm_insert_epi16(mc, cG, 1);
							mc = _mm_insert_epi16(mc, cR, 2);
							mc = _mm_shuffle_epi8(mc, mrot);
							mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
							gComputeSubsetError32AGR[rotation]++;
#endif
							eR = ComputeSubsetError32(area, mc, gWeightsAGR, _mm_cvtsi32_si128(water - minB), rotation);
							if (eR + minB >= water)
								continue;
						}

						for (int iB = 0; iB < nB; iB++)
						{
							int eB = chB.Err[iB].Error + eR;
							if (eB >= water)
								break;

							int cB = chB.Err[iB].Color;

							if constexpr (rotation != 3)
							{
								int eagb = memAGB[iB];
								if (eagb < 0)
								{
									__m128i mc = _mm_setzero_si128();
									mc = _mm_insert_epi16(mc, cA, 0);
									mc = _mm_insert_epi16(mc, cG, 1);
									mc = _mm_insert_epi16(mc, cB, 3);
									mc = _mm_shuffle_epi8(mc, mrot);
									mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
									gComputeSubsetError32AGB[rotation]++;
#endif
									eagb = ComputeSubsetError32(area, mc, gWeightsAGB, _mm_cvtsi32_si128(water - minR), rotation);
									memAGB[iB] = eagb;
								}
								if (eagb + minR >= water)
									continue;

								if constexpr (rotation == 1)
								{
									eB = eagb + chR.Err[iR].Error;
								}
							}

							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, cA, 0);
							mc = _mm_insert_epi16(mc, cG, 1);
							mc = _mm_insert_epi16(mc, cR, 2);
							mc = _mm_insert_epi16(mc, cB, 3);
							mc = _mm_shuffle_epi8(mc, mrot);
							mc = _mm_cvtepu8_epi16(mc);

							if constexpr ((rotation == 0) || (rotation == 2))
							{
#if defined(OPTION_COUNTERS)
								gComputeSubsetError32[rotation]++;
#endif
								eB = ComputeSubsetError32(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water), rotation);
							}

							if (water > eB)
							{
								water = eB;

								best_color = mc;
							}
						}
					}
				}
			}

			return water;
		}
	};

	static INLINED int CompressSubset(const Area& area, __m128i& mc, int water, const int rotation)
	{
		if (rotation & 4)
		{
			Subset32 subset32;
			if (subset32.InitLevels(area, water, rotation))
			{
				switch (rotation)
				{
				case 4:
					water = subset32.TryVariants<0>(area, mc, water);
					break;

				case 5:
					water = subset32.TryVariants<1>(area, mc, water);
					break;

				case 6:
					water = subset32.TryVariants<2>(area, mc, water);
					break;

				case 7:
					water = subset32.TryVariants<3>(area, mc, water);
					break;
				}
			}
		}
		else
		{
			Subset23 subset23;
			if (subset23.InitLevels(area, water, rotation))
			{
				switch (rotation)
				{
				case 0:
					water = subset23.TryVariants<0>(area, mc, water);
					break;

				case 1:
					water = subset23.TryVariants<1>(area, mc, water);
					break;

				case 2:
					water = subset23.TryVariants<2>(area, mc, water);
					break;

				case 3:
					water = subset23.TryVariants<3>(area, mc, water);
					break;
				}
			}
		}

		return water;
	}

	void CompressBlock(Cell& input) noexcept
	{
		int rotation = static_cast<int>(input.BestParameter);

		for (int i = 0; i < 2; i++, rotation ^= 4)
		{
			__m128i mc = input.BestColor0;

			int error = 0;
			{
				Area& area = input.Area1;

				error += CompressSubset(area, mc, input.Error.Total - error, rotation);
			}

			if (input.Error.Total > error)
			{
				input.Error.Total = error;

				input.BestColor0 = mc;
				input.BestParameter = rotation;
				//input.BestMode = 4;

				if (error <= 0)
					break;
			}
		}
	}

	void CompressBlockFull(Cell& input) noexcept
	{
		const int denoiseStep = input.DenoiseStep;

		for (int rotationIndex = 0; rotationIndex < 8; rotationIndex++)
		{
			const int rotation = gRotationsMode4[rotationIndex];

			if ((input.PersonalMode == 4) && !((input.PersonalParameter ^ rotation) & 3))
				continue;

			__m128i mc = _mm_setzero_si128();

			int error = denoiseStep;
			if (error < input.Error.Total)
			{
				Area& area = input.Area1;

				error += CompressSubset(area, mc, input.Error.Total - error, rotation);
			}

			if (input.Error.Total > error)
			{
				input.Error.Total = error - denoiseStep;

				input.BestColor0 = mc;
				input.BestParameter = rotation;
				input.BestMode = 4;

				if (error <= denoiseStep)
					break;
			}
		}
	}

	void PrintCounters() noexcept
	{
#if defined(OPTION_COUNTERS)
		for (int rotationIndex = 0; rotationIndex < 8; rotationIndex++)
		{
			const int rotation = gRotationsMode4[rotationIndex];

			if (rotation & 4)
			{
				PRINTF("[Mode 4.%i]\tAG32 = %i, AR32 = %i, AGR32 = %i, AGB32 = %i, AGRB32 = %i", rotation,
					gComputeSubsetError32AG[rotation - 4].load(), gComputeSubsetError32AR[rotation - 4].load(),
					gComputeSubsetError32AGR[rotation - 4].load(), gComputeSubsetError32AGB[rotation - 4].load(),
					gComputeSubsetError32[rotation - 4].load());
			}
			else
			{
				PRINTF("[Mode 4.%i]\tAG23 = %i, AR23 = %i, AGR23 = %i, AGB23 = %i, AGRB23 = %i", rotation,
					gComputeSubsetError23AG[rotation].load(), gComputeSubsetError23AR[rotation].load(),
					gComputeSubsetError23AGR[rotation].load(), gComputeSubsetError23AGB[rotation].load(),
					gComputeSubsetError23[rotation].load());
			}
		}
#endif
	}

} // namespace Mode4
