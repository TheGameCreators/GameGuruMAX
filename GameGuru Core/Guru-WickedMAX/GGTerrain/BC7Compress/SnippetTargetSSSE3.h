
#include <tmmintrin.h> // SSSE3

#undef _mm_cvtepu8_epi16
#define _mm_cvtepu8_epi16 emu_cvtepu8_epi16

static ALWAYS_INLINED __m128i emu_cvtepu8_epi16(__m128i a)
{
	return _mm_unpacklo_epi8(a, _mm_setzero_si128());
}

#undef _mm_cvtepu8_epi32
#define _mm_cvtepu8_epi32 emu_cvtepu8_epi32

static ALWAYS_INLINED __m128i emu_cvtepu8_epi32(__m128i a)
{
	__m128i zero = _mm_setzero_si128();
	a = _mm_unpacklo_epi8(a, zero);
	return _mm_unpacklo_epi16(a, zero);
}

#undef _mm_cvtepi8_epi16
#define _mm_cvtepi8_epi16 emu_cvtepi8_epi16_mask

static ALWAYS_INLINED __m128i emu_cvtepi8_epi16_mask(__m128i a)
{
	return _mm_unpacklo_epi8(a, a);
}

#undef _mm_cvtepu16_epi32
#define _mm_cvtepu16_epi32 emu_cvtepu16_epi32

static ALWAYS_INLINED __m128i emu_cvtepu16_epi32(__m128i a)
{
	return _mm_unpacklo_epi16(a, _mm_setzero_si128());
}

#undef _mm_min_epi32
#define _mm_min_epi32 emu_min_epi32

static ALWAYS_INLINED __m128i emu_min_epi32(__m128i a, __m128i b)
{
	__m128i mask = _mm_cmplt_epi32(a, b);
	return _mm_or_si128(_mm_and_si128(a, mask), _mm_andnot_si128(mask, b));
}

#undef _mm_min_epu16
#define _mm_min_epu16 emu_min_epu16

static ALWAYS_INLINED __m128i emu_min_epu16(__m128i a, __m128i b)
{
	__m128i sign = _mm_set1_epi16(-0x8000);
	__m128i mask = _mm_cmplt_epi16(_mm_xor_si128(a, sign), _mm_xor_si128(sign, b));
	return _mm_or_si128(_mm_and_si128(a, mask), _mm_andnot_si128(mask, b));
}

#undef _mm_minpos_epu16
#define _mm_minpos_epu16 emu_minpos_epu16_min

static ALWAYS_INLINED __m128i emu_minpos_epu16_min(__m128i a)
{
	__m128i zero = _mm_setzero_si128();
	a = _mm_min_epi32(_mm_unpacklo_epi16(a, zero), _mm_unpackhi_epi16(a, zero));
	a = _mm_min_epi32(a, _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1)));
	a = _mm_min_epi32(a, _mm_shuffle_epi32(a, _MM_SHUFFLE(0, 1, 2, 3)));
	return a;
}

#undef  _mm_mullo_epi32
#define _mm_mullo_epi32 emu_mullo_epi32

static ALWAYS_INLINED __m128i emu_mullo_epi32(__m128i a, __m128i b)
{
	__m128i c = _mm_mul_epu32(a, b);
	a = _mm_shuffle_epi32(a, _MM_SHUFFLE(2, 3, 0, 1));
	b = _mm_shuffle_epi32(b, _MM_SHUFFLE(2, 3, 0, 1));
	__m128i d = _mm_mul_epu32(a, b);
	c = _mm_shuffle_epi32(c, _MM_SHUFFLE(2, 0, 2, 0));
	d = _mm_shuffle_epi32(d, _MM_SHUFFLE(2, 0, 2, 0));
	return _mm_unpacklo_epi32(c, d);
}

#undef  _mm_blendv_epi8
#define _mm_blendv_epi8 emu_blendv_epi8

static ALWAYS_INLINED __m128i emu_blendv_epi8(__m128i a, __m128i b, __m128i mask)
{
	return _mm_or_si128(_mm_and_si128(b, mask), _mm_andnot_si128(mask, a));
}

#undef  _mm_blend_epi16
#define _mm_blend_epi16 emu_blend_epi16

static ALWAYS_INLINED __m128i emu_blend_epi16(__m128i a, __m128i b, int imm8)
{
	__m128i mask = _mm_set_epi16(
		(imm8 & 0x80) ? -1 : 0,
		(imm8 & 0x40) ? -1 : 0,
		(imm8 & 0x20) ? -1 : 0,
		(imm8 & 0x10) ? -1 : 0,
		(imm8 & 8) ? -1 : 0,
		(imm8 & 4) ? -1 : 0,
		(imm8 & 2) ? -1 : 0,
		(imm8 & 1) ? -1 : 0
	);

	return emu_blendv_epi8(a, b, mask);
}

#undef  _mm_extract_epi32
#define _mm_extract_epi32 emu_extract_epi32

static ALWAYS_INLINED int emu_extract_epi32(__m128i a, int imm8)
{
	switch (imm8)
	{
	case 0:
	{
		int H = _mm_extract_epi16(a, 1);
		int L = _mm_extract_epi16(a, 0);
		return static_cast<int>((static_cast<uint32_t>(H) << 16) | static_cast<uint32_t>(L));
	}

	case 1:
	{
		int H = _mm_extract_epi16(a, 3);
		int L = _mm_extract_epi16(a, 2);
		return static_cast<int>((static_cast<uint32_t>(H) << 16) | static_cast<uint32_t>(L));
	}

	case 2:
	{
		int H = _mm_extract_epi16(a, 5);
		int L = _mm_extract_epi16(a, 4);
		return static_cast<int>((static_cast<uint32_t>(H) << 16) | static_cast<uint32_t>(L));
	}

	case 3:
	{
		int H = _mm_extract_epi16(a, 7);
		int L = _mm_extract_epi16(a, 6);
		return static_cast<int>((static_cast<uint32_t>(H) << 16) | static_cast<uint32_t>(L));
	}

	default:
		return 0;
	}
}

#undef  _mm_blend_ps
#define _mm_blend_ps emu_blend_ps

static ALWAYS_INLINED __m128 emu_blend_ps(__m128 a, __m128 b, int imm8)
{
	__m128 mask = _mm_castsi128_ps(_mm_set_epi32(
		(imm8 & 8) ? -1 : 0,
		(imm8 & 4) ? -1 : 0,
		(imm8 & 2) ? -1 : 0,
		(imm8 & 1) ? -1 : 0
	));

	return _mm_or_ps(_mm_and_ps(b, mask), _mm_andnot_ps(mask, a));
}

#undef  _mm_dp_ps
#define _mm_dp_ps emu_dp_ps

static ALWAYS_INLINED __m128 emu_dp_ps(__m128 a, __m128 b, int imm8)
{
	__m128 c = _mm_mul_ps(a, b);

	__m128 maskH = _mm_castsi128_ps(_mm_set_epi32(
		(imm8 & 0x80) ? -1 : 0,
		(imm8 & 0x40) ? -1 : 0,
		(imm8 & 0x20) ? -1 : 0,
		(imm8 & 0x10) ? -1 : 0
	));

	c = _mm_and_ps(c, maskH);

	c = _mm_add_ps(c, _mm_movehdup_ps(c));
	c = _mm_add_ps(c, _mm_movehl_ps(_mm_setzero_ps(), c));
	c = _mm_shuffle_ps(c, c, 0);

	__m128 maskL = _mm_castsi128_ps(_mm_set_epi32(
		(imm8 & 8) ? -1 : 0,
		(imm8 & 4) ? -1 : 0,
		(imm8 & 2) ? -1 : 0,
		(imm8 & 1) ? -1 : 0
	));

	return _mm_and_ps(c, maskL);
}
