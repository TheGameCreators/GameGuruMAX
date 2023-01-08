
#include "pch.h"
#include "Bc7Tables.h"

// https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_texture_compression_bptc.txt

alignas(16) static constexpr short gTableInterpolate2[4][2] =
{
	{ 64 - 0, 0 },
	{ 64 - 21, 21 },

	{ 64 - 43, 43 },
	{ 64 - 64, 64 }
};

alignas(16) static constexpr short gTableInterpolate3[8][2] =
{
	{ 64 - 0, 0 },
	{ 64 - 9, 9 },
	{ 64 - 18, 18 },
	{ 64 - 27, 27 },

	{ 64 - 37, 37 },
	{ 64 - 46, 46 },
	{ 64 - 55, 55 },
	{ 64 - 64, 64 }
};

alignas(16) static constexpr short gTableInterpolate4[16][2] =
{
	{ 64 - 0, 0 },
	{ 64 - 4, 4 },
	{ 64 - 9, 9 },
	{ 64 - 13, 13 },
	{ 64 - 17, 17 },
	{ 64 - 21, 21 },
	{ 64 - 26, 26 },
	{ 64 - 30, 30 },

	{ 64 - 34, 34 },
	{ 64 - 38, 38 },
	{ 64 - 43, 43 },
	{ 64 - 47, 47 },
	{ 64 - 51, 51 },
	{ 64 - 55, 55 },
	{ 64 - 60, 60 },
	{ 64 - 64, 64 }
};

alignas(32) __m128i gTableInterpolate2_U8[4 >> 1];
alignas(64) __m128i gTableInterpolate3_U8[8 >> 1];
alignas(32) __m128i gTableInterpolate4_U8[16 >> 1];

alignas(32) __m128i gTableInterpolate2GR_U8[4 >> 2];
alignas(32) __m128i gTableInterpolate3GR_U8[8 >> 2];
alignas(32) __m128i gTableInterpolate4GR_U8[16 >> 2];

alignas(32) __m128i gTableInterpolate2n3_U8[8 >> 1];
alignas(32) __m128i gTableInterpolate3n2_U8[8 >> 1];

void InitInterpolation() noexcept
{
	const __m128i mfill = _mm_set_epi8(6, 4, 6, 4, 6, 4, 6, 4, 2, 0, 2, 0, 2, 0, 2, 0);

	{
		__m128i m0 = _mm_loadl_epi64((const __m128i*)gTableInterpolate2[0]);
		__m128i m1 = _mm_loadl_epi64((const __m128i*)gTableInterpolate2[2]);

		m0 = _mm_shuffle_epi8(m0, mfill);
		m1 = _mm_shuffle_epi8(m1, mfill);

		gTableInterpolate2_U8[0] = m0;
		gTableInterpolate2_U8[1] = m1;

		m0 = _mm_shuffle_epi32(m0, _MM_SHUFFLE(2, 0, 2, 0));
		m1 = _mm_shuffle_epi32(m1, _MM_SHUFFLE(2, 0, 2, 0));

		gTableInterpolate2GR_U8[0] = _mm_unpacklo_epi64(m0, m1);
	}

	{
		__m128i m0 = _mm_loadl_epi64((const __m128i*)gTableInterpolate3[0]);
		__m128i m1 = _mm_loadl_epi64((const __m128i*)gTableInterpolate3[2]);
		__m128i m2 = _mm_loadl_epi64((const __m128i*)gTableInterpolate3[4]);
		__m128i m3 = _mm_loadl_epi64((const __m128i*)gTableInterpolate3[6]);

		m0 = _mm_shuffle_epi8(m0, mfill);
		m1 = _mm_shuffle_epi8(m1, mfill);
		m2 = _mm_shuffle_epi8(m2, mfill);
		m3 = _mm_shuffle_epi8(m3, mfill);

		gTableInterpolate3_U8[0] = m0;
		gTableInterpolate3_U8[1] = m1;
		gTableInterpolate3_U8[2] = m2;
		gTableInterpolate3_U8[3] = m3;

		m0 = _mm_shuffle_epi32(m0, _MM_SHUFFLE(2, 0, 2, 0));
		m1 = _mm_shuffle_epi32(m1, _MM_SHUFFLE(2, 0, 2, 0));
		m2 = _mm_shuffle_epi32(m2, _MM_SHUFFLE(2, 0, 2, 0));
		m3 = _mm_shuffle_epi32(m3, _MM_SHUFFLE(2, 0, 2, 0));

		gTableInterpolate3GR_U8[0] = _mm_unpacklo_epi64(m0, m1);
		gTableInterpolate3GR_U8[1] = _mm_unpacklo_epi64(m2, m3);
	}

	{
		__m128i m0 = _mm_loadl_epi64((const __m128i*)gTableInterpolate4[0]);
		__m128i m1 = _mm_loadl_epi64((const __m128i*)gTableInterpolate4[2]);
		__m128i m2 = _mm_loadl_epi64((const __m128i*)gTableInterpolate4[4]);
		__m128i m3 = _mm_loadl_epi64((const __m128i*)gTableInterpolate4[6]);
		__m128i m4 = _mm_loadl_epi64((const __m128i*)gTableInterpolate4[8]);
		__m128i m5 = _mm_loadl_epi64((const __m128i*)gTableInterpolate4[10]);
		__m128i m6 = _mm_loadl_epi64((const __m128i*)gTableInterpolate4[12]);
		__m128i m7 = _mm_loadl_epi64((const __m128i*)gTableInterpolate4[14]);

		m0 = _mm_shuffle_epi8(m0, mfill);
		m1 = _mm_shuffle_epi8(m1, mfill);
		m2 = _mm_shuffle_epi8(m2, mfill);
		m3 = _mm_shuffle_epi8(m3, mfill);
		m4 = _mm_shuffle_epi8(m4, mfill);
		m5 = _mm_shuffle_epi8(m5, mfill);
		m6 = _mm_shuffle_epi8(m6, mfill);
		m7 = _mm_shuffle_epi8(m7, mfill);

		gTableInterpolate4_U8[0] = m0;
		gTableInterpolate4_U8[1] = m1;
		gTableInterpolate4_U8[2] = m2;
		gTableInterpolate4_U8[3] = m3;
		gTableInterpolate4_U8[4] = m4;
		gTableInterpolate4_U8[5] = m5;
		gTableInterpolate4_U8[6] = m6;
		gTableInterpolate4_U8[7] = m7;

		m0 = _mm_shuffle_epi32(m0, _MM_SHUFFLE(2, 0, 2, 0));
		m1 = _mm_shuffle_epi32(m1, _MM_SHUFFLE(2, 0, 2, 0));
		m2 = _mm_shuffle_epi32(m2, _MM_SHUFFLE(2, 0, 2, 0));
		m3 = _mm_shuffle_epi32(m3, _MM_SHUFFLE(2, 0, 2, 0));
		m4 = _mm_shuffle_epi32(m4, _MM_SHUFFLE(2, 0, 2, 0));
		m5 = _mm_shuffle_epi32(m5, _MM_SHUFFLE(2, 0, 2, 0));
		m6 = _mm_shuffle_epi32(m6, _MM_SHUFFLE(2, 0, 2, 0));
		m7 = _mm_shuffle_epi32(m7, _MM_SHUFFLE(2, 0, 2, 0));

		gTableInterpolate4GR_U8[0] = _mm_unpacklo_epi64(m0, m1);
		gTableInterpolate4GR_U8[1] = _mm_unpacklo_epi64(m2, m3);
		gTableInterpolate4GR_U8[2] = _mm_unpacklo_epi64(m4, m5);
		gTableInterpolate4GR_U8[3] = _mm_unpacklo_epi64(m6, m7);
	}

	{
		__m128i mratio2L = gTableInterpolate2_U8[0];
		__m128i mratio2H = gTableInterpolate2_U8[1];

		__m128i m0 = gTableInterpolate3_U8[0];
		__m128i m1 = gTableInterpolate3_U8[1];
		__m128i m2 = gTableInterpolate3_U8[2];
		__m128i m3 = gTableInterpolate3_U8[3];

		gTableInterpolate2n3_U8[0] = _mm_blend_epi16(mratio2L, m0, 0x11);
		gTableInterpolate2n3_U8[1] = _mm_blend_epi16(mratio2H, m1, 0x11);
		gTableInterpolate2n3_U8[2] = _mm_blend_epi16(mratio2L, m2, 0x11);
		gTableInterpolate2n3_U8[3] = _mm_blend_epi16(mratio2H, m3, 0x11);

		gTableInterpolate3n2_U8[0] = _mm_blend_epi16(mratio2L, m0, 0xEE);
		gTableInterpolate3n2_U8[1] = _mm_blend_epi16(mratio2H, m1, 0xEE);
		gTableInterpolate3n2_U8[2] = _mm_blend_epi16(mratio2L, m2, 0xEE);
		gTableInterpolate3n2_U8[3] = _mm_blend_epi16(mratio2H, m3, 0xEE);
	}
}


alignas(32) static constexpr uint8_t gTablePartitioning2[64][16] =
{
	{ 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 },
	{ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
	{ 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 },
	{ 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1 },
	{ 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1 },
	{ 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1 },
	{ 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1 },

	{ 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1 },
	{ 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0 },
	{ 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
	{ 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0 },
	{ 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1 },
	{ 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0 },
	{ 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0 },
	{ 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0 },
	{ 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0 },
	{ 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0 },

	{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1 },
	{ 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0 },
	{ 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0 },
	{ 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0 },
	{ 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0 },
	{ 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1 },
	{ 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1 },
	{ 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0 },
	{ 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0 },
	{ 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0 },
	{ 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0 },
	{ 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 },
	{ 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1 },
	{ 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1 },
	{ 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0 },

	{ 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0 },
	{ 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0 },
	{ 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1 },
	{ 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1 },
	{ 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0 },
	{ 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0 },
	{ 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1 },
	{ 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1 },
	{ 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1 },
	{ 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 },
	{ 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1 },
};

alignas(32) static constexpr uint8_t gTablePartitioning3[64][16] =
{
	{ 0, 0, 1, 1, 0, 0, 1, 1, 0, 2, 2, 1, 2, 2, 2, 2 },
	{ 0, 0, 0, 1, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2, 2, 1 },
	{ 0, 0, 0, 0, 2, 0, 0, 1, 2, 2, 1, 1, 2, 2, 1, 1 },
	{ 0, 2, 2, 2, 0, 0, 2, 2, 0, 0, 1, 1, 0, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2 },
	{ 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 2, 2, 0, 0, 2, 2 },
	{ 0, 0, 2, 2, 0, 0, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2 },
	{ 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2, 0, 1, 1, 2 },
	{ 0, 1, 2, 2, 0, 1, 2, 2, 0, 1, 2, 2, 0, 1, 2, 2 },
	{ 0, 0, 1, 1, 0, 1, 1, 2, 1, 1, 2, 2, 1, 2, 2, 2 },
	{ 0, 0, 1, 1, 2, 0, 0, 1, 2, 2, 0, 0, 2, 2, 2, 0 },

	{ 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 2, 1, 1, 2, 2 },
	{ 0, 1, 1, 1, 0, 0, 1, 1, 2, 0, 0, 1, 2, 2, 0, 0 },
	{ 0, 0, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2 },
	{ 0, 0, 2, 2, 0, 0, 2, 2, 0, 0, 2, 2, 1, 1, 1, 1 },
	{ 0, 1, 1, 1, 0, 1, 1, 1, 0, 2, 2, 2, 0, 2, 2, 2 },
	{ 0, 0, 0, 1, 0, 0, 0, 1, 2, 2, 2, 1, 2, 2, 2, 1 },
	{ 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 2, 2, 0, 1, 2, 2 },
	{ 0, 0, 0, 0, 1, 1, 0, 0, 2, 2, 1, 0, 2, 2, 1, 0 },
	{ 0, 1, 2, 2, 0, 1, 2, 2, 0, 0, 1, 1, 0, 0, 0, 0 },
	{ 0, 0, 1, 2, 0, 0, 1, 2, 1, 1, 2, 2, 2, 2, 2, 2 },
	{ 0, 1, 1, 0, 1, 2, 2, 1, 1, 2, 2, 1, 0, 1, 1, 0 },
	{ 0, 0, 0, 0, 0, 1, 1, 0, 1, 2, 2, 1, 1, 2, 2, 1 },
	{ 0, 0, 2, 2, 1, 1, 0, 2, 1, 1, 0, 2, 0, 0, 2, 2 },
	{ 0, 1, 1, 0, 0, 1, 1, 0, 2, 0, 0, 2, 2, 2, 2, 2 },
	{ 0, 0, 1, 1, 0, 1, 2, 2, 0, 1, 2, 2, 0, 0, 1, 1 },
	{ 0, 0, 0, 0, 2, 0, 0, 0, 2, 2, 1, 1, 2, 2, 2, 1 },

	{ 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 2, 2, 2 },
	{ 0, 2, 2, 2, 0, 0, 2, 2, 0, 0, 1, 2, 0, 0, 1, 1 },
	{ 0, 0, 1, 1, 0, 0, 1, 2, 0, 0, 2, 2, 0, 2, 2, 2 },
	{ 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0 },
	{ 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0, 0, 0 },
	{ 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0, 1, 2, 0 },
	{ 0, 1, 2, 0, 2, 0, 1, 2, 1, 2, 0, 1, 0, 1, 2, 0 },
	{ 0, 0, 1, 1, 2, 2, 0, 0, 1, 1, 2, 2, 0, 0, 1, 1 },
	{ 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 0, 0, 0, 0, 1, 1 },
	{ 0, 1, 0, 1, 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 1, 2, 1, 2, 1 },
	{ 0, 0, 2, 2, 1, 1, 2, 2, 0, 0, 2, 2, 1, 1, 2, 2 },
	{ 0, 0, 2, 2, 0, 0, 1, 1, 0, 0, 2, 2, 0, 0, 1, 1 },
	{ 0, 2, 2, 0, 1, 2, 2, 1, 0, 2, 2, 0, 1, 2, 2, 1 },
	{ 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1, 0, 1 },
	{ 0, 0, 0, 0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1 },

	{ 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 2, 2, 2, 2 },
	{ 0, 2, 2, 2, 0, 1, 1, 1, 0, 2, 2, 2, 0, 1, 1, 1 },
	{ 0, 0, 0, 2, 1, 1, 1, 2, 0, 0, 0, 2, 1, 1, 1, 2 },
	{ 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2 },
	{ 0, 2, 2, 2, 0, 1, 1, 1, 0, 1, 1, 1, 0, 2, 2, 2 },
	{ 0, 0, 0, 2, 1, 1, 1, 2, 1, 1, 1, 2, 0, 0, 0, 2 },
	{ 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 2, 2, 2, 2 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 1, 2 },
	{ 0, 1, 1, 0, 0, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 0, 0, 2, 2, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 2, 2 },
	{ 0, 0, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 0, 0, 2, 2 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 2 },
	{ 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1 },
	{ 0, 2, 2, 2, 1, 2, 2, 2, 0, 2, 2, 2, 1, 2, 2, 2 },
	{ 0, 1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
	{ 0, 1, 1, 1, 2, 0, 1, 1, 2, 2, 0, 1, 2, 2, 2, 0 },
};

alignas(32) const uint8_t gTableShrinked22[64] =
{
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 2, 8, 2, 2, 8, 8, 15,
	2, 8, 2, 2, 8, 8, 2, 2,
	15, 15, 6, 8, 2, 8, 15, 15,
	2, 8, 2, 2, 2, 15, 15, 6,
	6, 2, 6, 8, 15, 15, 2, 2,
	15, 15, 15, 15, 15, 2, 2, 15
};

alignas(32) static constexpr uint8_t gTableShrinked23[64] =
{
	3, 3, 15, 15, 8, 3, 15, 15,
	8, 8, 6, 6, 6, 5, 3, 3,
	3, 3, 8, 15, 3, 3, 6, 10,
	5, 8, 8, 6, 8, 5, 15, 15,
	8, 15, 3, 5, 6, 10, 8, 15,
	15, 3, 15, 5, 15, 15, 15, 15,
	3, 15, 5, 5, 5, 8, 5, 10,
	5, 10, 8, 13, 15, 12, 3, 3
};

alignas(32) static constexpr uint8_t gTableShrinked33[64] =
{
	15, 8, 8, 3, 15, 15, 3, 8,
	15, 15, 15, 15, 15, 15, 15, 8,
	15, 8, 15, 3, 15, 8, 15, 8,
	3, 15, 6, 10, 15, 15, 10, 8,
	15, 3, 15, 10, 10, 8, 9, 10,
	6, 15, 8, 15, 3, 6, 6, 8,
	15, 3, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 3, 15, 15, 8
};

alignas(32) uint8_t gTableShrinkedLow3[64];
alignas(32) uint8_t gTableShrinkedHigh3[64];

void InitShrinked() noexcept
{
	for (size_t partitionIndex = 0; partitionIndex < 64; partitionIndex++)
	{
		uint8_t a = gTableShrinked23[partitionIndex];
		uint8_t b = gTableShrinked33[partitionIndex];

		if (a > b)
		{
			uint8_t c = a; a = b; b = c;
		}

		gTableShrinkedLow3[partitionIndex] = a;
		gTableShrinkedHigh3[partitionIndex] = b;
	}
}


alignas(32) uint64_t gTableSelection12[64];
alignas(32) uint64_t gTableSelection22[64];

alignas(32) uint64_t gTableSelection13[64];
alignas(32) uint64_t gTableSelection23[64];
alignas(32) uint64_t gTableSelection33[64];

void InitSelection() noexcept
{
	for (size_t partitionIndex = 0; partitionIndex < 64; partitionIndex++)
	{
		// Two subsets
		{
			auto partition = gTablePartitioning2[partitionIndex];

			const uint64_t stop2 = gTableShrinked22[partitionIndex];

			uint64_t indices1 = 0;
			uint64_t indices2 = stop2;

			int count1 = 1;
			int count2 = 1;

			for (uint64_t index = 1; index < 16; index++)
			{
				const uint8_t subsetIndex = partition[index];
				if (subsetIndex == 0)
				{
					indices1 |= (index << (count1 << 2));
					count1++;
				}
				else
				{
					if (index != stop2)
					{
						indices2 |= (index << (count2 << 2));
						count2++;
					}
				}
			}

			indices1 <<= 4;
			indices2 <<= 4;

			indices1 |= static_cast<uint64_t>(count1);
			indices2 |= static_cast<uint64_t>(count2);

			gTableSelection12[partitionIndex] = indices1;
			gTableSelection22[partitionIndex] = indices2;
		}

		// Three subsets
		{
			auto partition = gTablePartitioning3[partitionIndex];

			const uint64_t stop2 = gTableShrinked23[partitionIndex];
			const uint64_t stop3 = gTableShrinked33[partitionIndex];

			uint64_t indices1 = 0;
			uint64_t indices2 = stop2;
			uint64_t indices3 = stop3;

			int count1 = 1;
			int count2 = 1;
			int count3 = 1;

			for (uint64_t index = 1; index < 16; index++)
			{
				const uint8_t subsetIndex = partition[index];
				if (subsetIndex == 0)
				{
					indices1 |= (index << (count1 << 2));
					count1++;
				}
				else if (subsetIndex == 1)
				{
					if (index != stop2)
					{
						indices2 |= (index << (count2 << 2));
						count2++;
					}
				}
				else
				{
					if (index != stop3)
					{
						indices3 |= (index << (count3 << 2));
						count3++;
					}
				}
			}

			indices1 <<= 4;
			indices2 <<= 4;
			indices3 <<= 4;

			indices1 |= static_cast<uint64_t>(count1);
			indices2 |= static_cast<uint64_t>(count2);
			indices3 |= static_cast<uint64_t>(count3);

			gTableSelection13[partitionIndex] = indices1;
			gTableSelection23[partitionIndex] = indices2;
			gTableSelection33[partitionIndex] = indices3;
		}
	}
}


alignas(64) uint8_t gTableDeltas2_Value8[0x100][0x100 * 0x100];
alignas(64) uint8_t gTableDeltas2_Value7[0x100][0x80 * 0x80];
alignas(64) uint8_t gTableDeltas2_Value6[0x100][0x40 * 0x40];
alignas(64) uint8_t gTableDeltas2_Value5[0x100][0x20 * 0x20];

alignas(64) uint16_t gTableCuts2_Value8[0x100][0x100];
alignas(64) uint16_t gTableCuts2_Value6[0x100][0x40];
alignas(64) uint16_t gTableCuts2_Value5[0x100][0x20];

alignas(64) uint8_t gTableDeltas3_Value7Shared[0x100][0x80 * 0x80];
alignas(64) uint8_t gTableDeltas3_Value6[0x100][0x40 * 0x40];
alignas(64) uint8_t gTableDeltas3_Value5[0x100][0x20 * 0x20];

alignas(64) uint16_t gTableCuts3_Value7Shared[0x100][0x80];
alignas(64) uint16_t gTableCuts3_Value5[0x100][0x20];

alignas(64) uint8_t gTableDeltas4Half_Value8[0x100][0x100 * 0x80];
alignas(64) uint8_t gTableDeltas2Half_Value8[0x100][0x100 * 0x80];

template<int bits>
static INLINED void ReduceLevels(const uint8_t table[0x100][0x100 * 0x100], uint8_t* p)
{
	constexpr int step = 1 << (8 - bits);

	for (int x = 0; x < 0x100; x++)
	{
		for (int iH = 0; iH < 0x100; iH += step)
		{
			int cH = (iH + (iH >> bits)) << 8;

			for (int iL = 0; iL < 0x100; iL += step)
			{
				int c = cH + iL + (iL >> bits);

				*p++ = table[x][c];
			}
		}
	}
}

template<int bits>
static INLINED void FilterSharedLevels(uint8_t* p)
{
	constexpr int N = 1 << bits;

	for (int x = 0; x < 0x100; x++)
	{
		for (int iH = 0; iH < N; iH++)
		{
			for (int iL = (iH & 1) ^ 1; iL < N; iL += 2)
			{
				p[iL] = 0xFF;
			}

			p += N;
		}
	}
}

template<int bits>
static INLINED void CutLevels(const uint8_t table[0x100][1 << 2 * bits], uint16_t tower[0x100][1 << bits])
{
	constexpr int N = 1 << bits;

	for (int x = 0; x < 0x100; x++)
	{
#if defined(OPTION_SELFCHECK)
		uint16_t e = 0xFFFF;
#endif

		for (int iH = 0; iH < N; iH++)
		{
			int cH = iH << bits;

			uint16_t cut = 0xFFFF;
			for (int iL = 0, hL = iH | 31; iL <= hL; iL++)
			{
				uint16_t value = table[x][cH + iL];

				if (cut > value)
				{
					cut = value;
				}
			}
			cut *= cut;

			tower[x][iH] = cut;

#if defined(OPTION_SELFCHECK)
			if (e > cut)
			{
				e = cut;
			}
#endif
		}

#if defined(OPTION_SELFCHECK)
		if (e > 1)
		{
			__debugbreak();
		}
#endif
	}
}

void InitLevels() noexcept
{
	const __m128i mhalf = _mm_set1_epi16(32);

	// 3-bit index
	{
		const auto gTableDeltas3_Value8 = gTableDeltas2_Value8;

		__m128i mratio = _mm_setzero_si128();
		{
			__m128i m0 = gTableInterpolate3_U8[0];
			__m128i m1 = gTableInterpolate3_U8[1];
			__m128i m2 = gTableInterpolate3_U8[2];
			__m128i m3 = gTableInterpolate3_U8[3];

			mratio = _mm_blend_epi16(mratio, m0, 0x11);
			mratio = _mm_blend_epi16(mratio, m1, 0x22);
			mratio = _mm_blend_epi16(mratio, m2, 0x44);
			mratio = _mm_blend_epi16(mratio, m3, 0x88);
		}

		for (int x = 0; x < 0x100; x++)
		{
			__m128i mx = _mm_shuffle_epi32(_mm_shufflelo_epi16(_mm_cvtsi32_si128(x), 0), 0);

			for (int cH = 0; cH < 0x100; cH++)
			{
				for (int cL = 0; cL < 0x100; cL++)
				{
					int c = (cH << 8) + cL;

					__m128i mc = _mm_shuffle_epi32(_mm_shufflelo_epi16(_mm_cvtsi32_si128(c), 0), 0);

					__m128i mv = _mm_maddubs_epi16(mc, mratio);
					mv = _mm_add_epi16(mv, mhalf);
					mv = _mm_srli_epi16(mv, 6);

					mv = _mm_sub_epi16(mv, mx);
					mv = _mm_abs_epi16(mv);

					if constexpr (!kDenoise)
					{
						mv = _mm_adds_epu8(mv, mv);
					}

					mv = _mm_srli_epi16(mv, kDenoiseShift);

					gTableDeltas3_Value8[x][c] = (uint8_t)_mm_extract_epi16(_mm_minpos_epu16(mv), 0);
				}
			}
		}

		ReduceLevels<7>(gTableDeltas3_Value8, &gTableDeltas3_Value7Shared[0][0]); FilterSharedLevels<7>(&gTableDeltas3_Value7Shared[0][0]);
		ReduceLevels<6>(gTableDeltas3_Value8, &gTableDeltas3_Value6[0][0]);
		ReduceLevels<5>(gTableDeltas3_Value8, &gTableDeltas3_Value5[0][0]);

		CutLevels<7>(gTableDeltas3_Value7Shared, gTableCuts3_Value7Shared);
		CutLevels<5>(gTableDeltas3_Value5, gTableCuts3_Value5);
	}

	// 2-bit index
	{
		__m128i mratio = _mm_setzero_si128();
		{
			__m128i m0 = gTableInterpolate2_U8[0];
			__m128i m1 = gTableInterpolate2_U8[1];

			mratio = _mm_blend_epi16(mratio, m0, 0x11 + 0x44);
			mratio = _mm_blend_epi16(mratio, m1, 0x22 + 0x88);
		}

		for (int x = 0; x < 0x100; x++)
		{
			__m128i mx = _mm_shuffle_epi32(_mm_shufflelo_epi16(_mm_cvtsi32_si128(x), 0), 0);

			for (int cH = 0; cH < 0x100; cH++)
			{
				for (int cL = 0; cL < 0x100; cL++)
				{
					int c = (cH << 8) + cL;

					__m128i mc = _mm_shuffle_epi32(_mm_shufflelo_epi16(_mm_cvtsi32_si128(c), 0), 0);

					__m128i mv = _mm_maddubs_epi16(mc, mratio);
					mv = _mm_add_epi16(mv, mhalf);
					mv = _mm_srli_epi16(mv, 6);

					mv = _mm_sub_epi16(mv, mx);
					mv = _mm_abs_epi16(mv);

					if constexpr (!kDenoise)
					{
						mv = _mm_adds_epu8(mv, mv);
					}

					mv = _mm_srli_epi16(mv, kDenoiseShift);

					gTableDeltas2_Value8[x][c] = (uint8_t)_mm_extract_epi16(_mm_minpos_epu16(mv), 0);
				}
			}
		}

		ReduceLevels<7>(gTableDeltas2_Value8, &gTableDeltas2_Value7[0][0]);
		ReduceLevels<6>(gTableDeltas2_Value8, &gTableDeltas2_Value6[0][0]);
		ReduceLevels<5>(gTableDeltas2_Value8, &gTableDeltas2_Value5[0][0]);

		CutLevels<8>(gTableDeltas2_Value8, gTableCuts2_Value8);
		CutLevels<6>(gTableDeltas2_Value6, gTableCuts2_Value6);
		CutLevels<5>(gTableDeltas2_Value5, gTableCuts2_Value5);
	}

	// 4-bit index
	{
		__m128i mratio0 = _mm_setzero_si128();
		__m128i mratio1 = _mm_setzero_si128();
		{
			__m128i m0 = gTableInterpolate4_U8[0];
			__m128i m1 = gTableInterpolate4_U8[1];
			__m128i m2 = gTableInterpolate4_U8[2];
			__m128i m3 = gTableInterpolate4_U8[3];
			__m128i m4 = gTableInterpolate4_U8[4];
			__m128i m5 = gTableInterpolate4_U8[5];
			__m128i m6 = gTableInterpolate4_U8[6];
			__m128i m7 = gTableInterpolate4_U8[7];

			mratio0 = _mm_blend_epi16(mratio0, m0, 0x11);
			mratio0 = _mm_blend_epi16(mratio0, m1, 0x22);
			mratio0 = _mm_blend_epi16(mratio0, m2, 0x44);
			mratio0 = _mm_blend_epi16(mratio0, m3, 0x88);

			mratio1 = _mm_blend_epi16(mratio1, m4, 0x11);
			mratio1 = _mm_blend_epi16(mratio1, m5, 0x22);
			mratio1 = _mm_blend_epi16(mratio1, m6, 0x44);
			mratio1 = _mm_blend_epi16(mratio1, m7, 0x88);
		}

		for (int x = 0; x < 0x100; x++)
		{
			__m128i mx = _mm_shuffle_epi32(_mm_shufflelo_epi16(_mm_cvtsi32_si128(x), 0), 0);

			for (int cH = 0; cH < 0x100; cH++)
			{
				for (int cL = 0; cL < 0x100; cL++)
				{
					int c = (cH << 8) + cL;

					__m128i mc = _mm_shuffle_epi32(_mm_shufflelo_epi16(_mm_cvtsi32_si128(c), 0), 0);

					__m128i mv0 = _mm_maddubs_epi16(mc, mratio0);
					__m128i mv1 = _mm_maddubs_epi16(mc, mratio1);

					mv0 = _mm_add_epi16(mv0, mhalf);
					mv1 = _mm_add_epi16(mv1, mhalf);

					mv0 = _mm_srli_epi16(mv0, 6);
					mv1 = _mm_srli_epi16(mv1, 6);

					mv0 = _mm_sub_epi16(mv0, mx);
					mv1 = _mm_sub_epi16(mv1, mx);

					mv0 = _mm_abs_epi16(mv0);
					mv1 = _mm_abs_epi16(mv1);

					__m128i mv = _mm_min_epi16(mv0, mv1);

					if constexpr (!kDenoise)
					{
						mv = _mm_adds_epu8(mv, mv);
					}

					mv = _mm_srli_epi16(mv, kDenoiseShift);

					mv = _mm_min_epi16(mv, _mm_set1_epi16(0xF));

					gTableDeltas4Half_Value8[x][c >> 1] |= static_cast<uint8_t>(_mm_extract_epi16(_mm_minpos_epu16(mv), 0) << ((c & 1) << 2));
				}
			}
		}
	}

	// 2-bit subset of 4-bit index
	{
		for (int x = 0; x < 0x100; x++)
		{
			for (int c = 0; c < 0x100 * 0x100; c++)
			{
				uint8_t v = gTableDeltas2_Value8[x][c];

				v = (v > 0xF) ? 0xF : v;

				gTableDeltas2Half_Value8[x][c >> 1] |= static_cast<uint8_t>(v << ((c & 1) << 2));
			}
		}
	}
}


alignas(16) const __m128i gWeightsAGRB = _mm_set_epi16(kBlue, kRed, kGreen, kAlpha, kBlue, kRed, kGreen, kAlpha);
alignas(16) const __m128i gWeightsAGR = _mm_set_epi16(0, kRed, kGreen, kAlpha, 0, kRed, kGreen, kAlpha);
alignas(16) const __m128i gWeightsAGB = _mm_set_epi16(kBlue, 0, kGreen, kAlpha, kBlue, 0, kGreen, kAlpha);
alignas(16) const __m128i gWeightsAG = _mm_set_epi16(0, 0, kGreen, kAlpha, 0, 0, kGreen, kAlpha);
alignas(16) const __m128i gWeightsAR = _mm_set_epi16(0, kRed, 0, kAlpha, 0, kRed, 0, kAlpha);
alignas(16) const __m128i gWeightsAGAG = _mm_set_epi16(kGreen, kAlpha, kGreen, kAlpha, kGreen, kAlpha, kGreen, kAlpha);
alignas(16) const __m128i gWeightsARAR = _mm_set_epi16(kRed, kAlpha, kRed, kAlpha, kRed, kAlpha, kRed, kAlpha);
alignas(16) const __m128i gWeightsGRB = _mm_set_epi16(kBlue, kRed, kGreen, 0, kBlue, kRed, kGreen, 0);
alignas(16) const __m128i gWeightsGRGR = _mm_set_epi16(kRed, kGreen, kRed, kGreen, kRed, kGreen, kRed, kGreen);
alignas(16) const __m128i gWeightsGBGB = _mm_set_epi16(kBlue, kGreen, kBlue, kGreen, kBlue, kGreen, kBlue, kGreen);


alignas(32) const int gRotationsMode4[8] = { 0 + 4, 0, 2 + 4, 2, 1 + 4, 1, 3 + 4, 3 };
alignas(16) const int gRotationsMode5[4] = { 0, 2, 1, 3 };
