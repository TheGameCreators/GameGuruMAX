#pragma once

#include "pch.h"

alignas(32) extern __m128i gTableInterpolate2_U8[4 >> 1];
alignas(64) extern __m128i gTableInterpolate3_U8[8 >> 1];
alignas(32) extern __m128i gTableInterpolate4_U8[16 >> 1];

alignas(32) extern __m128i gTableInterpolate2GR_U8[4 >> 2];
alignas(32) extern __m128i gTableInterpolate3GR_U8[8 >> 2];
alignas(32) extern __m128i gTableInterpolate4GR_U8[16 >> 2];

alignas(32) extern __m128i gTableInterpolate2n3_U8[8 >> 1];
alignas(32) extern __m128i gTableInterpolate3n2_U8[8 >> 1];

void InitInterpolation() noexcept;


alignas(32) extern const uint8_t gTableShrinked22[64];

alignas(32) extern uint8_t gTableShrinkedLow3[64];
alignas(32) extern uint8_t gTableShrinkedHigh3[64];

void InitShrinked() noexcept;


constexpr uint64_t gTableSelection11 = 0xFEDCBA9876543210uLL;

alignas(32) extern uint64_t gTableSelection12[64];
alignas(32) extern uint64_t gTableSelection22[64];

alignas(32) extern uint64_t gTableSelection13[64];
alignas(32) extern uint64_t gTableSelection23[64];
alignas(32) extern uint64_t gTableSelection33[64];

void InitSelection() noexcept;


alignas(64) extern uint8_t gTableDeltas2_Value8[0x100][0x100 * 0x100];
alignas(64) extern uint8_t gTableDeltas2_Value7[0x100][0x80 * 0x80];
alignas(64) extern uint8_t gTableDeltas2_Value6[0x100][0x40 * 0x40];
alignas(64) extern uint8_t gTableDeltas2_Value5[0x100][0x20 * 0x20];

alignas(64) extern uint16_t gTableCuts2_Value8[0x100][0x100];
alignas(64) extern uint16_t gTableCuts2_Value6[0x100][0x40];
alignas(64) extern uint16_t gTableCuts2_Value5[0x100][0x20];

alignas(64) extern uint8_t gTableDeltas3_Value7Shared[0x100][0x80 * 0x80];
alignas(64) extern uint8_t gTableDeltas3_Value6[0x100][0x40 * 0x40];
alignas(64) extern uint8_t gTableDeltas3_Value5[0x100][0x20 * 0x20];

alignas(64) extern uint16_t gTableCuts3_Value7Shared[0x100][0x80];
alignas(64) extern uint16_t gTableCuts3_Value5[0x100][0x20];

alignas(64) extern uint8_t gTableDeltas4Half_Value8[0x100][0x100 * 0x80];
alignas(64) extern uint8_t gTableDeltas2Half_Value8[0x100][0x100 * 0x80];

void InitLevels() noexcept;


alignas(16) extern const __m128i gWeightsAGRB, gWeightsAGR, gWeightsAGB, gWeightsAG, gWeightsAR, gWeightsAGAG, gWeightsARAR;
alignas(16) extern const __m128i gWeightsGRB, gWeightsGRGR, gWeightsGBGB;


alignas(32) extern const int gRotationsMode4[8];
alignas(16) extern const int gRotationsMode5[4];
