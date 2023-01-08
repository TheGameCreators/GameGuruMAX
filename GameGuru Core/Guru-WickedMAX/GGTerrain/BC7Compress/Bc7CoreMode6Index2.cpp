
#include "pch.h"
#include "Bc7Core.h"
#include "Bc7Tables.h"
#include "Bc7Pca.h"

#include "SnippetInsertRemoveZeroBit.h"
#include "SnippetComputeOpaqueSubset2.h"
#include "SnippetLevelsBufferHalf.h"

// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/bc7-format-mode-reference#mode-6

namespace Mode6Index2 {

	constexpr int LevelsCapacity = 20;

#if defined(OPTION_COUNTERS)
	static std::atomic_int gComputeSubsetError2, gComputeSubsetError2AG, gComputeSubsetError2AR, gComputeSubsetError2GR, gComputeSubsetError2AGR, gComputeSubsetError2GB, gComputeSubsetError2AGB;
#endif

	static INLINED void ComposeBlock(uint8_t output[16], __m128i mc0, uint64_t indices) noexcept
	{
		uint64_t data1 = RemoveZeroBit(indices, 3);

		data1 <<= 1; data1 |= _mm_extract_epi16(mc0, 1) & 1;

		uint64_t data0 = _mm_extract_epi16(mc0, 0) & 1;

		mc0 = _mm_srli_epi16(mc0, 1);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 1);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 0);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 7);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 6);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 3);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 2);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 5);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 4);

		data0 <<= 7; data0 |= 1 << 6;

		*(uint64_t*)&output[0] = data0;
		*(uint64_t*)&output[8] = data1;
	}

	static INLINED int ComputeSubsetTransparentError2(const Area& area, const int alpha) noexcept
	{
		int error = static_cast<int>(area.Count - area.Active);
		if (error)
		{
			error *= kAlpha;
			int v = gTableDeltas2_Value8[0][alpha];
			error *= v * v;
		}

		return error;
	}

	void FinalPackBlock(uint8_t output[16], Cell& input) noexcept
	{
		Area& area = input.Area1;

		__m128i mc = input.BestColor0;

		uint64_t indices2 = 0;
		input.Error = ComputeSubsetTable2<false>(area, mc, indices2);

		uint64_t indices = 0;
		for (size_t i = 0, n = area.Count; i < n; i++)
		{
			uint64_t index = (indices2 >> (i << 1)) & 3u;
			index += index << 2;
			indices |= index << (i << 2);
		}

		AreaReduceTable4(mc, indices);

		ComposeBlock(output, mc, indices);
	}

	static INLINED int CompressSubsetFast(const Area& area, __m128i& mc, int water) noexcept
	{
		mc = area.Bounds_U16;

		const __m128i m1 = _mm_set1_epi16(1);
		__m128i mpbits = _mm_shuffle_epi32(_mm_and_si128(m1, mc), 0);

		const __m128i mh7 = _mm_set1_epi16(0xFE);
		mc = _mm_and_si128(mc, mh7);

		mc = _mm_or_si128(mc, mpbits);

#if defined(OPTION_COUNTERS)
		gComputeSubsetError2++;
#endif

		const int ea = ComputeSubsetTransparentError2(area, _mm_extract_epi16(_mm_packus_epi16(mc, mc), 0));
		if (ea >= water)
			return water;

		return ComputeSubsetError2(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water - ea)) + ea;
	}

	void CompressBlockFast(Cell& input) noexcept
	{
		const int denoiseStep = input.DenoiseStep;

		__m128i mc = _mm_setzero_si128();

		int error = denoiseStep;
		if (error < input.Error.Total)
		{
			Area& area = input.Area1;

			error += CompressSubsetFast(area, mc, input.Error.Total - error);
		}

		if (input.Error.Total > error)
		{
			input.Error.Total = error - denoiseStep;

			input.BestColor0 = mc;
			input.BestMode = 9;
		}
	}

	class Subset final
	{
	public:
		LevelsBufferHalf<LevelsCapacity> ch0, ch1, ch2, ch3;

		ALWAYS_INLINED Subset() noexcept = default;

		template<int pbits>
		INLINED bool InitLevels(const Area& area, const int water) noexcept
		{
			if (area.IsOpaque)
			{
				ch0.SetZeroError(0xFFFF);
			}
			else
			{
				ch0.ComputeChannelLevelsReduced<7, pbits, false, gTableDeltas2Half_Value8>(area, 0, kAlpha, water);
			}
			int min0 = ch0.MinErr;
			if (min0 >= water)
				return false;

			ch1.ComputeChannelLevelsReduced<7, pbits, true, gTableDeltas2Half_Value8>(area, 1, kGreen, water - min0);
			int min1 = ch1.MinErr;
			if (min0 + min1 >= water)
				return false;

			ch2.ComputeChannelLevelsReduced<7, pbits, true, gTableDeltas2Half_Value8>(area, 2, kRed, water - min0 - min1);
			int min2 = ch2.MinErr;
			if (min0 + min1 + min2 >= water)
				return false;

			ch3.ComputeChannelLevelsReduced<7, pbits, true, gTableDeltas2Half_Value8>(area, 3, kBlue, water - min0 - min1 - min2);
			int min3 = ch3.MinErr;
			if (min0 + min1 + min2 + min3 >= water)
				return false;

			return true;
		}

		INLINED int TryVariants(const Area& area, __m128i& best_color, int water) noexcept
		{
			int min0 = ch0.MinErr;
			int min1 = ch1.MinErr;
			int min2 = ch2.MinErr;
			int min3 = ch3.MinErr;
			if (min0 + min1 + min2 + min3 >= water)
				return water;

			int n0 = ch0.Count;
			int n1 = ch1.Count;
			int n2 = ch2.Count;
			int n3 = ch3.Count;

			int memAR[LevelsCapacity];
			int memAGB[LevelsCapacity];

			for (int i0 = 0; i0 < n0; i0++)
			{
				int e0 = ch0.Err[i0].Error;
				if (e0 + min1 + min2 + min3 >= water)
					break;

				int c0 = ch0.Err[i0].Color;

				const int ea = ComputeSubsetTransparentError2(area, c0);

				for (int i = 0; i < n2; i++)
				{
					memAR[i] = -1;
				}

				for (int i1 = 0; i1 < n1; i1++)
				{
					int e1 = ch1.Err[i1].Error + e0;
					if (e1 + min2 + min3 >= water)
						break;

					int c1 = ch1.Err[i1].Color;

					if (!area.IsOpaque)
					{
						__m128i mc = _mm_setzero_si128();
						mc = _mm_insert_epi16(mc, c0, 0);
						mc = _mm_insert_epi16(mc, c1, 1);
						mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
						gComputeSubsetError2AG++;
#endif
						e1 = ComputeSubsetError2Pair<_MM_SHUFFLE(1, 0, 1, 0)>(area, mc, gWeightsAGAG, _mm_cvtsi32_si128(water - ea - min2 - min3)) + ea;
						if (e1 + min2 + min3 >= water)
							continue;
					}

					for (int i = 0; i < n3; i++)
					{
						memAGB[i] = -1;
					}

					for (int i2 = 0; i2 < n2; i2++)
					{
						int e2 = ch2.Err[i2].Error + e1;
						if (e2 + min3 >= water)
							break;

						int c2 = ch2.Err[i2].Color;

						if (!area.IsOpaque)
						{
							int ear = memAR[i2];
							if (ear < 0)
							{
								__m128i mc = _mm_setzero_si128();
								mc = _mm_insert_epi16(mc, c0, 0);
								mc = _mm_insert_epi16(mc, c2, 2);
								mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
								gComputeSubsetError2AR++;
#endif
								ear = ComputeSubsetError2Pair<_MM_SHUFFLE(2, 0, 2, 0)>(area, mc, gWeightsARAR, _mm_cvtsi32_si128(water - ea - min1 - min3)) + ea;
								memAR[i2] = ear;
							}
							if (ear + min1 + min3 >= water)
								continue;
						}

						{
							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, c0, 0);
							mc = _mm_insert_epi16(mc, c1, 1);
							mc = _mm_insert_epi16(mc, c2, 2);
							mc = _mm_cvtepu8_epi16(mc);

							if (area.IsOpaque)
							{
#if defined(OPTION_COUNTERS)
								gComputeSubsetError2GR++;
#endif
								e2 = ComputeSubsetError2Pair<_MM_SHUFFLE(2, 1, 2, 1)>(area, mc, gWeightsGRGR, _mm_cvtsi32_si128(water - ea - min3)) + ea;
							}
							else
							{
#if defined(OPTION_COUNTERS)
								gComputeSubsetError2AGR++;
#endif
								e2 = ComputeSubsetError2(area, mc, gWeightsAGR, _mm_cvtsi32_si128(water - ea - min3)) + ea;
							}
							if (e2 + min3 >= water)
								continue;
						}

						for (int i3 = 0; i3 < n3; i3++)
						{
							int e3 = ch3.Err[i3].Error + e2;
							if (e3 >= water)
								break;

							int c3 = ch3.Err[i3].Color;

							int eagb = memAGB[i3];
							if (eagb < 0)
							{
								__m128i mc = _mm_setzero_si128();
								mc = _mm_insert_epi16(mc, c0, 0);
								mc = _mm_insert_epi16(mc, c1, 1);
								mc = _mm_insert_epi16(mc, c3, 3);
								mc = _mm_cvtepu8_epi16(mc);

								if (area.IsOpaque)
								{
#if defined(OPTION_COUNTERS)
									gComputeSubsetError2GB++;
#endif
									eagb = ComputeSubsetError2Pair<_MM_SHUFFLE(3, 1, 3, 1)>(area, mc, gWeightsGBGB, _mm_cvtsi32_si128(water - ea - min2)) + ea;
								}
								else
								{
#if defined(OPTION_COUNTERS)
									gComputeSubsetError2AGB++;
#endif
									eagb = ComputeSubsetError2(area, mc, gWeightsAGB, _mm_cvtsi32_si128(water - ea - min2)) + ea;
								}
								memAGB[i3] = eagb;
							}
							if (eagb + min2 >= water)
								continue;

							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, c0, 0);
							mc = _mm_insert_epi16(mc, c1, 1);
							mc = _mm_insert_epi16(mc, c2, 2);
							mc = _mm_insert_epi16(mc, c3, 3);
							mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
							gComputeSubsetError2++;
#endif
							int err = ComputeSubsetError2(area, mc, gWeightsAGRB, _mm_cvtsi32_si128(water - ea)) + ea;

							if (water > err)
							{
								water = err;

								best_color = mc;
							}
						}
					}
				}
			}

			return water;
		}
	};

	static INLINED int CompressSubset(const Area& area, __m128i& mc, int water)
	{
		Subset subset3;
		if (subset3.InitLevels<0x0101>(area, water))
		{
			water = subset3.TryVariants(area, mc, water);
		}

		if (!area.IsOpaque)
		{
			Subset subset0;
			if (subset0.InitLevels<0>(area, water))
			{
				water = subset0.TryVariants(area, mc, water);
			}

			Subset subset1;
			if (subset1.InitLevels<1>(area, water))
			{
				water = subset1.TryVariants(area, mc, water);
			}

			Subset subset2;
			if (subset2.InitLevels<1 << 8>(area, water))
			{
				water = subset2.TryVariants(area, mc, water);
			}
		}

		return water;
	}

	void CompressBlock(Cell& input) noexcept
	{
		Area& area = input.Area1;

		__m128i mc = input.BestColor0;

		int error = CompressSubset(area, mc, input.Error.Total);

		if (input.Error.Total > error)
		{
			input.Error.Total = error;

			input.BestColor0 = mc;
			//input.BestMode = 9;
		}
	}

	void PrintCounters() noexcept
	{
#if defined(OPTION_COUNTERS)
		PRINTF("[Mode 6I2]\tAG2 = %i, AR2 = %i, GR2 = %i, AGR2 = %i, GB2 = %i, AGB2 = %i, AGRB2 = %i",
			gComputeSubsetError2AG.load(), gComputeSubsetError2AR.load(),
			gComputeSubsetError2GR.load(), gComputeSubsetError2AGR.load(),
			gComputeSubsetError2GB.load(), gComputeSubsetError2AGB.load(),
			gComputeSubsetError2.load());
#endif
	}

} // namespace Mode6
