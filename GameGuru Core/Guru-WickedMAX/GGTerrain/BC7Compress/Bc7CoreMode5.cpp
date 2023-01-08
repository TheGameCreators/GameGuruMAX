
#include "pch.h"
#include "Bc7Core.h"
#include "Bc7Tables.h"

#include "SnippetInsertRemoveZeroBit.h"
#include "SnippetHorizontalSum4.h"
#include "SnippetLevelsBuffer.h"

// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/bc7-format-mode-reference#mode-5

namespace Mode5 {

	constexpr int LevelsCapacity = 32;

#if defined(OPTION_COUNTERS)
	static std::atomic_int gComputeSubsetError2[4], gComputeSubsetError2AG[4], gComputeSubsetError2AR[4], gComputeSubsetError2AGR[4], gComputeSubsetError2AGB[4];
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

	static INLINED void DecompressSubset(__m128i mc, int* output, uint64_t dataColor, uint64_t dataAlpha, const int rotation) noexcept
	{
		const __m128i mrot = GetRotationShuffleNarrow(rotation);
		const __m128i mhalf = _mm_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);

		for (size_t i = 0; i < 16; i++)
		{
			__m128i mratio = _mm_loadl_epi64((const __m128i*)&((const uint64_t*)gTableInterpolate2_U8)[dataColor & 3]); dataColor >>= 2;
			mratio = _mm_insert_epi16(mratio, *(const uint16_t*)&((const uint64_t*)gTableInterpolate2_U8)[dataAlpha & 3], 0); dataAlpha >>= 2;

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

		data0 >>= 6;

		int rotation = static_cast<int>(data0 & 3); data0 >>= 2;

		__m128i mc0 = _mm_setzero_si128();

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 4); data0 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 5); data0 >>= 7;

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 2); data0 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 3); data0 >>= 7;

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 6); data0 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 7); data0 >>= 7;

		mc0 = _mm_add_epi16(mc0, mc0);

		const __m128i m8 = _mm_set1_epi16(0xFF);
		mc0 = _mm_and_si128(mc0, m8);

		mc0 = _mm_or_si128(mc0, _mm_srli_epi16(mc0, 7));

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 0); data0 >>= 8;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0 + (data1 << 6)), 1); data1 >>= 2;

		mc0 = _mm_and_si128(mc0, m8);

		data1 = InsertZeroBit(data1, 1 + 31);
		data1 = InsertZeroBit(data1, 1);

		DecompressSubset(mc0, (int*)output.ImageRows_U8, data1, data1 >> 32, rotation);

		output.BestColor0 = mc0;
		output.BestColor1 = _mm_setzero_si128();
		output.BestColor2 = _mm_setzero_si128();
		output.BestParameter = rotation;
		output.BestMode = 5;
	}

	static INLINED void ComposeBlock(uint8_t output[16], __m128i mc0, uint64_t dataColor, uint64_t dataAlpha, const int rotation) noexcept
	{
		uint64_t data1 = RemoveZeroBit(dataAlpha, 1);
		data1 <<= 31; data1 |= RemoveZeroBit(dataColor, 1);

		data1 <<= 2; data1 |= _mm_extract_epi16(mc0, 1) >> 6;
		uint64_t data0 = _mm_extract_epi16(mc0, 1) & 0x3F;
		data0 <<= 8; data0 |= _mm_extract_epi16(mc0, 0);

		mc0 = _mm_srli_epi16(mc0, 1);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 7);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 6);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 3);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 2);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 5);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 4);

		data0 <<= 2; data0 |= rotation;

		data0 <<= 6; data0 |= 1 << 5;

		*(uint64_t*)&output[0] = data0;
		*(uint64_t*)&output[8] = data1;
	}

	static INLINED int ComputeSubsetError2(const Area& area, __m128i mc, const __m128i mweights, const __m128i mwater, const int rotation) noexcept
	{
		__m128i merrorBlock = _mm_setzero_si128();

#if defined(OPTION_AVX512)
		const __m512i wrot = _mm512_broadcast_i32x4(GetRotationShuffleNarrow(rotation));
		const __m256i vhalf = _mm256_set1_epi16(32);
		const __m512i wweights = _mm512_broadcastq_epi64(mweights);

		mc = _mm_packus_epi16(mc, mc);
		__m256i vc = _mm256_broadcastq_epi64(mc);

		const __m512i wmask3 = _mm512_shuffle_epi8(_mm512_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0), wrot);
		const __m512i wweights3 = _mm512_and_epi32(wweights, wmask3);
		const __m512i wweights1 = _mm512_shuffle_epi8(_mm512_andnot_epi32(wmask3, wweights), wrot);

		vc = _mm256_shuffle_epi8(vc, _mm512_castsi512_si256(wrot));

		__m256i vt = *(const __m256i*)gTableInterpolate2_U8;

		vt = _mm256_maddubs_epi16(vc, vt);

		vt = _mm256_add_epi16(vt, vhalf);

		vt = _mm256_srli_epi16(vt, 6);

		__m512i wtx = _mm512_broadcast_i32x4(_mm256_castsi256_si128(vt));
		__m512i wty = _mm512_broadcast_i32x4(_mm256_extracti128_si256(vt, 1));

		int k = 16;
		const __m512i* p = (const __m512i*)area.DataMask_I16;
		do
		{
			__m512i wpacked = _mm512_load_epi64(p);
			__m512i wpixel = _mm512_unpacklo_epi64(wpacked, wpacked);
			__m512i wmask = _mm512_unpackhi_epi64(wpacked, wpacked);

			__m512i wx = _mm512_sub_epi16(wpixel, wtx);
			__m512i wy = _mm512_sub_epi16(wpixel, wty);

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

			wx = _mm512_and_epi32(wx, wmask);
			wy = _mm512_and_epi32(wy, wmask);

			__m512i wa = _mm512_shuffle_epi8(_mm512_min_epi16(wx, wy), wrot);

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
				break;

		} while ((k -= 4) > 0);
#elif defined(OPTION_AVX2)
		const __m256i vrot = _mm256_broadcastsi128_si256(GetRotationShuffleNarrow(rotation));
		const __m256i vhalf = _mm256_set1_epi16(32);
		const __m256i vweights = _mm256_broadcastq_epi64(mweights);

		mc = _mm_packus_epi16(mc, mc);
		__m256i vc = _mm256_broadcastq_epi64(mc);

		const __m256i vmask3 = _mm256_shuffle_epi8(_mm256_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0), vrot);
		const __m256i vweights3 = _mm256_and_si256(vweights, vmask3);
		const __m256i vweights1 = _mm256_shuffle_epi8(_mm256_andnot_si256(vmask3, vweights), vrot);

		vc = _mm256_shuffle_epi8(vc, vrot);

		__m256i vt = *(const __m256i*)gTableInterpolate2_U8;

		vt = _mm256_maddubs_epi16(vc, vt);

		vt = _mm256_add_epi16(vt, vhalf);

		vt = _mm256_srli_epi16(vt, 6);

		__m256i vtx = _mm256_permute4x64_epi64(vt, 0x44);
		__m256i vty = _mm256_permute4x64_epi64(vt, 0xEE);

		int k = 16;
		const __m256i* p = (const __m256i*)area.DataMask_I16;
		do
		{
			__m256i vpacked0 = _mm256_load_si256(&p[0]);
			__m256i vpacked1 = _mm256_load_si256(&p[1]);
			__m256i vpixel0 = _mm256_unpacklo_epi64(vpacked0, vpacked0);
			__m256i vpixel1 = _mm256_unpacklo_epi64(vpacked1, vpacked1);
			__m256i vmask0 = _mm256_unpackhi_epi64(vpacked0, vpacked0);
			__m256i vmask1 = _mm256_unpackhi_epi64(vpacked1, vpacked1);

			__m256i vx = _mm256_sub_epi16(vpixel0, vtx);
			__m256i vy = _mm256_sub_epi16(vpixel0, vty);
			__m256i vz = _mm256_sub_epi16(vpixel1, vtx);
			__m256i vw = _mm256_sub_epi16(vpixel1, vty);

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

			vx = _mm256_and_si256(vx, vmask0);
			vy = _mm256_and_si256(vy, vmask0);
			vz = _mm256_and_si256(vz, vmask1);
			vw = _mm256_and_si256(vw, vmask1);

			__m256i va0 = _mm256_shuffle_epi8(_mm256_min_epi16(vx, vy), vrot);
			__m256i va1 = _mm256_shuffle_epi8(_mm256_min_epi16(vz, vw), vrot);

			vx = _mm256_madd_epi16(vx, vweights3);
			vy = _mm256_madd_epi16(vy, vweights3);
			vz = _mm256_madd_epi16(vz, vweights3);
			vw = _mm256_madd_epi16(vw, vweights3);
			va0 = _mm256_madd_epi16(va0, vweights1);
			va1 = _mm256_madd_epi16(va1, vweights1);

			vx = _mm256_add_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
			vy = _mm256_add_epi32(vy, _mm256_shuffle_epi32(vy, _MM_SHUFFLE(2, 3, 0, 1)));
			vz = _mm256_add_epi32(vz, _mm256_shuffle_epi32(vz, _MM_SHUFFLE(2, 3, 0, 1)));
			vw = _mm256_add_epi32(vw, _mm256_shuffle_epi32(vw, _MM_SHUFFLE(2, 3, 0, 1)));

			vx = _mm256_min_epi32(vx, vy);
			vz = _mm256_min_epi32(vz, vw);
			va0 = _mm256_min_epi32(va0, _mm256_shuffle_epi32(va0, _MM_SHUFFLE(1, 0, 3, 2)));
			va1 = _mm256_min_epi32(va1, _mm256_shuffle_epi32(va1, _MM_SHUFFLE(1, 0, 3, 2)));
			vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));
			vz = _mm256_min_epi32(vz, _mm256_shuffle_epi32(vz, _MM_SHUFFLE(1, 0, 3, 2)));

			vx = _mm256_add_epi32(vx, va0);
			vz = _mm256_add_epi32(vz, va1);

			vx = _mm256_add_epi32(vx, vz);

			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
			merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

			p += 2;

			if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
				break;

		} while ((k -= 4) > 0);
#else
		const __m128i mrot = GetRotationShuffleNarrow(rotation);
		const __m128i mhalf = _mm_set1_epi16(32);

		mc = _mm_packus_epi16(mc, mc);

		const __m128i mmask3 = _mm_shuffle_epi8(_mm_set_epi16(-1, -1, -1, 0, -1, -1, -1, 0), mrot);
		const __m128i mweights3 = _mm_and_si128(mweights, mmask3);
		const __m128i mweights1 = _mm_shuffle_epi8(_mm_andnot_si128(mmask3, mweights), mrot);

		mc = _mm_shuffle_epi8(mc, mrot);

		__m128i mtx = gTableInterpolate2_U8[0];
		__m128i mty = gTableInterpolate2_U8[1];

		mtx = _mm_maddubs_epi16(mc, mtx);
		mty = _mm_maddubs_epi16(mc, mty);

		mtx = _mm_add_epi16(mtx, mhalf);
		mty = _mm_add_epi16(mty, mhalf);

		mtx = _mm_srli_epi16(mtx, 6);
		mty = _mm_srli_epi16(mty, 6);

		for (size_t i = 0; i < 16; i++)
		{
			__m128i mpacked = _mm_load_si128(&area.DataMask_I16[i]);
			__m128i mpixel = _mm_unpacklo_epi64(mpacked, mpacked);
			__m128i mmask = _mm_unpackhi_epi64(mpacked, mpacked);

			__m128i mx = _mm_sub_epi16(mpixel, mtx);
			__m128i my = _mm_sub_epi16(mpixel, mty);

			mx = _mm_abs_epi16(mx);
			my = _mm_abs_epi16(my);

			if constexpr (!kDenoise)
			{
				mx = _mm_adds_epu8(mx, mx);
				my = _mm_adds_epu8(my, my);
			}

			mx = _mm_srli_epi16(mx, kDenoiseShift);
			my = _mm_srli_epi16(my, kDenoiseShift);

			mx = _mm_mullo_epi16(mx, mx);
			my = _mm_mullo_epi16(my, my);

			mx = _mm_and_si128(mx, mmask);
			my = _mm_and_si128(my, mmask);

			__m128i ma = _mm_shuffle_epi8(_mm_min_epi16(mx, my), mrot);

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
				break;
		}
#endif

		return _mm_cvtsi128_si32(merrorBlock);
	}

	static INLINED BlockError ComputeSubsetTable2(const Area& area, __m128i mc, uint64_t& indicesColor, uint64_t& indicesAlpha, const int rotation) noexcept
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
				indicesColor |= indexColor << (area.Indices[i] << 1);
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
				indicesAlpha |= indexAlpha << (area.Indices[i] << 1);
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

	void FinalPackBlock(uint8_t output[16], Cell& input) noexcept
	{
		const int rotation = static_cast<int>(input.BestParameter);

		Area& area = input.Area1;

		__m128i mc = input.BestColor0;

		uint64_t indicesColor = 0, indicesAlpha = 0;
		input.Error = ComputeSubsetTable2(area, mc, indicesColor, indicesAlpha, rotation);

		__m128i mcolor = mc;
		AreaReduceTable2(area, mcolor, indicesColor);

		__m128i malpha = mc;
		AreaReduceTable2(area, malpha, indicesAlpha);

		mc = _mm_blend_epi16(mcolor, malpha, 0x03);

		ComposeBlock(output, mc, indicesColor, indicesAlpha, rotation);
	}

	static INLINED int CompressSubsetFast(const Area& area, __m128i& mc, int water, const int rotation) noexcept
	{
		mc = area.Bounds_U16;

		const __m128i mrot = GetRotationShuffle(rotation);
		mc = _mm_shuffle_epi8(mc, mrot);

		__m128i ma = mc;

		const __m128i mh7 = _mm_set1_epi16(0xFE);
		mc = _mm_and_si128(mc, mh7);

		mc = _mm_or_si128(mc, _mm_srli_epi16(mc, 7));

		mc = _mm_blend_epi16(mc, ma, 3);

#if defined(OPTION_COUNTERS)
		gComputeSubsetError2[rotation]++;
#endif
		return ComputeSubsetError2(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water), rotation);
	}

	void CompressBlockFast(Cell& input) noexcept
	{
		const int denoiseStep = input.DenoiseStep;

		for (int rotationIndex = 0; rotationIndex < 4; rotationIndex++)
		{
			const int rotation = gRotationsMode5[rotationIndex];

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
				input.BestMode = 5;

				if (error <= denoiseStep)
					break;
			}
		}
	}

	class Subset final
	{
	public:
		LevelsBuffer<LevelsCapacity> chA, chG, chR, chB;

		ALWAYS_INLINED Subset() noexcept = default;

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
					chA.ComputeChannelLevelsReduced<8, -1, false, gTableDeltas2_Value8, true>(area, 0, kAlpha, water);
				}
				else
				{
					chA.ComputeChannelLevelsReduced<7, -1, false, gTableDeltas2_Value7>(area, 0, kAlpha, water);
				}
			}
			int minA = chA.MinErr;
			if (minA >= water)
				return false;

			if (rotation == 2)
			{
				chG.ComputeChannelLevelsReduced<8, -1, true, gTableDeltas2_Value8, true>(area, 1, kGreen, water - minA);
			}
			else
			{
				chG.ComputeChannelLevelsReduced<7, -1, true, gTableDeltas2_Value7>(area, 1, kGreen, water - minA);
			}
			int minG = chG.MinErr;
			if (minA + minG >= water)
				return false;

			if (rotation == 1)
			{
				chR.ComputeChannelLevelsReduced<8, -1, true, gTableDeltas2_Value8, true>(area, 2, kRed, water - minA - minG);
			}
			else
			{
				chR.ComputeChannelLevelsReduced<7, -1, true, gTableDeltas2_Value7>(area, 2, kRed, water - minA - minG);
			}
			int minR = chR.MinErr;
			if (minA + minG + minR >= water)
				return false;

			if (rotation == 3)
			{
				chB.ComputeChannelLevelsReduced<8, -1, true, gTableDeltas2_Value8, true>(area, 3, kBlue, water - minA - minG - minR);
			}
			else
			{
				chB.ComputeChannelLevelsReduced<7, -1, true, gTableDeltas2_Value7>(area, 3, kBlue, water - minA - minG - minR);
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

					if constexpr ((rotation != 0) && (rotation != 2))
					{
						__m128i mc = _mm_setzero_si128();
						mc = _mm_insert_epi16(mc, cA, 0);
						mc = _mm_insert_epi16(mc, cG, 1);
						mc = _mm_shuffle_epi8(mc, mrot);
						mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
						gComputeSubsetError2AG[rotation]++;
#endif
						eG = ComputeSubsetError2(area, mc, gWeightsAG, _mm_cvtsi32_si128(water - minR - minB), rotation);
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

						if constexpr ((rotation != 0) && (rotation != 1))
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
								gComputeSubsetError2AR[rotation]++;
#endif
								ear = ComputeSubsetError2(area, mc, gWeightsAR, _mm_cvtsi32_si128(water - minG - minB), rotation);
								memAR[iR] = ear;
							}
							if (ear + minG + minB >= water)
								continue;

							if constexpr (rotation == 2)
							{
								eR = ear + chG.Err[iG].Error;
							}
						}

						if constexpr ((rotation != 2) && (rotation != 1))
						{
							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, cA, 0);
							mc = _mm_insert_epi16(mc, cG, 1);
							mc = _mm_insert_epi16(mc, cR, 2);
							mc = _mm_shuffle_epi8(mc, mrot);
							mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
							gComputeSubsetError2AGR[rotation]++;
#endif
							eR = ComputeSubsetError2(area, mc, gWeightsAGR, _mm_cvtsi32_si128(water - minB), rotation);
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
									gComputeSubsetError2AGB[rotation]++;
#endif
									eagb = ComputeSubsetError2(area, mc, gWeightsAGB, _mm_cvtsi32_si128(water - minR), rotation);
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

							if constexpr ((rotation != 1) && (rotation != 3))
							{
#if defined(OPTION_COUNTERS)
								gComputeSubsetError2[rotation]++;
#endif
								eB = ComputeSubsetError2(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water), rotation);
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
		Subset subset;
		if (subset.InitLevels(area, water, rotation))
		{
			switch (rotation)
			{
			case 0:
				water = subset.TryVariants<0>(area, mc, water);
				break;

			case 1:
				water = subset.TryVariants<1>(area, mc, water);
				break;

			case 2:
				water = subset.TryVariants<2>(area, mc, water);
				break;

			case 3:
				water = subset.TryVariants<3>(area, mc, water);
				break;
			}
		}

		return water;
	}

	void CompressBlock(Cell& input) noexcept
	{
		const int rotation = static_cast<int>(input.BestParameter);

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
			//input.BestParameter = rotation;
			//input.BestMode = 5;
		}
	}

	void CompressBlockFull(Cell& input) noexcept
	{
		const int denoiseStep = input.DenoiseStep;

		for (int rotationIndex = 0; rotationIndex < 4; rotationIndex++)
		{
			const int rotation = gRotationsMode5[rotationIndex];

			if ((input.PersonalMode == 5) && (input.PersonalParameter == rotation))
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
				input.BestMode = 5;

				if (error <= denoiseStep)
					break;
			}
		}
	}

	void PrintCounters() noexcept
	{
#if defined(OPTION_COUNTERS)
		for (int rotationIndex = 0; rotationIndex < 4; rotationIndex++)
		{
			const int rotation = gRotationsMode5[rotationIndex];

			PRINTF("[Mode 5.%i]\tAG2 = %i, AR2 = %i, AGR2 = %i, AGB2 = %i, AGRB2 = %i", rotation,
				gComputeSubsetError2AG[rotation].load(), gComputeSubsetError2AR[rotation].load(),
				gComputeSubsetError2AGR[rotation].load(), gComputeSubsetError2AGB[rotation].load(),
				gComputeSubsetError2[rotation].load());
		}
#endif
	}

} // namespace Mode5
