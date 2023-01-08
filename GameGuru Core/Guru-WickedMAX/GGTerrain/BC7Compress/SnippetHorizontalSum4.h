#pragma once

#include "pch.h"

static ALWAYS_INLINED __m128i HorizontalSum4(__m128i me4) noexcept
{
	__m128i me2 = _mm_add_epi32(me4, _mm_shuffle_epi32(me4, _MM_SHUFFLE(2, 3, 0, 1)));
	__m128i me1 = _mm_add_epi32(me2, _mm_shuffle_epi32(me2, _MM_SHUFFLE(0, 1, 2, 3)));
	return me1;
}
