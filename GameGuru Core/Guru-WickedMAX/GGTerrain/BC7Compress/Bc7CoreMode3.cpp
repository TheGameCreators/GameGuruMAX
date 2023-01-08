
#include "pch.h"
#include "Bc7Core.h"
#include "Bc7Tables.h"

#include "SnippetDecompressIndexedSubset.h"
#include "SnippetInsertRemoveZeroBit.h"
#include "SnippetComputeOpaqueSubset2.h"
#include "SnippetLevelsMinimum.h"
#include "SnippetLevelsBuffer.h"

// https://docs.microsoft.com/en-us/windows/desktop/direct3d11/bc7-format-mode-reference#mode-3

namespace Mode3 {

	constexpr int LevelsCapacity = 32;

#if defined(OPTION_COUNTERS)
	static std::atomic_int gComputeSubsetError2, gComputeSubsetError2GR, gComputeSubsetError2GB;
#endif

	static ALWAYS_INLINED int Max(int x, int y) noexcept
	{
		return (x > y) ? x : y;
	}

	void DecompressBlock(uint8_t input[16], Cell& output) noexcept
	{
		uint64_t data0 = *(const uint64_t*)&input[0];
		uint64_t data1 = *(const uint64_t*)&input[8];

		data0 >>= 4;

		size_t partitionIndex = data0 & 0x3F; data0 >>= 6;

		__m128i mc0 = _mm_setzero_si128();
		__m128i mc1 = _mm_setzero_si128();

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 4); data0 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 5); data0 >>= 7;
		mc1 = _mm_insert_epi16(mc1, static_cast<int>(data0), 4); data0 >>= 7;
		mc1 = _mm_insert_epi16(mc1, static_cast<int>(data0), 5); data0 >>= 7;

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 2); data0 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data0), 3); data0 >>= 7;
		mc1 = _mm_insert_epi16(mc1, static_cast<int>(data0), 2); data0 >>= 7;
		mc1 = _mm_insert_epi16(mc1, static_cast<int>(data0 + (data1 << 5)), 3); data1 >>= 2;

		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data1), 6); data1 >>= 7;
		mc0 = _mm_insert_epi16(mc0, static_cast<int>(data1), 7); data1 >>= 7;
		mc1 = _mm_insert_epi16(mc1, static_cast<int>(data1), 6); data1 >>= 7;
		mc1 = _mm_insert_epi16(mc1, static_cast<int>(data1), 7); data1 >>= 7;

		const __m128i m7 = _mm_set1_epi16(0x7F);
		mc0 = _mm_and_si128(mc0, m7);
		mc1 = _mm_and_si128(mc1, m7);

		mc0 = _mm_add_epi16(mc0, mc0);
		mc1 = _mm_add_epi16(mc1, mc1);

		int pbits0 = static_cast<int>(data1 & 1) + (static_cast<int>(data1 & 2) << 15); data1 >>= 2;
		int pbits1 = static_cast<int>(data1 & 1) + (static_cast<int>(data1 & 2) << 15); data1 >>= 2;
		mc0 = _mm_or_si128(mc0, _mm_shuffle_epi32(_mm_cvtsi32_si128(pbits0), 0));
		mc1 = _mm_or_si128(mc1, _mm_shuffle_epi32(_mm_cvtsi32_si128(pbits1), 0));

		const __m128i mopaque = _mm_set_epi16(0, 0, 0, 0, 0, 0, 255, 255);
		mc0 = _mm_or_si128(mc0, mopaque);
		mc1 = _mm_or_si128(mc1, mopaque);

		data1 = InsertZeroBit(data1, gTableShrinked22[partitionIndex] * 2);
		data1 = InsertZeroBit(data1, 1);

		DecompressIndexedSubset<2>(mc0, gTableSelection12[partitionIndex], (int*)output.ImageRows_U8, data1);
		DecompressIndexedSubset<2>(mc1, gTableSelection22[partitionIndex], (int*)output.ImageRows_U8, data1);

		output.BestColor0 = mc0;
		output.BestColor1 = mc1;
		output.BestColor2 = _mm_setzero_si128();
		output.BestParameter = partitionIndex;
		output.BestMode = 3;
	}

	static INLINED void ComposeBlock(uint8_t output[16], __m128i mc0, __m128i mc1, uint64_t indices, const size_t partitionIndex) noexcept
	{
		uint64_t data1 = RemoveZeroBit(RemoveZeroBit(indices, 1), gTableShrinked22[partitionIndex] * 2);

		data1 <<= 1; data1 |= _mm_extract_epi16(mc1, 3) & 1;
		data1 <<= 1; data1 |= _mm_extract_epi16(mc1, 2) & 1;
		data1 <<= 1; data1 |= _mm_extract_epi16(mc0, 3) & 1;
		data1 <<= 1; data1 |= _mm_extract_epi16(mc0, 2) & 1;

		mc0 = _mm_srli_epi16(mc0, 1);
		mc1 = _mm_srli_epi16(mc1, 1);

		data1 <<= 7; data1 |= _mm_extract_epi16(mc1, 7);
		data1 <<= 7; data1 |= _mm_extract_epi16(mc1, 6);
		data1 <<= 7; data1 |= _mm_extract_epi16(mc0, 7);
		data1 <<= 7; data1 |= _mm_extract_epi16(mc0, 6);

		data1 <<= 2; data1 |= _mm_extract_epi16(mc1, 3) >> 5;
		uint64_t data0 = _mm_extract_epi16(mc1, 3) & 0x1F;
		data0 <<= 7; data0 |= _mm_extract_epi16(mc1, 2);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 3);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 2);

		data0 <<= 7; data0 |= _mm_extract_epi16(mc1, 5);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc1, 4);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 5);
		data0 <<= 7; data0 |= _mm_extract_epi16(mc0, 4);

		data0 <<= 6; data0 |= partitionIndex;

		data0 <<= 4; data0 |= 1 << 3;

		*(uint64_t*)&output[0] = data0;
		*(uint64_t*)&output[8] = data1;
	}

	void FinalPackBlock(uint8_t output[16], Cell& input) noexcept
	{
		const size_t partitionIndex = input.BestParameter;

		Area& area1 = GetArea(input.Area12[partitionIndex], input.LazyArea12[partitionIndex], input, gTableSelection12[partitionIndex]);
		Area& area2 = GetArea(input.Area22[partitionIndex], input.LazyArea22[partitionIndex], input, gTableSelection22[partitionIndex]);

		__m128i mc0 = input.BestColor0;
		__m128i mc1 = input.BestColor1;

		uint64_t indices = 0;
		input.Error.Alpha = input.OpaqueAlphaError;
		input.Error.Total = ComputeSubsetTable2<true>(area1, mc0, indices);
		input.Error.Total += ComputeSubsetTable2<true>(area2, mc1, indices);

		AreaReduceTable2(area1, mc0, indices);
		AreaReduceTable2(area2, mc1, indices);

		ComposeBlock(output, mc0, mc1, indices, partitionIndex);
	}

	static INLINED int CompressSubsetFast(const Area& area, __m128i& mc, int water) noexcept
	{
		mc = area.Bounds_U16;

		const __m128i mp = _mm_set1_epi16(1);
		__m128i mpbits = _mm_shuffle_epi32(_mm_and_si128(mp, mc), _MM_SHUFFLE(1, 1, 1, 1));

		const __m128i mh7 = _mm_set1_epi16(0xFE);
		mc = _mm_and_si128(mc, mh7);

		mc = _mm_or_si128(mc, mpbits);

#if defined(OPTION_COUNTERS)
		gComputeSubsetError2++;
#endif
		return ComputeSubsetError2(area, mc, gWeightsGRB, _mm_cvtsi32_si128(water));
	}

	void CompressBlockFast(Cell& input) noexcept
	{
		const int denoiseStep = input.DenoiseStep;

		for (size_t partitionIndex = 0; partitionIndex < 64; partitionIndex++)
		{
			__m128i mc0 = _mm_setzero_si128();
			__m128i mc1 = _mm_setzero_si128();

			int error = input.OpaqueAlphaError + denoiseStep;
			if (error < input.Error.Total)
			{
				Area& area1 = GetArea(input.Area12[partitionIndex], input.LazyArea12[partitionIndex], input, gTableSelection12[partitionIndex]);

				error += CompressSubsetFast(area1, mc0, input.Error.Total - error);

				if (error < input.Error.Total)
				{
					Area& area2 = GetArea(input.Area22[partitionIndex], input.LazyArea22[partitionIndex], input, gTableSelection22[partitionIndex]);

					error += CompressSubsetFast(area2, mc1, input.Error.Total - error);

					if (input.Error.Total > error)
					{
						input.Error.Total = error - denoiseStep;

						input.BestColor0 = mc0;
						input.BestColor1 = mc1;
						input.BestParameter = partitionIndex;
						input.BestMode = 3;

						if (error <= input.OpaqueAlphaError + denoiseStep)
							return;
					}
				}
			}
		}
	}

	struct Estimation
	{
		int ch1, ch2, ch3;
	};

	class Subset final
	{
	public:
		LevelsBuffer<LevelsCapacity> ch1, ch2, ch3;

		ALWAYS_INLINED Subset() noexcept = default;

		template<int pbits>
		INLINED bool InitLevels(const Area& area, const int water, const Estimation& estimation) noexcept
		{
			ch1.ComputeChannelLevelsReduced<7, pbits, false, gTableDeltas2_Value8>(area, 1, kGreen, water - estimation.ch2 - estimation.ch3);
			int min1 = ch1.MinErr;
			if (min1 >= water)
				return false;

			ch2.ComputeChannelLevelsReduced<7, pbits, false, gTableDeltas2_Value8>(area, 2, kRed, water - min1 - estimation.ch3);
			int min2 = ch2.MinErr;
			if (min1 + min2 >= water)
				return false;

			ch3.ComputeChannelLevelsReduced<7, pbits, false, gTableDeltas2_Value8>(area, 3, kBlue, water - min1 - min2);
			int min3 = ch3.MinErr;
			if (min1 + min2 + min3 >= water)
				return false;

			return true;
		}

		INLINED int TryVariants(const Area& area, __m128i& best_color, int water) noexcept
		{
			int min1 = ch1.MinErr;
			int min2 = ch2.MinErr;
			int min3 = ch3.MinErr;
			if (min1 + min2 + min3 >= water)
				return water;

			int n1 = ch1.Count;
			int n2 = ch2.Count;
			int n3 = ch3.Count;

			int memGB[LevelsCapacity];

			for (int i1 = 0; i1 < n1; i1++)
			{
				int e1 = ch1.Err[i1].Error;
				if (e1 + min2 + min3 >= water)
					break;

				int c1 = ch1.Err[i1].Color;

				for (int i = 0; i < n3; i++)
				{
					memGB[i] = -1;
				}

				for (int i2 = 0; i2 < n2; i2++)
				{
					int e2 = ch2.Err[i2].Error + e1;
					if (e2 + min3 >= water)
						break;

					int c2 = ch2.Err[i2].Color;

					{
						__m128i mc = _mm_setzero_si128();
						mc = _mm_insert_epi16(mc, c1, 1);
						mc = _mm_insert_epi16(mc, c2, 2);
						mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
						gComputeSubsetError2GR++;
#endif
						e2 = ComputeSubsetError2Pair<_MM_SHUFFLE(2, 1, 2, 1)>(area, mc, gWeightsGRGR, _mm_cvtsi32_si128(water - min3));
						if (e2 + min3 >= water)
							continue;
					}

					for (int i3 = 0; i3 < n3; i3++)
					{
						int e3 = ch3.Err[i3].Error + e2;
						if (e3 >= water)
							break;

						int c3 = ch3.Err[i3].Color;

						int egb = memGB[i3];
						if (egb < 0)
						{
							__m128i mc = _mm_setzero_si128();
							mc = _mm_insert_epi16(mc, c1, 1);
							mc = _mm_insert_epi16(mc, c3, 3);
							mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
							gComputeSubsetError2GB++;
#endif
							egb = ComputeSubsetError2Pair<_MM_SHUFFLE(3, 1, 3, 1)>(area, mc, gWeightsGBGB, _mm_cvtsi32_si128(water - min2));
							memGB[i3] = egb;
						}
						if (egb + min2 >= water)
							continue;

						__m128i mc = _mm_setzero_si128();
						mc = _mm_insert_epi16(mc, c1, 1);
						mc = _mm_insert_epi16(mc, c2, 2);
						mc = _mm_insert_epi16(mc, c3, 3);
						mc = _mm_cvtepu8_epi16(mc);

#if defined(OPTION_COUNTERS)
						gComputeSubsetError2++;
#endif
						int err = ComputeSubsetError2(area, mc, gWeightsGRB, _mm_cvtsi32_si128(water));

						if (water > err)
						{
							water = err;

							best_color = mc;
						}
					}
				}
			}

			return water;
		}
	};

	class Subsets final
	{
	public:
		Subset subset3;
		Subset subset0;
		Subset subset1;
		Subset subset2;

		bool valid3 = false;
		bool valid0 = false;
		bool valid1 = false;
		bool valid2 = false;

		ALWAYS_INLINED Subsets() noexcept = default;

		INLINED bool InitLevels(const Area& area, const int water, const Estimation& estimation) noexcept
		{
			valid3 = subset3.InitLevels<0x0101>(area, water, estimation);
			valid0 = subset0.InitLevels<0>(area, water, estimation);
			valid1 = subset1.InitLevels<1>(area, water, estimation);
			valid2 = subset2.InitLevels<1 << 8>(area, water, estimation);

			return valid3 || valid0 || valid1 || valid2;
		}

		INLINED int TryVariants(const Area& area, __m128i& best_color, int water) noexcept
		{
			if (valid3)
			{
				water = subset3.TryVariants(area, best_color, water);
			}

			if (valid0)
			{
				water = subset0.TryVariants(area, best_color, water);
			}

			if (valid1)
			{
				water = subset1.TryVariants(area, best_color, water);
			}

			if (valid2)
			{
				water = subset2.TryVariants(area, best_color, water);
			}

			return water;
		}
	};

	void CompressBlock(Cell& input) noexcept
	{
		const size_t partitionIndex = input.BestParameter;

		__m128i mc0 = input.BestColor0;
		__m128i mc1 = input.BestColor1;

		const Estimation estimation{ 0, 0, 0 };

		int error = input.OpaqueAlphaError;
		if (error < input.Error.Total)
		{
			Area& area1 = GetArea(input.Area12[partitionIndex], input.LazyArea12[partitionIndex], input, gTableSelection12[partitionIndex]);

#if defined(OPTION_COUNTERS)
			gComputeSubsetError2++;
#endif
			int water1 = ComputeSubsetError2(area1, mc0, gWeightsGRB, _mm_cvtsi32_si128(kBlockMaximalColorError));
			if (water1)
			{
				Subsets subsets1;
				if (subsets1.InitLevels(area1, water1, estimation))
				{
					error += subsets1.TryVariants(area1, mc0, water1);
				}
				else
				{
					error += water1;
				}
			}

			Area& area2 = GetArea(input.Area22[partitionIndex], input.LazyArea22[partitionIndex], input, gTableSelection22[partitionIndex]);

#if defined(OPTION_COUNTERS)
			gComputeSubsetError2++;
#endif
			int water2 = ComputeSubsetError2(area2, mc1, gWeightsGRB, _mm_cvtsi32_si128(kBlockMaximalColorError));
			if (water2)
			{
				Subsets subsets2;
				if (subsets2.InitLevels(area2, water2, estimation))
				{
					error += subsets2.TryVariants(area2, mc1, water2);
				}
				else
				{
					error += water2;
				}
			}

			if (input.Error.Total > error)
			{
				input.Error.Total = error;

				input.BestColor0 = mc0;
				input.BestColor1 = mc1;
				//input.BestParameter = partitionIndex;
				//input.BestMode = 3;
			}
		}
	}

	static INLINED int EstimateLevels(const Area& area, const int water, Estimation& estimation) noexcept
	{
		int error = 0;
		if (error < water)
		{
			int level1 = LevelsMinimum::EstimateChannelLevelsReduced<8, false, gTableDeltas2_Value8, gTableCuts2_Value8>(area, 1, kGreen, water - error);
			estimation.ch1 = level1;
			error += level1;

			if (error < water)
			{
				int level2 = LevelsMinimum::EstimateChannelLevelsReduced<8, false, gTableDeltas2_Value8, gTableCuts2_Value8>(area, 2, kRed, water - error);
				estimation.ch2 = level2;
				error += level2;

				if (error < water)
				{
					int level3 = LevelsMinimum::EstimateChannelLevelsReduced<8, false, gTableDeltas2_Value8, gTableCuts2_Value8>(area, 3, kBlue, water - error);
					estimation.ch3 = level3;
					error += level3;

					if (error < water)
					{
						return error;
					}
				}
			}
		}

		return water;
	}

	void CompressBlockFull(Cell& input) noexcept
	{
		Node order[64]{};
		int lines1[64];
		int lines2[64];

		const int denoiseStep = input.DenoiseStep;

		size_t partitionsCount = 0;
		for (size_t partitionIndex = 0; partitionIndex < 64; partitionIndex++)
		{
			if ((input.PersonalMode == 3) && (input.PersonalParameter == partitionIndex))
				continue;

			Area& area1 = GetArea(input.Area12[partitionIndex], input.LazyArea12[partitionIndex], input, gTableSelection12[partitionIndex]);

			const int water1 = input.Error.Total - denoiseStep - input.OpaqueAlphaError;
			int line1 = AreaGetBestPca3(area1);
			if (line1 < water1)
			{
				Area& area2 = GetArea(input.Area22[partitionIndex], input.LazyArea22[partitionIndex], input, gTableSelection22[partitionIndex]);

				const int water2 = water1 - line1;
				int line2 = AreaGetBestPca3(area2);
				if (line2 < water2)
				{
					lines1[partitionIndex] = line1;
					lines2[partitionIndex] = line2;

					order[partitionsCount++].Init(input.OpaqueAlphaError + line1 + line2 + denoiseStep, static_cast<int>(partitionIndex));
				}
			}
		}

		{
			Node order2[64];
			if (radix_sort(order, order2, partitionsCount) == order2)
			{
				memcpy(order, order2, sizeof(Node) * partitionsCount);
			}
		}

		for (size_t i = 0; i < partitionsCount; i++)
		{
			if (order[i].Error < input.Error.Total)
			{
				const size_t partitionIndex = order[i].Color;

				__m128i mc0 = _mm_setzero_si128();
				__m128i mc1 = _mm_setzero_si128();

				Area& area1 = GetArea(input.Area12[partitionIndex], input.LazyArea12[partitionIndex], input, gTableSelection12[partitionIndex]);
				Area& area2 = GetArea(input.Area22[partitionIndex], input.LazyArea22[partitionIndex], input, gTableSelection22[partitionIndex]);

				int line1 = lines1[partitionIndex];
				int line2 = lines2[partitionIndex];

				Estimation estimations1;
				int water1 = input.Error.Total - denoiseStep - input.OpaqueAlphaError - line2;
				line1 = Max(line1, EstimateLevels(area1, water1, estimations1));
				if (line1 < water1)
				{
					Estimation estimations2;
					int water2 = water1 - line1;
					line2 = Max(line2, EstimateLevels(area2, water2, estimations2));
					if (line2 < water2)
					{
						water1 = input.Error.Total - denoiseStep - input.OpaqueAlphaError - line2;
						Subsets subsets1;
						if (subsets1.InitLevels(area1, water1, estimations1))
						{
							int error = subsets1.TryVariants(area1, mc0, water1);
							if (error < water1)
							{
								error += input.OpaqueAlphaError;

								water2 = input.Error.Total - denoiseStep - error;
								Subsets subsets2;
								if (subsets2.InitLevels(area2, water2, estimations2))
								{
									error += subsets2.TryVariants(area2, mc1, water2);

									if (input.Error.Total > error + denoiseStep)
									{
										input.Error.Total = error;

										input.BestColor0 = mc0;
										input.BestColor1 = mc1;
										input.BestParameter = partitionIndex;
										input.BestMode = 3;

										if (error <= input.OpaqueAlphaError)
											return;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void PrintCounters() noexcept
	{
#if defined(OPTION_COUNTERS)
		PRINTF("[Mode 3]\tGR2 = %i, GB2 = %i, GRB2 = %i",
			gComputeSubsetError2GR.load(), gComputeSubsetError2GB.load(), gComputeSubsetError2.load());
#endif
	}

} // namespace Mode3
