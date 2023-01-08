#pragma once

#include "pch.h"
#include "Bc7Tables.h"

template<int bits>
static INLINED void DecompressIndexedSubset(__m128i mc, uint64_t indices, int* output, uint64_t data) noexcept
{
	const __m128i mhalf = _mm_set1_epi16(32);

	mc = _mm_packus_epi16(mc, mc);

	uint64_t count = indices & 0xF; indices >>= 4;
	do
	{
		uint64_t index = indices & 0xF; indices >>= 4;

		__m128i mratio;
		if constexpr (bits == 2)
		{
			mratio = _mm_loadl_epi64((const __m128i*)&((const uint64_t*)gTableInterpolate2_U8)[(data >> (index + index)) & 3]);
		}
		else if constexpr (bits == 3)
		{
			mratio = _mm_loadl_epi64((const __m128i*)&((const uint64_t*)gTableInterpolate3_U8)[(data >> (index + index + index)) & 7]);
		}
		else
		{
			static_assert((bits == 2) || (bits == 3));
		}

		__m128i mx = _mm_maddubs_epi16(mc, mratio);
		mx = _mm_add_epi16(mx, mhalf);
		mx = _mm_srli_epi16(mx, 6);

		output[index] = _mm_cvtsi128_si32(_mm_packus_epi16(mx, mx));

	} while (--count);
}
