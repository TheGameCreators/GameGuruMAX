#pragma once

#include "pch.h"
#include "Bc7Core.h"

constexpr double gSsim16k1L = (0.01 * 255 * 16) * (0.01 * 255 * 16);
constexpr double gSsim16k2L = gSsim16k1L * 9;

#define SSIM_INIT() \
	__m128i sa = _mm_setzero_si128(); \
	__m128i sb = _mm_setzero_si128(); \
	__m128i sab = _mm_setzero_si128(); \
	__m128i saa_sbb = _mm_setzero_si128(); \

#define SSIM_UPDATE(a, b) \
	sa = _mm_add_epi32(sa, a); \
	sb = _mm_add_epi32(sb, b); \
	sab = _mm_add_epi32(sab, _mm_mullo_epi16(a, b)); \
	saa_sbb = _mm_add_epi32(saa_sbb, _mm_add_epi32(_mm_mullo_epi16(a, a), _mm_mullo_epi16(b, b))); \

#define SSIM_CLOSE(shift) \
	sab = _mm_slli_epi32(sab, shift + 1); \
	saa_sbb = _mm_slli_epi32(saa_sbb, shift); \
	__m128i sasb = _mm_mullo_epi32(sa, sb); \
	sasb = _mm_add_epi32(sasb, sasb); \
	__m128i sasa_sbsb = _mm_add_epi32(_mm_mullo_epi32(sa, sa), _mm_mullo_epi32(sb, sb)); \
	sab = _mm_sub_epi32(sab, sasb); \
	saa_sbb = _mm_sub_epi32(saa_sbb, sasa_sbsb); \

#define SSIM_OTHER() \
	sab = _mm_unpackhi_epi64(sab, sab); \
	saa_sbb = _mm_unpackhi_epi64(saa_sbb, saa_sbb); \
	sasb = _mm_unpackhi_epi64(sasb, sasb); \
	sasa_sbsb = _mm_unpackhi_epi64(sasa_sbsb, sasa_sbsb); \

#define SSIM_FINAL(dst, p1, p2) \
	__m128d dst; \
	{ \
		__m128d mp1 = _mm_load_sd(&p1); \
		__m128d mp2 = _mm_load_sd(&p2); \
		mp1 = _mm_shuffle_pd(mp1, mp1, 0); \
		mp2 = _mm_shuffle_pd(mp2, mp2, 0); \
		dst = _mm_div_pd( \
			_mm_mul_pd(_mm_add_pd(_mm_cvtepi32_pd(sasb), mp1), _mm_add_pd(_mm_cvtepi32_pd(sab), mp2)), \
			_mm_mul_pd(_mm_add_pd(_mm_cvtepi32_pd(sasa_sbsb), mp1), _mm_add_pd(_mm_cvtepi32_pd(saa_sbb), mp2))); \
	} \

BlockError CompareBlocks(const Cell& cell1, const Cell& cell2) noexcept;

BlockSSIM CompareBlocksSSIM(const Cell& cell1, const Cell& cell2) noexcept;
