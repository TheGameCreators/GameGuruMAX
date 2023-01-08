#pragma once

#include "pch.h"

#if !defined(OPTION_LIBRARY) && defined(_WIN32)

bool ReadImage(const char* src_name, uint8_t* &pixels, int &width, int &height, bool flip);
void WriteImage(const char* dst_name, const uint8_t* pixels, int w, int h, bool flip);

void LoadBc7(const char* name, int position, uint8_t* buffer, int size);
void SaveBc7(const char* name, const uint8_t* head, int position, const uint8_t* buffer, int size);

#endif
