#pragma once

#include "pch.h"
#include "Bc7Core.h"

#if defined(OPTION_AVX512)

template<int step>
static ALWAYS_INLINED NodeShort* Store16(NodeShort* nodesPtr, __m256i vsum, uint32_t flags, const int c) noexcept
{
	__m512i wc = _mm512_broadcastd_epi32(_mm_cvtsi32_si128(c));

	uint64_t positions = _pdep_u64(flags, 0x1111111111111111uLL) * 0xF;

	__m512i wsum = _mm512_cvtepu16_epi32(vsum);

	uint64_t idx = _pext_u64(0xFEDCBA9876543210uLL, positions);

	wc = _mm512_add_epi32(wc, _mm512_set_epi32(
		15 * step, 14 * step, 13 * step, 12 * step, 11 * step, 10 * step, 9 * step, 8 * step,
		7 * step, 6 * step, 5 * step, 4 * step, 3 * step, 2 * step, 1 * step, 0 * step));

	__m512i widx = _mm512_cvtepu8_epi64(_mm_cvtsi64_si128(static_cast<int64_t>(idx)));

	wc = _mm512_or_epi64(wc, _mm512_slli_epi32(wsum, 16));

	widx = _mm512_or_epi64(widx, _mm512_slli_epi64(widx, 32 - 4));

	__m512i wnodes = _mm512_permutexvar_epi32(widx, wc);

	uint32_t count = static_cast<uint32_t>(_mm_popcnt_u32(flags));

	_mm512_storeu_epi32(reinterpret_cast<__m512i*>(nodesPtr), wnodes);
	return nodesPtr + count;
}

#else

template<int step>
static ALWAYS_INLINED NodeShort* Store8(NodeShort* nodesPtr, __m128i msum, uint32_t flags, const int c) noexcept
{
	__m128i mc = _mm_shuffle_epi32(_mm_shufflelo_epi16(_mm_cvtsi32_si128(c), 0), 0);

	mc = _mm_add_epi16(mc, _mm_set_epi16(7 * step, 6 * step, 5 * step, 4 * step, 3 * step, 2 * step, 1 * step, 0 * step));

	__m128i mx = _mm_unpacklo_epi16(mc, msum);
	__m128i my = _mm_unpackhi_epi16(mc, msum);

	{
		nodesPtr->ColorError = static_cast<uint32_t>(_mm_cvtsi128_si32(mx));
		nodesPtr += flags & 1u;

		mx = _mm_shuffle_epi32(mx, _MM_SHUFFLE(0, 3, 2, 1));
		flags >>= 2;
	}
	{
		nodesPtr->ColorError = static_cast<uint32_t>(_mm_cvtsi128_si32(mx));
		nodesPtr += flags & 1u;

		mx = _mm_shuffle_epi32(mx, _MM_SHUFFLE(0, 3, 2, 1));
		flags >>= 2;
	}
	{
		nodesPtr->ColorError = static_cast<uint32_t>(_mm_cvtsi128_si32(mx));
		nodesPtr += flags & 1u;

		mx = _mm_shuffle_epi32(mx, _MM_SHUFFLE(0, 3, 2, 1));
		flags >>= 2;
	}
	{
		nodesPtr->ColorError = static_cast<uint32_t>(_mm_cvtsi128_si32(mx));
		nodesPtr += flags & 1u;
	}
	flags >>= 2;
	{
		nodesPtr->ColorError = static_cast<uint32_t>(_mm_cvtsi128_si32(my));
		nodesPtr += flags & 1u;

		my = _mm_shuffle_epi32(my, _MM_SHUFFLE(0, 3, 2, 1));
		flags >>= 2;
	}
	{
		nodesPtr->ColorError = static_cast<uint32_t>(_mm_cvtsi128_si32(my));
		nodesPtr += flags & 1u;

		my = _mm_shuffle_epi32(my, _MM_SHUFFLE(0, 3, 2, 1));
		flags >>= 2;
	}
	{
		nodesPtr->ColorError = static_cast<uint32_t>(_mm_cvtsi128_si32(my));
		nodesPtr += flags & 1u;

		my = _mm_shuffle_epi32(my, _MM_SHUFFLE(0, 3, 2, 1));
		flags >>= 2;
	}
	{
		nodesPtr->ColorError = static_cast<uint32_t>(_mm_cvtsi128_si32(my));
		nodesPtr += flags & 1u;
	}

	return nodesPtr;
}

#endif
