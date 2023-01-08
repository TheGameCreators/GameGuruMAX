#pragma once

#if !defined(OPTION_LIBRARY)

#include "Bc7Mode.h"

#if defined(_WIN32)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdint.h>
#include <string.h>
#include <math.h>

#if defined(OPTION_COUNTERS)
#include <stdio.h>
#include <atomic>
#endif

#if defined(OPTION_AVX512)
#include <immintrin.h> // AVX512
#elif defined(OPTION_AVX2)
#include <immintrin.h> // AVX2
#elif !defined(OPTION_SLOWPOKE)
#include <smmintrin.h> // SSE4.1
#endif

#if defined(_WIN32)
#include <stdlib.h>
#endif

#if defined(_WIN32)
#define ALWAYS_INLINED __forceinline
#define NOTINLINED __declspec(noinline)
#else
#define ALWAYS_INLINED __attribute__((always_inline))
#define NOTINLINED __attribute__((noinline))
#endif
#define INLINED ALWAYS_INLINED

#define PRINTF(...) printf(__VA_ARGS__); printf("\n");

#endif // !OPTION_LIBRARY

#if defined(OPTION_SLOWPOKE)
#include "SnippetTargetSSSE3.h"
#endif
