#pragma once

#include "pch.h"
#include "Bc7Core.h"

void ProcessTexture(uint8_t* dst, uint8_t* src_bgra, uint8_t* mask_agrb, int stride, int src_w, int src_h, PBlockKernel blockKernel, size_t block_size, int64_t& pErrorAlpha, int64_t& pErrorColor, BlockSSIM& pssim);

bool DetectGlitches(const Cell& input, const Cell& output) noexcept;

void ShowBadBlocks(const uint8_t* src_bgra, const uint8_t* dst_bgra, uint8_t* mask_agrb, int stride, int src_w, int src_h) noexcept;
