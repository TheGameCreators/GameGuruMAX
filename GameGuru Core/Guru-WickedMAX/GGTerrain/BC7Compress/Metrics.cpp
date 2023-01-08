
#include "pch.h"
#include "Metrics.h"
#include "SnippetHorizontalSum4.h"

BlockError CompareBlocks(const Cell& cell1, const Cell& cell2) noexcept
{
#if defined(OPTION_AVX2)
	__m256i vsum = _mm256_setzero_si256();

	const __m256i vnear = _mm256_set_epi8(
		15, 14, 7, 6,
		13, 12, 5, 4,
		11, 10, 3, 2,
		9, 8, 1, 0,

		15, 14, 7, 6,
		13, 12, 5, 4,
		11, 10, 3, 2,
		9, 8, 1, 0);

	for (size_t y = 0; y < 4; y++)
	{
		__m256i vc1 = _mm256_cvtepu8_epi16(_mm_load_si128(&cell1.ImageRows_U8[y]));
		__m256i vc2 = _mm256_cvtepu8_epi16(_mm_load_si128(&cell2.ImageRows_U8[y]));
		__m256i vd = _mm256_sub_epi16(vc1, vc2);

		vd = _mm256_abs_epi16(vd);

		if constexpr (!kDenoise)
		{
			vd = _mm256_adds_epu8(vd, vd);
		}

		vd = _mm256_srli_epi16(vd, kDenoiseShift);

		__m256i vmask = _mm256_cvtepi8_epi16(_mm_load_si128(&cell1.MaskRows_S8[y]));
		vd = _mm256_and_si256(vd, vmask);

		vd = _mm256_shuffle_epi8(vd, vnear);
		vd = _mm256_madd_epi16(vd, vd);

		vsum = _mm256_add_epi32(vsum, vd);
	}

	__m128i msum = _mm_add_epi32(_mm256_castsi256_si128(vsum), _mm256_extracti128_si256(vsum, 1));
#else
	__m128i msum = _mm_setzero_si128();

	const __m128i mnear = _mm_set_epi8(
		15, 14, 7, 6,
		13, 12, 5, 4,
		11, 10, 3, 2,
		9, 8, 1, 0);

	const __m128i mzero = _mm_setzero_si128();

	for (size_t y = 0; y < 4; y++)
	{
		__m128i mc1 = _mm_load_si128(&cell1.ImageRows_U8[y]);
		__m128i mc2 = _mm_load_si128(&cell2.ImageRows_U8[y]);

		__m128i mc1L = _mm_unpacklo_epi8(mc1, mzero);
		__m128i mc2L = _mm_unpacklo_epi8(mc2, mzero);
		__m128i mc1H = _mm_unpackhi_epi8(mc1, mzero);
		__m128i mc2H = _mm_unpackhi_epi8(mc2, mzero);

		__m128i mdL = _mm_sub_epi16(mc1L, mc2L);
		__m128i mdH = _mm_sub_epi16(mc1H, mc2H);

		mdL = _mm_abs_epi16(mdL);
		mdH = _mm_abs_epi16(mdH);

		if constexpr (!kDenoise)
		{
			mdL = _mm_adds_epu8(mdL, mdL);
			mdH = _mm_adds_epu8(mdH, mdH);
		}

		mdL = _mm_srli_epi16(mdL, kDenoiseShift);
		mdH = _mm_srli_epi16(mdH, kDenoiseShift);

		__m128i mmask = _mm_load_si128(&cell1.MaskRows_S8[y]);

		__m128i mmaskL = _mm_unpacklo_epi8(mmask, mmask);
		__m128i mmaskH = _mm_unpackhi_epi8(mmask, mmask);

		mdL = _mm_and_si128(mdL, mmaskL);
		mdH = _mm_and_si128(mdH, mmaskH);

		mdL = _mm_shuffle_epi8(mdL, mnear);
		mdH = _mm_shuffle_epi8(mdH, mnear);

		mdL = _mm_madd_epi16(mdL, mdL);
		mdH = _mm_madd_epi16(mdH, mdH);

		msum = _mm_add_epi32(msum, _mm_add_epi32(mdL, mdH));
	}
#endif

	const __m128i mweights = _mm_set_epi32(kBlue, kRed, kGreen, kAlpha);
	msum = _mm_mullo_epi32(msum, mweights);

	const int alpha = _mm_cvtsi128_si32(msum);

	msum = HorizontalSum4(msum);

	const int total = _mm_cvtsi128_si32(msum);

	return BlockError(alpha, total);
}

BlockSSIM CompareBlocksSSIM(const Cell& cell1, const Cell& cell2) noexcept
{
	SSIM_INIT();

	for (size_t y = 0; y < 4; y++)
	{
		__m128i mc1 = _mm_load_si128(&cell1.ImageRows_U8[y]);
		__m128i mc2 = _mm_load_si128(&cell2.ImageRows_U8[y]);

		__m128i mmask = _mm_load_si128(&cell1.MaskRows_S8[y]);
		mc1 = _mm_and_si128(mc1, mmask);
		mc2 = _mm_and_si128(mc2, mmask);

		for (size_t x = 0; x < 4; x++)
		{
			__m128i mt = _mm_cvtepu8_epi32(mc2);

			__m128i mb = _mm_cvtepu8_epi32(mc1);

			SSIM_UPDATE(mt, mb);

			mc1 = _mm_shuffle_epi32(mc1, _MM_SHUFFLE(0, 3, 2, 1));
			mc2 = _mm_shuffle_epi32(mc2, _MM_SHUFFLE(0, 3, 2, 1));
		}
	}

	SSIM_CLOSE(4);

	SSIM_FINAL(mssim_ga, gSsim16k1L, gSsim16k2L);
	SSIM_OTHER();
	SSIM_FINAL(mssim_br, gSsim16k1L, gSsim16k2L);

	double ssim =
		_mm_cvtsd_f64(_mm_unpackhi_pd(mssim_ga, mssim_ga)) * kGreen +
		_mm_cvtsd_f64(mssim_br) * kRed +
		_mm_cvtsd_f64(_mm_unpackhi_pd(mssim_br, mssim_br)) * kBlue;

	return BlockSSIM(_mm_cvtsd_f64(mssim_ga), ssim * (1.0 / kColor));
}
