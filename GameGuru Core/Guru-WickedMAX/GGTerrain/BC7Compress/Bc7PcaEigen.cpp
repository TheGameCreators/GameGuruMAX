
#include "pch.h"
#include "Bc7Pca.h"

#if defined(OPTION_PCA)

#include "SnippetHorizontalSum4.h"

#define EIGEN_DONT_VECTORIZE
#define EIGEN_FAST_MATH 1
#include "Eigen/SVD" // Eigen 3.3.7 is required

static ALWAYS_INLINED __m128 FMS_ps(__m128 a, __m128 b, __m128 c)
{
#if defined(OPTION_FMA)
	return _mm_fmsub_ps(a, b, c);
#else
	return _mm_sub_ps(_mm_mul_ps(a, b), c);
#endif
}

alignas(16) static float K4[4], InvK4[4];

template<int C>
static INLINED int PrincipalComponentAnalysis(const Area& area)
{
	static_assert((C == 3) || (C == 4));

	const int R = static_cast<int>(area.Active);
	if (R < C)
		return 0;

	typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor | Eigen::AutoAlign, 16, 4> A;

	A matrix(R, C);

	__m128i msum = _mm_setzero_si128();

	for (int j = 0; j < R; j++)
	{
		__m128i mx = _mm_load_si128(&area.DataMask_I16[j]);

		msum = _mm_add_epi16(msum, mx);
	}

	const __m128 mcolor = _mm_load_ps(K4);

	const __m128 mmean = _mm_div_ps(_mm_mul_ps(_mm_cvtepi32_ps(_mm_cvtepu16_epi32(msum)), mcolor), _mm_cvtepi32_ps(_mm_shuffle_epi32(_mm_cvtsi32_si128(R), 0)));

	for (int j = 0; j < R; j++)
	{
		__m128i mx = _mm_load_si128(&area.DataMask_I16[j]);

		__m128 mv = _mm_cvtepi32_ps(_mm_cvtepu16_epi32(mx));

		mv = FMS_ps(mv, mcolor, mmean);

		if constexpr (C == 3)
		{
			float* p = &matrix(j, 0);
			mv = _mm_shuffle_ps(mv, mv, _MM_SHUFFLE(3, 2, 0, 1));
			_mm_store_ss(p + 0, mv);
			_mm_storeh_pi((__m64*)(p + 1), mv);
		}
		else
		{
			_mm_store_ps(&matrix(j, 0), mv);
		}
	}

	Eigen::JacobiSVD<A, Eigen::HouseholderQRPreconditioner> svd(matrix, Eigen::ComputeThinV);
	const auto& V = svd.matrixV();

	__m128 mu;
	if constexpr (C == 3)
	{
		mu = _mm_set_ps(V(2, 0), V(1, 0), V(0, 0), 0.f);
	}
	else
	{
		mu = _mm_set_ps(V(3, 0), V(2, 0), V(1, 0), V(0, 0));
	}

	const __m128 mscale = _mm_load_ps(InvK4);
	const __m128 mshift = _mm_set1_ps(1.1f);
	const __m128 msign = _mm_set1_ps(-0.f);

	msum = _mm_setzero_si128();

	for (int j = 0; j < R; j++)
	{
		__m128 mx;
		if constexpr (C == 3)
		{
			const float* p = &matrix(j, 0);
			mx = _mm_load_ss(p + 0);
			mx = _mm_loadh_pi(mx, (__m64*)(p + 1));
			mx = _mm_shuffle_ps(mx, mx, _MM_SHUFFLE(3, 2, 0, 1));
		}
		else
		{
			mx = _mm_load_ps(&matrix(j, 0));
		}

		__m128 mlambda = _mm_mul_ps(mu, mx);

		mlambda = _mm_add_ps(mlambda, _mm_shuffle_ps(mlambda, mlambda, _MM_SHUFFLE(2, 3, 0, 1)));
		mlambda = _mm_add_ps(mlambda, _mm_shuffle_ps(mlambda, mlambda, _MM_SHUFFLE(0, 1, 2, 3)));

		__m128 me = _mm_mul_ps(_mm_sub_ps(_mm_mul_ps(mlambda, mu), mx), mscale);

		me = _mm_andnot_ps(msign, me);
		me = _mm_sub_ps(me, mshift);
		me = _mm_max_ps(me, _mm_setzero_ps());

		__m128i md = _mm_cvttps_epi32(me);

		if constexpr (!kDenoise)
		{
			md = _mm_adds_epu8(md, md);
		}

		md = _mm_srli_epi16(md, kDenoiseShift);

		md = _mm_mullo_epi16(md, md);

		msum = _mm_add_epi32(msum, md);
	}

	const __m128i mweights = _mm_set_epi32(kBlue, kRed, kGreen, kAlpha * int(C == 4));

	msum = _mm_mullo_epi32(msum, mweights);

	return _mm_cvtsi128_si32(HorizontalSum4(msum));
}

int PrincipalComponentAnalysis3(const Area& area)
{
	return PrincipalComponentAnalysis<3>(area);
}

int PrincipalComponentAnalysis4(const Area& area)
{
	return PrincipalComponentAnalysis<4>(area);
}

void InitPCA() noexcept
{
	__m128 mk = _mm_set_ps(kBlue, kRed, kGreen, kAlpha);

	mk = _mm_sqrt_ps(mk);

	_mm_store_ps(K4, mk);

	__m128 mik = _mm_div_ps(_mm_set1_ps(1.0f), mk);

	_mm_store_ps(InvK4, mik);
}

#endif
