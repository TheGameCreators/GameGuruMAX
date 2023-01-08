#pragma once

#include "pch.h"
#include "Bc7Core.h"
#include "Bc7Tables.h"

static INLINED int ComputeSubsetError3(const Area& area, __m128i mc, const __m128i mweights, const __m128i mwater) noexcept
{
	__m128i merrorBlock = _mm_setzero_si128();

#if defined(OPTION_AVX512)
	const __m512i wweights = _mm512_broadcastq_epi64(mweights);

	const __m512i whalf = _mm512_set1_epi16(32);

	mc = _mm_packus_epi16(mc, mc);
	__m512i wc = _mm512_broadcastq_epi64(mc);

	__m512i wt = *(const __m512i*)gTableInterpolate3_U8;

	wt = _mm512_maddubs_epi16(wc, wt);

	wt = _mm512_add_epi16(wt, whalf);

	wt = _mm512_srli_epi16(wt, 6);

	__m512i wtx = _mm512_permutex_epi64(wt, 0x44);
	__m512i wty = _mm512_permutex_epi64(wt, 0xEE);

	int k = static_cast<int>(area.Active);
	const __m256i* p = (const __m256i*)area.DataMask_I16;

	while ((k -= 2) >= 0)
	{
		__m256i vpacked = _mm256_load_si256(p);
		__m256i vpixel = _mm256_unpacklo_epi64(vpacked, vpacked);
		__m512i wpixel = _mm512_broadcast_i64x4(vpixel);

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

		wx = _mm512_madd_epi16(wx, wweights);
		wy = _mm512_madd_epi16(wy, wweights);

		wx = _mm512_add_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(2, 3, 0, 1)));
		wy = _mm512_add_epi32(wy, _mm512_shuffle_epi32(wy, _MM_SHUFFLE(2, 3, 0, 1)));

		wx = _mm512_min_epi32(wx, wy);
		__m256i vx = _mm256_min_epi32(_mm512_extracti64x4_epi64(wx, 1), _mm512_castsi512_si256(wx));
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
		__m512i wpixel = _mm512_broadcastq_epi64(mpacked);

		__m512i wx = _mm512_sub_epi16(wpixel, wt);

		wx = _mm512_abs_epi16(wx);

		if constexpr (!kDenoise)
		{
			wx = _mm512_adds_epu8(wx, wx);
		}

		wx = _mm512_srli_epi16(wx, kDenoiseShift);

		wx = _mm512_mullo_epi16(wx, wx);

		wx = _mm512_madd_epi16(wx, wweights);

		wx = _mm512_add_epi32(wx, _mm512_shuffle_epi32(wx, _MM_SHUFFLE(2, 3, 0, 1)));

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

	__m256i vt0 = *(const __m256i*)&gTableInterpolate3_U8[0];
	__m256i vt1 = *(const __m256i*)&gTableInterpolate3_U8[2];

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
		__m256i vpixel = _mm256_unpacklo_epi64(vpacked, vpacked);

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

		merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
		merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

		p++;

		if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
			goto done;
	}

	if (k & 1)
	{
		__m128i mpacked = _mm_load_si128((const __m128i*)p);
		__m256i vpixel = _mm256_broadcastq_epi64(mpacked);

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

		vx = _mm256_add_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
		vy = _mm256_add_epi32(vy, _mm256_shuffle_epi32(vy, _MM_SHUFFLE(2, 3, 0, 1)));

		vx = _mm256_min_epi32(vx, vy);
		vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

		merrorBlock = _mm_add_epi32(merrorBlock, _mm_min_epi32(_mm256_extracti128_si256(vx, 1), _mm256_castsi256_si128(vx)));
	}
done:
#else
	const __m128i mhalf = _mm_set1_epi16(32);

	mc = _mm_packus_epi16(mc, mc);

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

		mx = _mm_min_epi32(_mm_min_epi32(mx, my), _mm_min_epi32(mz, mw));
		mx = _mm_min_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(1, 0, 3, 2)));

		merrorBlock = _mm_add_epi32(merrorBlock, mx);

		if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
			break;
	}
#endif

	return _mm_cvtsi128_si32(merrorBlock);
}

template<int shuffle>
static INLINED int ComputeSubsetError3Pair(const Area& area, __m128i mc, const __m128i mweights, const __m128i mwater) noexcept
{
	__m128i merrorBlock = _mm_setzero_si128();

#if defined(OPTION_AVX512)
	const __m512i wweights = _mm512_broadcastq_epi64(mweights);

	const __m256i vhalf = _mm256_set1_epi16(32);

	mc = _mm_shuffle_epi32(mc, shuffle);
	mc = _mm_packus_epi16(mc, mc);
	__m256i vc = _mm256_broadcastq_epi64(mc);

	__m256i vt = *(const __m256i*)gTableInterpolate3GR_U8;

	vt = _mm256_maddubs_epi16(vc, vt);

	vt = _mm256_add_epi16(vt, vhalf);

	vt = _mm256_srli_epi16(vt, 6);

	__m512i wtx = _mm512_broadcast_i32x4(_mm256_castsi256_si128(vt));
	__m512i wty = _mm512_broadcast_i32x4(_mm256_extracti128_si256(vt, 1));

	int k = static_cast<int>(area.Active);
	const __m512i* p = (const __m512i*)area.DataMask_I16;

	while ((k -= 4) >= 0)
	{
		__m512i wpacked = _mm512_load_epi64(p);
		__m512i wpixel = _mm512_shufflelo_epi16(wpacked, shuffle);
		wpixel = _mm512_unpacklo_epi64(wpixel, wpixel);

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

		wx = _mm512_madd_epi16(wx, wweights);
		wy = _mm512_madd_epi16(wy, wweights);

		wx = _mm512_min_epi32(wx, wy);
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

		merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
		merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

		p = reinterpret_cast<const __m512i*>(reinterpret_cast<const __m256i*>(p) + 1);
	}

	if (k & 1)
	{
		__m128i mpacked = _mm_load_si128((const __m128i*)p);
		__m128i mpixel = _mm_shufflelo_epi16(mpacked, shuffle);
		__m256i vpixel = _mm256_broadcastq_epi64(mpixel);

		__m256i vx = _mm256_sub_epi16(vpixel, vt);

		vx = _mm256_abs_epi16(vx);

		if constexpr (!kDenoise)
		{
			vx = _mm256_adds_epu8(vx, vx);
		}

		vx = _mm256_srli_epi16(vx, kDenoiseShift);

		vx = _mm256_mullo_epi16(vx, vx);

		vx = _mm256_madd_epi16(vx, _mm512_castsi512_si256(wweights));

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

	__m256i vt = *(const __m256i*)gTableInterpolate3GR_U8;

	vt = _mm256_maddubs_epi16(vc, vt);

	vt = _mm256_add_epi16(vt, vhalf);

	vt = _mm256_srli_epi16(vt, 6);

	__m256i vtx = _mm256_permute4x64_epi64(vt, 0x44);
	__m256i vty = _mm256_permute4x64_epi64(vt, 0xEE);

	int k = static_cast<int>(area.Active);
	const __m256i* p = (const __m256i*)area.DataMask_I16;

	while ((k -= 4) >= 0)
	{
		__m256i vpacked0 = _mm256_load_si256(&p[0]);
		__m256i vpacked1 = _mm256_load_si256(&p[1]);
		__m256i vpixel0 = _mm256_shufflelo_epi16(vpacked0, shuffle);
		__m256i vpixel1 = _mm256_shufflelo_epi16(vpacked1, shuffle);
		vpixel0 = _mm256_unpacklo_epi64(vpixel0, vpixel0);
		vpixel1 = _mm256_unpacklo_epi64(vpixel1, vpixel1);

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

		vx = _mm256_madd_epi16(vx, vweights);
		vy = _mm256_madd_epi16(vy, vweights);
		vz = _mm256_madd_epi16(vz, vweights);
		vw = _mm256_madd_epi16(vw, vweights);

		vx = _mm256_min_epi32(vx, vy);
		vz = _mm256_min_epi32(vz, vw);
		vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
		vz = _mm256_min_epi32(vz, _mm256_shuffle_epi32(vz, _MM_SHUFFLE(2, 3, 0, 1)));
		vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));
		vz = _mm256_min_epi32(vz, _mm256_shuffle_epi32(vz, _MM_SHUFFLE(1, 0, 3, 2)));

		vx = _mm256_add_epi32(vx, vz);

		merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
		merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

		p += 2;

		if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
			goto done;
	}

	if (k & 2)
	{
		__m256i vpacked = _mm256_load_si256(p);
		__m256i vpixel = _mm256_shufflelo_epi16(vpacked, shuffle);
		vpixel = _mm256_unpacklo_epi64(vpixel, vpixel);

		__m256i vx = _mm256_sub_epi16(vpixel, vtx);
		__m256i vy = _mm256_sub_epi16(vpixel, vty);

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

		merrorBlock = _mm_add_epi32(merrorBlock, _mm256_castsi256_si128(vx));
		merrorBlock = _mm_add_epi32(merrorBlock, _mm256_extracti128_si256(vx, 1));

		p++;
	}

	if (k & 1)
	{
		__m128i mpacked = _mm_load_si128((const __m128i*)p);
		__m128i mpixel = _mm_shufflelo_epi16(mpacked, shuffle);
		__m256i vpixel = _mm256_broadcastq_epi64(mpixel);

		__m256i vx = _mm256_sub_epi16(vpixel, vt);

		vx = _mm256_abs_epi16(vx);

		if constexpr (!kDenoise)
		{
			vx = _mm256_adds_epu8(vx, vx);
		}

		vx = _mm256_srli_epi16(vx, kDenoiseShift);

		vx = _mm256_mullo_epi16(vx, vx);

		vx = _mm256_madd_epi16(vx, vweights);

		vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(2, 3, 0, 1)));
		vx = _mm256_min_epi32(vx, _mm256_shuffle_epi32(vx, _MM_SHUFFLE(1, 0, 3, 2)));

		merrorBlock = _mm_add_epi32(merrorBlock, _mm_min_epi32(_mm256_extracti128_si256(vx, 1), _mm256_castsi256_si128(vx)));
	}
done:
#else
	const __m128i mhalf = _mm_set1_epi16(32);

	mc = _mm_shuffle_epi32(mc, shuffle);
	mc = _mm_packus_epi16(mc, mc);

	__m128i mtx = gTableInterpolate3GR_U8[0];
	__m128i mty = gTableInterpolate3GR_U8[1];

	mtx = _mm_maddubs_epi16(mc, mtx);
	mty = _mm_maddubs_epi16(mc, mty);

	mtx = _mm_add_epi16(mtx, mhalf);
	mty = _mm_add_epi16(mty, mhalf);

	mtx = _mm_srli_epi16(mtx, 6);
	mty = _mm_srli_epi16(mty, 6);

	for (size_t i = 0, n = area.Active; i < n; i++)
	{
		__m128i mpacked = _mm_load_si128(&area.DataMask_I16[i]);
		__m128i mpixel = _mm_shufflelo_epi16(mpacked, shuffle);
		mpixel = _mm_unpacklo_epi64(mpixel, mpixel);

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

		mx = _mm_madd_epi16(mx, mweights);
		my = _mm_madd_epi16(my, mweights);

		mx = _mm_min_epi32(mx, my);
		mx = _mm_min_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(2, 3, 0, 1)));
		mx = _mm_min_epi32(mx, _mm_shuffle_epi32(mx, _MM_SHUFFLE(1, 0, 3, 2)));

		merrorBlock = _mm_add_epi32(merrorBlock, mx);

		if (!(_mm_movemask_epi8(_mm_cmpgt_epi32(mwater, merrorBlock)) & 0xF))
			break;
	}
#endif

	return _mm_cvtsi128_si32(merrorBlock);
}

static INLINED int ComputeOpaqueSubsetTable3(const Area& area, __m128i mc, uint64_t& indices) noexcept
{
	mc = _mm_or_si128(mc, _mm_set_epi16(0, 0, 0, 0, 0, 0, 255, 255));

	const __m128i mhalf = _mm_set1_epi16(32);

	mc = _mm_packus_epi16(mc, mc);

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

	Modulations state;
	_mm_store_si128((__m128i*)&state.Values_I16[0], mtx);
	_mm_store_si128((__m128i*)&state.Values_I16[2], mty);
	_mm_store_si128((__m128i*)&state.Values_I16[4], mtz);
	_mm_store_si128((__m128i*)&state.Values_I16[6], mtw);

	int error = ComputeSubsetTable3(area, gWeightsAGRB, state);

	for (size_t i = 0, n = area.Count; i < n; i++)
	{
		uint64_t index = static_cast<uint32_t>(state.Best[i]);
		indices |= index << (area.Indices[i] * 3);
	}

	return error;
}
