// Bc7Compress.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Bc7Core.h"
#include "Bc7Tables.h"
#include "Bc7Pca.h"
#include "IO.h"
#include "Worker.h"

#include <chrono>
#include <string>
#include <vector>

#include "DirectXTex.h"
using namespace DirectX;

//#define STB_IMAGE_IMPLEMENTATION
#include "Utility/stb_image.h"

static ALWAYS_INLINED int Max(int x, int y) noexcept
{
	return (x > y) ? x : y;
}

static INLINED void FullMask(uint8_t* mask_agrb, int stride, int src_w, int src_h) noexcept
{
	for (int y = 0; y < src_h; y++)
	{
		int* w = (int*)&mask_agrb[y * stride];

		for (int x = 0; x < src_w; x++)
		{
			w[x] = -1;
		}
	}
}

static INLINED void ComputeAlphaMaskWithOutline(uint8_t* mask_agrb, const uint8_t* src_bgra, int stride, int src_w, int src_h, int radius)
{
	int full_w = 1 + radius + src_w + radius;
	int full_h = 1 + radius + src_h + radius;

	uint8_t* data = new uint8_t[full_h * full_w];
	memset(data, 0, full_h * full_w);

	for (int y = 0; y < src_h; y++)
	{
		const uint8_t* r = &src_bgra[y * stride + 3];
		uint8_t* w = &data[(y + radius + 1) * full_w + (radius + 1)];

		for (int x = 0; x < src_w; x++)
		{
			w[x] = uint8_t(r[x * 4] != 0);
		}
	}

	int* sum = new int[full_h * full_w];
	memset(sum, 0, full_h * full_w * sizeof(int));

	int from_py = (radius + 1) * full_w;
	int to_py = full_h * full_w;
	for (int py = from_py; py < to_py; py += full_w)
	{
		int prev_py = py - full_w;

		for (int x = radius + 1; x < full_w; x++)
		{
			sum[py + x] = sum[prev_py + x] - sum[prev_py + x - 1] + data[py + x] + sum[py + x - 1];
		}
	}

	int a = radius + radius + 1;
	for (int y = 0; y < src_h; y++)
	{
		int* w = (int*)&mask_agrb[y * stride];

		const int* rL = &sum[y * full_w];
		const int* rH = &sum[(y + a) * full_w];

		for (int x = 0; x < src_w; x++, rL++, rH++)
		{
			int v = rH[a] - *rH + *rL - rL[a];

			w[x] = (v != 0) ? -1 : 0xFF;
		}
	}

	delete[] sum, sum = nullptr;

	delete[] data, data = nullptr;
}

static void PackTexture(const IBc7Core& bc7Core, uint8_t* dst_bc7, uint8_t* src_bgra, uint8_t* mask_agrb, int stride, int src_w, int src_h, PBlockKernel blockKernel, size_t block_size, int64_t& pErrorAlpha, int64_t& pErrorColor, BlockSSIM& pssim)
{
	auto start = std::chrono::high_resolution_clock::now();

	ProcessTexture(dst_bc7, src_bgra, mask_agrb, stride, src_w, src_h, blockKernel, block_size, pErrorAlpha, pErrorColor, pssim);

	auto finish = std::chrono::high_resolution_clock::now();

	if (blockKernel == bc7Core.pCompress)
	{
		int pixels = src_h * src_w;

		int span = Max((int)std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count(), 1);

		int kpx_s = pixels / span;

		PRINTF("    Compressed %d blocks, elapsed %i ms, throughput %d.%03d Mpx/s", pixels >> 4, span, kpx_s / 1000, kpx_s % 1000);

#if defined(OPTION_COUNTERS)
		CompressStatistics();
#endif
	}
}

static INLINED void VisualizePartitionsGRB(uint8_t* dst_bc7, int size)
{
	for (int i = 0; i < size; i += 16)
	{
		uint64_t data0 = *(const uint64_t*)&dst_bc7[i + 0];
		uint64_t data1 = *(const uint64_t*)&dst_bc7[i + 8];

		if (data0 & 0xF)
		{
			if (data0 & 3)
			{
				if (data0 & 1)
				{
					data0 &= (1u << (4 + 1)) - 1u;

					data0 |=
						(0xFuLL << 29) + // G0
						(0xFuLL << 13); // R2

					data1 = 
						(0xFuLL << 5); // B4
				}
				else
				{
					data0 &= (1u << (6 + 2)) - 1u;

					data0 |=
						(0x3FuLL << 32) + // G0
						(0x3FuLL << 20); // R2

					data1 = 0;
				}
			}
			else
			{
				if (data0 & 4)
				{
					data0 &= (1u << (6 + 3)) - 1u;

					data0 |=
						(0x1FuLL << 39) + // G0
						(0x1FuLL << 19); // R2

					data1 =
						(0x1FuLL << 25); // B4
				}
				else
				{
					data0 &= (1u << (6 + 4)) - 1u;

					data0 |=
						(0x7FuLL << 38) + // G0
						(0x7FuLL << 24); // R2

					data1 = 0;
				}
			}
		}
		else if (data0 & 0xF0)
		{
			if (data0 & 0x30)
			{
				if (data0 & 0x10)
				{
					data0 &= (1u << 5) - 1u;

					data0 |=
						(0x1FuLL << 18) + // G0
						(0x1FuLL << 8) + // R0
						(0x3FuLL << 38); // A0

					data1 = 0;
				}
				else
				{
					data0 &= (1u << 6) - 1u;

					data0 |=
						(0x7FuLL << 22) + // G0
						(0x7FuLL << 8) + // R0
						(0xFFuLL << 50); // A0

					data1 = 0;
				}
			}
			else
			{
				if (data0 & 0x40)
				{
					if ((data0 == 0x40) && (data1 == 0))
						continue;

					data0 &= (1u << 7) - 1u;

					// data1 = InsertZeroBit(data1 >> 1, 3);
					data1 = (data1 & ~0xFuLL) + ((data1 & 0xFuLL) >> 1);

					if (((data1 >> 2) ^ data1) & 0x3333333333333333uLL)
					{
						data0 |=
							(0x50uLL << 21) + // G0
							(0x50uLL << 7) + // R0
							(0x50uLL << 35) + // B0
							(0x7FuLL << 49); // A0
					}
					else
					{
						data0 |=
							(0x30uLL << 21) + // G0
							(0x30uLL << 7) + // R0
							(0x30uLL << 35) + // B0
							(0x7FuLL << 49); // A0
					}

					data1 = 0;
				}
				else
				{
					data0 &= (1u << (6 + 8)) - 1u;

					data0 |=
						(0x1FuLL << 34) + // G0
						(0x1FuLL << 24); // R2

					data1 =
						(0x1FuLL << 10) + // A0
						(0x1FuLL << 20); // A2
				}
			}
		}

		*(uint64_t*)&dst_bc7[i + 0] = data0;
		*(uint64_t*)&dst_bc7[i + 8] = data1;
	}
}

int Bc7MainWithArgs(const IBc7Core& bc7Core, const std::vector<std::string>& args)
{
	bool doDraft = true;
	bool doNormal = true;
	bool doSlow = false;

	bool flip = true;
	bool mask = true;
	int border = 1;

	const char* src_name = nullptr;
	const char* dst_name = nullptr;
	const char* result_name = nullptr;
	const char* partitions_name = nullptr;
	const char* bad_name = nullptr;

	for (int i = 0, n = (int)args.size(); i < n; i++)
	{
		const char* arg = args[i].c_str();

		if (arg[0] == '/')
		{
			if (strcmp(arg, "/compare") == 0)
			{
				doDraft = false;
				doNormal = false;
				doSlow = false;
				continue;
			}
			else if (strcmp(arg, "/draft") == 0)
			{
				doDraft = true;
				doNormal = false;
				doSlow = false;
				continue;
			}
			else if (strcmp(arg, "/normal") == 0)
			{
				doDraft = true;
				doNormal = true;
				doSlow = false;
				continue;
			}
			else if (strcmp(arg, "/slow") == 0)
			{
				doDraft = true;
				doNormal = true;
				doSlow = true;
				continue;
			}
			else if (strcmp(arg, "/noflip") == 0)
			{
				flip = false;
				continue;
			}
			else if (strcmp(arg, "/nomask") == 0)
			{
				mask = false;
				continue;
			}
			else if (strcmp(arg, "/retina") == 0)
			{
				border = 2;
				continue;
			}
			else if (strcmp(arg, "/debug") == 0)
			{
				if (++i < n)
				{
					result_name = args[i].c_str();
				}
				continue;
			}
			else if (strcmp(arg, "/map") == 0)
			{
				if (++i < n)
				{
					partitions_name = args[i].c_str();
				}
				continue;
			}
			else if (strcmp(arg, "/bad") == 0)
			{
				if (++i < n)
				{
					bad_name = args[i].c_str();
				}
				continue;
			}
#if defined(WIN32)
			else
			{
				PRINTF("Unknown %s", arg);
				continue;
			}
#endif
		}

		if (src_name == nullptr)
		{
			src_name = arg;
		}
		else if (dst_name == nullptr)
		{
			dst_name = arg;
		}
		else
		{
			PRINTF("Error: %s", arg);
			return 1;
		}
	}

	if (!src_name)
	{
		PRINTF("No input");
		return 1;
	}

	uint8_t* src_image_bgra;
	int src_image_w, src_image_h;

	if (!ReadImage(src_name, src_image_bgra, src_image_w, src_image_h, flip))
	{
		PRINTF("Problem with image %s", src_name);
		return 1;
	}

	PRINTF("Loaded %s", src_name);

	int src_texture_w = (Max(4, src_image_w) + 3) & ~3;
	int src_texture_h = (Max(4, src_image_h) + 3) & ~3;

	if (Max(src_texture_w, src_texture_h) > 16384)
	{
		PRINTF("Huge image %s", src_name);
		return 1;
	}

	int c = 4;
	int src_image_stride = src_image_w * c;
	int src_texture_stride = src_texture_w * c;

	uint8_t* src_texture_bgra = new uint8_t[src_texture_h * src_texture_stride];

	for (int i = 0; i < src_image_h; i++)
	{
		memcpy(&src_texture_bgra[i * src_texture_stride], &src_image_bgra[i * src_image_stride], src_image_stride);

		for (int j = src_image_stride; j < src_texture_stride; j += c)
		{
			memcpy(&src_texture_bgra[i * src_texture_stride + j], &src_image_bgra[i * src_image_stride + src_image_stride - c], c);
		}
	}

	for (int i = src_image_h; i < src_texture_h; i++)
	{
		memcpy(&src_texture_bgra[i * src_texture_stride], &src_texture_bgra[(src_image_h - 1) * src_texture_stride], src_texture_stride);
	}

	PRINTF("  Image %dx%d, Texture %dx%d", src_image_w, src_image_h, src_texture_w, src_texture_h);

	delete[] src_image_bgra, src_image_bgra = nullptr;

	uint8_t* dst_texture_bgra = new uint8_t[src_texture_h * src_texture_stride];

	int Size = src_texture_h * src_texture_w;

	// https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
	uint32_t head[16 + 7 + 1];
	head[0] = 0x58544BABu; // identifier
	head[1] = 0xBB313120u; // identifier
	head[2] = 0x0A1A0A0Du; // identifier
	head[3] = 0x04030201u; // endianness
	head[4] = 0; // glType
	head[5] = 1; // glTypeSize
	head[6] = 0; // glFormat
	head[7] = 0x8E8C; // glInternalFormat = COMPRESSED_RGBA_BPTC_UNORM_ARB
	head[8] = 0x1908; // glBaseInternalFormat = GL_RGBA
	head[9] = static_cast<uint32_t>(src_image_w); // pixelWidth
	head[10] = static_cast<uint32_t>(src_image_h); // pixelHeight
	head[11] = 0; // pixelDepth
	head[12] = 0; // numberOfArrayElements
	head[13] = 1; // numberOfFaces
	head[14] = 1; // numberOfMipmapLevels
	head[15] = 0x1C; // bytesOfKeyValueData
	head[16] = 0x17; // keyAndValueByteSize
	head[17] = 0x4F58544Bu; // "KTXOrientation"
	head[18] = 0x6E656972u;
	head[19] = 0x69746174u;
	head[20] = 0x53006E6Fu; // "S=r,T=u"
	head[21] = 0x542C723Du;
	head[22] = flip ? 0x00753Du : 0x00643Du;
	head[23] = static_cast<uint32_t>(Size); // imageSize

	bc7Core.pInitTables(doDraft, doNormal, doSlow);

	memcpy(dst_texture_bgra, src_texture_bgra, src_texture_h * src_texture_stride);

	if ((dst_name != nullptr) && dst_name[0])
	{
		uint8_t* mask_agrb = new uint8_t[src_texture_h * src_texture_stride];

		if (mask)
		{
			ComputeAlphaMaskWithOutline(mask_agrb, src_texture_bgra, src_texture_stride, src_texture_w, src_texture_h, border);
		}
		else
		{
			FullMask(mask_agrb, src_texture_stride, src_texture_w, src_texture_h);
		}

		uint8_t* dst_bc7 = new uint8_t[Size];
		memset(dst_bc7, 0, Size);

		LoadBc7(dst_name, sizeof(head), dst_bc7, Size);

		int64_t mse_alpha = 0;
		int64_t mse_color = 0;
		BlockSSIM ssim = BlockSSIM(0, 0);
		PackTexture(bc7Core, dst_bc7, src_texture_bgra, mask_agrb, src_texture_stride, src_texture_w, src_texture_h, bc7Core.pCompress, 16, mse_alpha, mse_color, ssim);

		int pixels = src_texture_h * src_texture_w;

		if (mse_alpha > 0)
		{
			PRINTF("      SubTexture A qMSE = %.1f, qPSNR = %f, SSIM_4x4 = %.8f",
				(1.0 / kAlpha) * mse_alpha / pixels,
				10.0 * log((255.0 * 255.0) * kAlpha * pixels / mse_alpha) / log(10.0),
				ssim.Alpha * 16.0 / pixels);
		}
		else
		{
			PRINTF("      Whole A");
		}

		if (mse_color > 0)
		{
#if defined(OPTION_LINEAR)
			PRINTF("      SubTexture RGB qMSE = %.1f, qPSNR = %f, SSIM_4x4 = %.8f",
				(1.0 / kColor) * mse_color / pixels,
				10.0 * log((255.0 * 255.0) * kColor * pixels / mse_color) / log(10.0),
				ssim.Color * 16.0 / pixels);
#else
			PRINTF("      SubTexture RGB qMSE = %.1f, qPSNR = %f, wSSIM_4x4 = %.8f",
				(1.0 / kColor) * mse_color / pixels,
				10.0 * log((255.0 * 255.0) * kColor * pixels / mse_color) / log(10.0),
				ssim.Color * 16.0 / pixels);
#endif
		}
		else
		{
			PRINTF("      Whole RGB");
		}

		//SaveBc7(dst_name, (const uint8_t*)head, sizeof(head), dst_bc7, Size);

		DirectX::Image d3dImage;
		d3dImage.format = DXGI_FORMAT_BC7_UNORM;
		d3dImage.height = src_texture_h;
		d3dImage.width = src_texture_w;
		d3dImage.rowPitch = src_texture_w * 4;
		d3dImage.slicePitch = 0;
		d3dImage.pixels = dst_bc7;

		wchar_t wDstFilename[ MAX_PATH ];
		MultiByteToWideChar( CP_ACP, 0, dst_name, -1, wDstFilename, sizeof(wDstFilename) );
		HRESULT hr = DirectX::SaveToDDSFile( d3dImage, 0, wDstFilename );
		PRINTF( "HR: %d", hr );

		PackTexture(bc7Core, dst_bc7, dst_texture_bgra, mask_agrb, src_texture_stride, src_texture_w, src_texture_h, bc7Core.pDecompress, 16, mse_alpha, mse_color, ssim);

		if ((bad_name != nullptr) && bad_name[0])
		{
			ShowBadBlocks(src_texture_bgra, dst_texture_bgra, mask_agrb, src_texture_stride, src_texture_w, src_texture_h);

			WriteImage(bad_name, mask_agrb, src_texture_w, src_texture_h, flip);
		}

		if ((partitions_name != nullptr) && partitions_name[0])
		{
			VisualizePartitionsGRB(dst_bc7, Size);

			PackTexture(bc7Core, dst_bc7, mask_agrb, dst_texture_bgra, src_texture_stride, src_texture_w, src_texture_h, bc7Core.pDecompress, 16, mse_alpha, mse_color, ssim);

			WriteImage(partitions_name, mask_agrb, src_texture_w, src_texture_h, flip);
		}

		delete[] dst_bc7;
		delete[] mask_agrb;
	}

	if ((result_name != nullptr) && result_name[0])
	{
		WriteImage(result_name, dst_texture_bgra, src_texture_w, src_texture_h, flip);
	}

	delete[] dst_texture_bgra;
	delete[] src_texture_bgra;

	return 0;
}

int Bc7GGMax(const IBc7Core& bc7Core, const char* src_name, const char* dst_name )
{
	bool doDraft = true;
	bool doNormal = true;
	bool doSlow = false;

	bool flip = false;

	char src_name_copy[ MAX_PATH ];
	strcpy_s( src_name_copy, src_name );

	uint8_t* src_texture_bgra;
	int src_texture_w, src_texture_h;

	if (!ReadImage(src_name_copy, src_texture_bgra, src_texture_w, src_texture_h, flip))
	{
		char* szExt = strrchr( src_name_copy, '.' );
		strcpy_s( szExt, 5, ".jpg" );
		if (!ReadImage(src_name_copy, src_texture_bgra, src_texture_w, src_texture_h, flip))
		{
			PRINTF("Failed to read image %s", src_name);
			return 0;
		}
	}

	PRINTF("Loaded %s", src_name_copy);

	if (Max(src_texture_w, src_texture_h) > 16384)
	{
		PRINTF("Image too large %s", src_name_copy);
		return 1;
	}

	int c = 4;
	int src_texture_stride = src_texture_w * c;

	PRINTF("  Image %dx%d", src_texture_w, src_texture_h);

	uint32_t mipLevels = 1;
	int testWidth = 4; // lowest mip level must be 4x4 to match compressed block size
	while( testWidth < src_texture_w )
	{
		testWidth <<= 1;
		mipLevels++;
	}

	// create final image
	DirectX::ScratchImage mipChain;
	HRESULT hr = mipChain.Initialize2D( DXGI_FORMAT_BC7_UNORM, src_texture_w, src_texture_h, 1, mipLevels, DirectX::DDS_FLAGS_NONE );
	
	bc7Core.pInitTables(doDraft, doNormal, doSlow);

	uint8_t* mask_agrb = new uint8_t[src_texture_h * src_texture_stride];
	FullMask(mask_agrb, src_texture_stride, src_texture_w, src_texture_h);
	
	int Size = src_texture_h * src_texture_w;
	uint8_t* dst_bc7 = new uint8_t[Size];
	
	int64_t mse_alpha_final = 0;
	int64_t mse_color_final = 0;
	BlockSSIM ssim_final = BlockSSIM(0, 0);
		
	for( uint32_t m = 0; m < mipLevels; m++ )
	{
		uint32_t width = src_texture_w >> m;
		uint32_t height = src_texture_h >> m;
		uint32_t stride = src_texture_stride >> m;
		uint32_t finalSize = Size >> (m*2);
				
		if ( m > 0 )
		{
			uint32_t prevWidth = width * 2;
			uint32_t prevStride = stride * 2;
			for( uint32_t y = 0; y < height; y++ )
			{
				for( uint32_t x = 0; x < width; x++ )
				{
					uint32_t index = y * width + x;
					uint32_t index2 = 2 * (y * prevWidth  + x);

					uint32_t red   = src_texture_bgra[ 4*index2 + 0 ];
					uint32_t green = src_texture_bgra[ 4*index2 + 1 ];
					uint32_t blue  = src_texture_bgra[ 4*index2 + 2 ];

					red   += src_texture_bgra[ 4*index2 + 4 + 0 ];
					green += src_texture_bgra[ 4*index2 + 4 + 1 ];
					blue  += src_texture_bgra[ 4*index2 + 4 + 2 ];

					red   += src_texture_bgra[ 4*index2 + prevStride + 0 ];
					green += src_texture_bgra[ 4*index2 + prevStride + 1 ];
					blue  += src_texture_bgra[ 4*index2 + prevStride + 2 ];

					red   += src_texture_bgra[ 4*index2 + prevStride + 4 + 0 ];
					green += src_texture_bgra[ 4*index2 + prevStride + 4 + 1 ];
					blue  += src_texture_bgra[ 4*index2 + prevStride + 4 + 2 ];

					src_texture_bgra[ 4*index + 0 ] = (uint8_t) (red / 4);
					src_texture_bgra[ 4*index + 1 ] = (uint8_t) (green / 4);
					src_texture_bgra[ 4*index + 2 ] = (uint8_t) (blue / 4);
				}
			}
		}

		int64_t mse_alpha = 0;
		int64_t mse_color = 0;
		BlockSSIM ssim = BlockSSIM(0, 0);
		memset(dst_bc7, 0, finalSize);
		PackTexture(bc7Core, dst_bc7, src_texture_bgra, mask_agrb, stride, width, height, bc7Core.pCompress, 16, mse_alpha, mse_color, ssim);

		if ( m == 0 )
		{
			mse_alpha_final = mse_alpha;
			mse_color_final = mse_color;
			ssim_final = ssim;
		}

		uint8_t* pDstPtr = mipChain.GetImages()[m].pixels;
		memcpy( pDstPtr, dst_bc7, finalSize );
	}

	int pixels = src_texture_h * src_texture_w;

	if (mse_alpha_final > 0)
	{
		PRINTF("      SubTexture A qMSE = %.1f, qPSNR = %f, SSIM_4x4 = %.8f",
			(1.0 / kAlpha) * mse_alpha_final / pixels,
			10.0 * log((255.0 * 255.0) * kAlpha * pixels / mse_alpha_final) / log(10.0),
			ssim_final.Alpha * 16.0 / pixels);
	}
	else
	{
		PRINTF("      Whole A");
	}

	if (mse_color_final > 0)
	{
		PRINTF("      SubTexture RGB qMSE = %.1f, qPSNR = %f, wSSIM_4x4 = %.8f",
			(1.0 / kColor) * mse_color_final / pixels,
			10.0 * log((255.0 * 255.0) * kColor * pixels / mse_color_final) / log(10.0),
			ssim_final.Color * 16.0 / pixels);
	}
	else
	{
		PRINTF("      Whole RGB");
	}

	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dst_name, -1, wDstFilename, sizeof(wDstFilename) );
	hr = DirectX::SaveToDDSFile( mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK ) 
	{
		PRINTF( "Save DDS Failed: 0x%08x", hr );
	}
	else 
	{
		PRINTF( "Saved DDS: %s", dst_name );
	}

	delete[] dst_bc7;
	delete[] mask_agrb;
	delete[] src_texture_bgra;

	return 0;
}

int CompressNormals( const char* srcFilename, const char* dstFilename )
{
	HRESULT hr;

	PRINTF( "Normal Map" );
	PRINTF( "  Loading normal map %s", srcFilename );

	int width, height, channels;
	uint8_t* imageData = stbi_load( srcFilename, &width, &height, &channels, 4 );
	if ( !imageData )
	{
		PRINTF( "  Not found, trying jpg" );

		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, srcFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		imageData = stbi_load( newSrcFilename, &width, &height, &channels, 4 );
		if ( !imageData )
		{
			PRINTF( "  Failed to load normal map %s", srcFilename );
			return 0;
		}
	}

	PRINTF( "  Loaded normal map" );

	uint32_t finalWidth = width;
	uint32_t finalHeight = height;
	uint32_t mipLevels = 1;
	uint32_t testWidth = 1;
	while( testWidth < finalWidth )
	{
		testWidth <<= 1;
		mipLevels++;
	}

	// create final image
	DirectX::ScratchImage mipChain;
	hr = mipChain.Initialize2D( DXGI_FORMAT_R8G8_UNORM, finalWidth, finalHeight, 1, mipLevels, DirectX::DDS_FLAGS_NONE );
		
	struct vec3
	{
		float x, y ,z;
	};

	// convert image data into vectors
	vec3** pNormalMaps = new vec3*[ mipLevels ];
	pNormalMaps[ 0 ] = new vec3[ finalWidth * finalHeight ];

	for( uint32_t y = 0; y < finalHeight; y++ )
	{
		for( uint32_t x = 0; x < finalWidth; x++ )
		{
			uint32_t index = y * finalWidth + x;

			float nx = (imageData[ 4*index + 0 ] / 255.0f) * 2.0f - 1.0f;
			float ny = (imageData[ 4*index + 1 ] / 255.0f) * 2.0f - 1.0f;
			//float nz = (imageData[ 4*index + 2 ] / 255.0f) * 2.0f - 1.0f;
			float nz = 1 - sqrt(nx*nx + ny*ny);

			pNormalMaps[ 0 ][ index ].x = nx;
			pNormalMaps[ 0 ][ index ].y = ny;
			pNormalMaps[ 0 ][ index ].z = nz;
		}
	}

	delete [] imageData;

	PRINTF( "  Creating mip levels" );

	// create mip levels
	for( uint32_t m = 1; m < mipLevels; m++ )
	{
		uint32_t mipWidth = finalWidth >> m;
		uint32_t mipHeight = finalHeight >> m;
		pNormalMaps[ m ] = new vec3[ mipWidth * mipHeight ];

		uint32_t prevMipWidth = finalWidth >> (m-1);
		uint32_t prevMipHeight = finalHeight >> (m-1);

		for( uint32_t y = 0; y < mipHeight; y++ )
		{
			for( uint32_t x = 0; x < mipWidth; x++ )
			{
				uint32_t index = y * mipWidth + x;
				uint32_t index2 = 2 * (y * prevMipWidth + x);

				float nx,ny,nz;

				nx = pNormalMaps[ m-1 ][ index2 ].x;
				ny = pNormalMaps[ m-1 ][ index2 ].y;
				nz = pNormalMaps[ m-1 ][ index2 ].z;
				
				nx += pNormalMaps[ m-1 ][ index2+1 ].x;
				ny += pNormalMaps[ m-1 ][ index2+1 ].y;
				nz += pNormalMaps[ m-1 ][ index2+1 ].z;
				
				nx += pNormalMaps[ m-1 ][ index2+prevMipWidth ].x;
				ny += pNormalMaps[ m-1 ][ index2+prevMipWidth ].y;
				nz += pNormalMaps[ m-1 ][ index2+prevMipWidth ].z;
				
				nx += pNormalMaps[ m-1 ][ index2+prevMipWidth+1 ].x;
				ny += pNormalMaps[ m-1 ][ index2+prevMipWidth+1 ].y;
				nz += pNormalMaps[ m-1 ][ index2+prevMipWidth+1 ].z;

				float length = nx*nx + ny*ny + nz*nz;
				if ( length == 0 )
				{
					nx = 0;
					ny = 0;
					nz = 0;
				}
				else
				{
					length = sqrt( length );
					nx = nx / length;
					ny = ny / length;
					nz = nz / length;
				}

				pNormalMaps[ m ][ index ].x = nx;
				pNormalMaps[ m ][ index ].y = ny;
				pNormalMaps[ m ][ index ].z = nz;
			}
		}
	}

	PRINTF( "  Correcting normal bias" );

	float avgX = pNormalMaps[ mipLevels-1 ][ 0 ].x;
	float avgY = pNormalMaps[ mipLevels-1 ][ 0 ].y;
	float avgZ = pNormalMaps[ mipLevels-1 ][ 0 ].z;

	// copy into textures
	for( uint32_t m = 0; m < mipLevels; m++ )
	{
		uint32_t mipWidth = finalWidth >> m;
		uint32_t mipHeight = finalHeight >> m;
		
		uint8_t* pDstPtr = mipChain.GetImages()[m].pixels;

		for( uint32_t y = 0; y < mipHeight; y++ )
		{
			for( uint32_t x = 0; x < mipWidth; x++ )
			{
				uint32_t index = y * mipWidth + x;

				float nx,ny,nz,nx2,ny2,nz2;

				nx = pNormalMaps[ m ][ index ].x;
				ny = pNormalMaps[ m ][ index ].y;
				nz = pNormalMaps[ m ][ index ].z;

				nx2 = avgZ*nx - avgX*nz;
				ny2 = ny;
				nz2 = avgX*nx + avgZ*nz;

				nx = nx2;
				ny = avgZ*ny2 - avgY*nz2;
				nz = avgY*ny2 + avgZ*nz2;

				int iNx = (int) ((nx + 1.0f) / 2.0f * 255.0f);
				int iNy = (int) ((ny + 1.0f) / 2.0f * 255.0f);

				if ( iNx > 255 ) iNx = 255;
				if ( iNx < 0 ) iNx = 0;

				if ( iNy > 255 ) iNy = 255;
				if ( iNy < 0 ) iNy = 0;

				// only copy red and green channels into the texture
				*(pDstPtr + 0) = iNx;
				*(pDstPtr + 1) = iNy;
				pDstPtr += 2;
			}
		}
	}

	for( uint32_t m = 0; m < mipLevels; m++ )
	{
		delete [] pNormalMaps[ m ];
	}
	delete [] pNormalMaps;

	PRINTF( "  Compressing normal map" );

	// compress resulting image using BC5 (2 channel greyscale)
	DirectX::ScratchImage compressedSurface;
	hr = DirectX::Compress( mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC5_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface );
	if ( hr != S_OK )
	{
		PRINTF( "  Failed to compress normal map (%d)", hr );
		return 1;
	}

	mipChain.Release();

	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( compressedSurface.GetImages(), compressedSurface.GetImageCount(), compressedSurface.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		PRINTF( "  Failed to save normal map (%d) %s", hr, dstFilename );
		return 1;
	}

	PRINTF( "  Saved normal map %s", dstFilename );

	compressedSurface.Release();

	return 0;
}


// compress roughness, metalness, and AO into single BC1 texture
int CompressSurface( const char* roughnessFilename, const char* metalnessFilename, const char* aoFilename, const char* dstFilename )
{
	HRESULT hr;

	PRINTF( "Surface Map" );
	PRINTF( "  Loading roughness %s", roughnessFilename );

	int width, height, channels;
	uint8_t* roughnessImageData = stbi_load( roughnessFilename, &width, &height, &channels, 4 );
	if ( !roughnessImageData )
	{
		PRINTF( "  Not found, trying jpg" );

		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, roughnessFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		roughnessImageData = stbi_load( newSrcFilename, &width, &height, &channels, 4 );
		if ( !roughnessImageData )
		{
			PRINTF( "  Failed to load roughness map %s", roughnessFilename );
			return 0;
		}
	}

	PRINTF( "  Loaded roughness map" );
	PRINTF( "  Loading metalness %s", metalnessFilename );

	int width2, height2;
	uint8_t* metalnessImageData = stbi_load( metalnessFilename, &width2, &height2, &channels, 4 );
	if ( !metalnessImageData )
	{
		PRINTF( "  Not found, trying jpg" );

		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, metalnessFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		metalnessImageData = stbi_load( newSrcFilename, &width2, &height2, &channels, 4 );
		if ( !metalnessImageData )
		{
			PRINTF( "  Failed to load metalness map %s", metalnessFilename );
			return 1;
		}
	}

	PRINTF( "  Loaded metalness" );
	PRINTF( "  Loading AO %s", aoFilename );

	int width3, height3;
	uint8_t* aoImageData = stbi_load( aoFilename, &width3, &height3, &channels, 4 );
	if ( !aoImageData )
	{
		PRINTF( "  Not found, trying jpg" );

		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, aoFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		aoImageData = stbi_load( newSrcFilename, &width2, &height2, &channels, 4 );
		if ( !aoImageData )
		{
			PRINTF( "  Failed to load AO map %s", aoFilename );
			return 1;
		}
	}

	PRINTF( "  Loaded AO" );

	if ( width != width2 || height != height2 || width != width3 || height != height3 )
	{
		PRINTF( "  Error: roughness, metalness, and AO must all be the same resolution" );
		return 1;
	}

	// create final image
	DirectX::ScratchImage srcImage;
	hr = srcImage.Initialize2D( DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1, DirectX::DDS_FLAGS_NONE );

	PRINTF( "  Merging images" );

	// copy source data into texture channels
	uint8_t* pDstPtr = srcImage.GetPixels();
	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			int index = y * width + x;

			// copy into red and green channels
			*(pDstPtr + 0) = aoImageData[ 4*index + 0 ];
			*(pDstPtr + 1) = roughnessImageData[ 4*index + 0 ];
			*(pDstPtr + 2) = metalnessImageData[ 4*index + 0 ];
			*(pDstPtr + 3) = 255;
			pDstPtr += 4;
		}
	}

	delete [] roughnessImageData;
	delete [] metalnessImageData;
	delete [] aoImageData;

	PRINTF( "  Creating mipmaps" );
	
	// create mipmaps
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps( srcImage.GetImages()[0], TEX_FILTER_WRAP | TEX_FILTER_SEPARATE_ALPHA | TEX_FILTER_FORCE_NON_WIC, 0, mipChain );
	if ( hr != S_OK )
	{
		PRINTF( "  Failed to generate mipmaps for surface (%d)", hr );
		return 1;
	}

	srcImage.Release();

	PRINTF( "  Compressing image" );

	// compress resulting image
	DirectX::ScratchImage compressedSurface;
	hr = DirectX::Compress( mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC1_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface );
	if ( hr != S_OK )
	{
		PRINTF( "  Failed to compress surface (%d)", hr );
		return 1;
	}

	mipChain.Release();

	PRINTF( "  Saving image" );
			
	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( compressedSurface.GetImages(), compressedSurface.GetImageCount(), compressedSurface.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		PRINTF( "Failed to save surface (%d) %s", hr, dstFilename );
		return 1;
	}

	PRINTF( "  Saved surface %s", dstFilename );

	compressedSurface.Release();

	return 0;
}

// compress roughness and metalness into single BC5 texture (2 channel greyscale)
int CompressRoughnessMetalness( const char* roughnessFilename, const char* metalnessFilename, const char* dstFilename )
{
	HRESULT hr;

	PRINTF( "Roughness/Metalness Map" );
	PRINTF( "  Loading roughness %s", roughnessFilename );

	int width, height, channels;
	uint8_t* roughnessImageData = stbi_load( roughnessFilename, &width, &height, &channels, 4 );
	if ( !roughnessImageData )
	{
		PRINTF( "  Not found, trying jpg" );

		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, roughnessFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		roughnessImageData = stbi_load( newSrcFilename, &width, &height, &channels, 4 );
		if ( !roughnessImageData )
		{
			PRINTF( "  Failed to load roughness map %s", newSrcFilename );
			return 0;
		}
	}

	PRINTF( "  Loaded roughness map" );
	PRINTF( "  Loading metalness %s", metalnessFilename );

	int width2, height2;
	uint8_t* metalnessImageData = stbi_load( metalnessFilename, &width2, &height2, &channels, 4 );
	if ( !metalnessImageData )
	{
		PRINTF( "  Not found, trying jpg" );

		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, metalnessFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		metalnessImageData = stbi_load( newSrcFilename, &width2, &height2, &channels, 4 );
		if ( !metalnessImageData )
		{
			PRINTF( "  Failed to load metalness map %s", newSrcFilename );
			return 1;
		}
	}

	PRINTF( "  Loaded metalness" );

	if ( width != width2 || height != height2 )
	{
		PRINTF( "  Error: roughness and metalness map must be the same resolution" );
		return 1;
	}

	// create final image
	DirectX::ScratchImage srcImage;
	hr = srcImage.Initialize2D( DXGI_FORMAT_R8G8_UNORM, width, height, 1, 1, DirectX::DDS_FLAGS_NONE );

	PRINTF( "  Merging images" );

	// copy source data into texture channels
	uint8_t* pDstPtr = srcImage.GetPixels();
	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			int index = y * width + x;

			// copy into red and green channels
			*(pDstPtr + 0) = roughnessImageData[ 4*index + 0 ];
			*(pDstPtr + 1) = metalnessImageData[ 4*index + 0 ];
			pDstPtr += 2;
		}
	}

	delete [] roughnessImageData;
	delete [] metalnessImageData;

	PRINTF( "  Creating mipmaps" );
	
	// create mipmaps
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps( srcImage.GetImages()[0], TEX_FILTER_WRAP | TEX_FILTER_SEPARATE_ALPHA | TEX_FILTER_FORCE_NON_WIC, 0, mipChain );
	if ( hr != S_OK )
	{
		PRINTF( "  Failed to generate mipmaps for roughness/metalness (%d)", hr );
		return 1;
	}

	srcImage.Release();

	PRINTF( "  Compressing image" );

	// compress resulting image
	DirectX::ScratchImage compressedSurface;
	hr = DirectX::Compress( mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC5_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface );
	if ( hr != S_OK )
	{
		PRINTF( "  Failed to compress roughness/metalness (%d)", hr );
		return 1;
	}

	mipChain.Release();

	PRINTF( "  Saving image" );
			
	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( compressedSurface.GetImages(), compressedSurface.GetImageCount(), compressedSurface.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		PRINTF( "Failed to save roughness/metalness (%d) %s", hr, dstFilename );
		return 1;
	}

	PRINTF( "  Saved roughness/metalness %s", dstFilename );

	compressedSurface.Release();

	return 0;
}

// compress ambient occulusion as single BC4 texture (1 channel greyscale)
int CompressAO( const char* aoFilename, const char* dstFilename )
{
	HRESULT hr;

	PRINTF( "Ambient Occulusion Map" );
	PRINTF( "  Loading AO %s", aoFilename );

	int width, height, channels;
	uint8_t* aoImageData = stbi_load( aoFilename, &width, &height, &channels, 4 );
	if ( !aoImageData )
	{
		PRINTF( "  Not found, trying jpg" );

		char newSrcFilename[ MAX_PATH ];
		strcpy_s( newSrcFilename, MAX_PATH, aoFilename );
		char* szExt = strrchr( newSrcFilename, '.' );
		if ( szExt ) strcpy_s( szExt, 5, ".jpg" );
		else strcat_s( newSrcFilename, MAX_PATH, ".jpg" );
		aoImageData = stbi_load( newSrcFilename, &width, &height, &channels, 4 );
		if ( !aoImageData )
		{
			PRINTF( "  Failed to load AO %s", newSrcFilename );
			return 0;
		}
	}

	PRINTF( "  Loaded AO" );

	// create final image
	DirectX::ScratchImage srcImage;
	hr = srcImage.Initialize2D( DXGI_FORMAT_R8_UNORM, width, height, 1, 1, DirectX::DDS_FLAGS_NONE );

	// copy source data into texture channels
	uint8_t* pDstPtr = srcImage.GetPixels();
	for( int y = 0; y < height; y++ )
	{
		for( int x = 0; x < width; x++ )
		{
			int index = y * width + x;

			// copy into red and green channels
			*(pDstPtr + 0) = aoImageData[ 4*index + 0 ];
			pDstPtr += 1;
		}
	}

	delete [] aoImageData;

	PRINTF( "  Creating mipmaps" );
	
	// create mipmaps
	DirectX::ScratchImage mipChain;
	hr = DirectX::GenerateMipMaps( srcImage.GetImages()[0], TEX_FILTER_WRAP | TEX_FILTER_SEPARATE_ALPHA | TEX_FILTER_FORCE_NON_WIC, 0, mipChain );
	if ( hr != S_OK )
	{
		PRINTF( "  Failed to generate mipmaps for AO (%d)", hr );
		return 1;
	}

	srcImage.Release();

	PRINTF( "  Compressing AO" );

	// compress resulting image
	DirectX::ScratchImage compressedSurface;
	hr = DirectX::Compress( mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), DXGI_FORMAT_BC4_UNORM, TEX_COMPRESS_PARALLEL, DirectX::TEX_THRESHOLD_DEFAULT, compressedSurface );
	if ( hr != S_OK )
	{
		PRINTF( "  Failed to compress AO (%d)", hr );
		return 1;
	}

	mipChain.Release();

	PRINTF( "  Saving AO" );
			
	// save the image as a DDS
	wchar_t wDstFilename[ MAX_PATH ];
	MultiByteToWideChar( CP_ACP, 0, dstFilename, -1, wDstFilename, sizeof(wDstFilename) );
    hr = DirectX::SaveToDDSFile( compressedSurface.GetImages(), compressedSurface.GetImageCount(), compressedSurface.GetMetadata(), DirectX::DDS_FLAGS_NONE, wDstFilename );
	if ( hr != S_OK )
	{
		PRINTF( "  Failed to save AO (%d) %s", hr, dstFilename );
		return 1;
	}

	PRINTF( "  Saved AO %s", dstFilename );

	compressedSurface.Release();

	return 0;
}

#if !defined(OPTION_LIBRARY) && defined(WIN32)

bool GetBc7Core(void* bc7Core);

#include "PNGToDDSTypes.h"
int ConvertMaterialToDDSFromPNG(MaterialToConvert* mat)
{
	IBc7Core bc7Core{};
	if (!GetBc7Core(&bc7Core))
	{
		return 0;
	}

	Bc7GGMax(bc7Core, mat->color[0].c_str(), mat->color[1].c_str());

	CompressNormals(mat->normal[0].c_str(), mat->normal[1].c_str());

	CompressSurface(mat->roughness.c_str(), mat->metalness.c_str(), mat->ao.c_str(), mat->surface[1].c_str());

	return 1;
}

int __cdecl main(int argc, char* argv[])
{
	(void)argc; 
	(void)argv;

	IBc7Core bc7Core{};
	if (!GetBc7Core(&bc7Core))
	{
		PRINTF("Unsupported CPU");
		return 2;
	}

	/*
	const char* origNames[] = {
		"rrRFpeBol0yyENN8gdrw7g_4K",
		"TWfeorkCv0yHYqJWgteq0A_4K",
		"7gir7IuixU6sdfkf_88ZNw_4K",
		"BTcqk6tO5EOog-wvmZpYvQ_4K",
		"4VtFp_v6-EOPiUMWilDGIA_4K",
		"YMKpuREwMUultXc196xPtw_4K",
		"F4FEiA96a02If3JnSCa_1Q_4K",
		"mSUxKvgtOkqoooYbmSJ6kA_4K",
		"1oKsiZAKp0G7vzzZFw-MNw_4K",
		"5jYkmOUSi0-yCum8lV2YdA_4K",
		"5e46JhgUGk2oVHTKKoSb_g_4K",
		"Ground 56_4K",
		"snow_26",
		"snow_29",
		"snow_25",
		"snow_04",
		"Rock 778_4K",
		"Rock 777_4K",
		"Rock 784_4K",
		"Rock 782_4K",
	};

	const char* folderNames[] = {
		"01 - Grass, mud and stones 1",
		"02 - Grass, mud and stones 2",
		"03 - Grass, tree roots and stones",
		"04 - Mud and rocks",
		"05 - Mud, rocks and weeds 1",
		"06 - Mud, rocks and weeds 2",
		"07 - Muddy",
		"08 - Muddy rocks",
		"09 - Sandy and rocks 1",
		"10 - Sandy and rocks 2",
		"11 - Sandy and rocks 3",
		"12 - Sand, rocks and weeds",
		"13 - Cracking ice",
		"14 - Snow and ice",
		"15 - Ice and rocks 1",
		"16 - Ice and rocks 2",
		"17 - Dark rock 1",
		"18 - Dark rock 2",
		"19 - Mossy rock",
		"20 - Dark rock 3",
	};
	
	for( uint32_t i = 1; i < 20; i++ )
	{
		char szSrcRoot[ MAX_PATH ];
		char szDstRoot[ MAX_PATH ];

		sprintf_s( szSrcRoot, MAX_PATH, "E:\\Downloads\\Terrain Textures Original\\textures\\maps_%02d\\maps\\%s", i+1, origNames[ i ] );
		sprintf_s( szDstRoot, MAX_PATH, "E:\\Temp\\GGMax\\Final\\%s\\%s", folderNames[ i ], folderNames[ i ] + 5 );
		
		char szSrcColorPath[ MAX_PATH ];
		char szDstColorPath[ MAX_PATH ];

		sprintf_s( szSrcColorPath, MAX_PATH, "%s_Albedo.png", szSrcRoot );
		sprintf_s( szDstColorPath, MAX_PATH, "%s_Color.dds", szDstRoot );
		
		if ( Bc7GGMax( bc7Core, szSrcColorPath, szDstColorPath ) ) return 1;
	}
	*/
	
	/*
	char szSrcColorPath[ MAX_PATH ];
	char szDstColorPath[ MAX_PATH ];
	
	strcpy_s( szSrcColorPath, MAX_PATH, "E:\\Downloads\\grass2\\grass_green_02_basecolor.png" );
	strcpy_s( szDstColorPath, MAX_PATH, "E:\\Downloads\\grass2\\grass2_Color.dds" );
		
	if ( Bc7GGMax( bc7Core, szSrcColorPath, szDstColorPath ) ) return 1;
	*/

	if ( Bc7GGMax( bc7Core, "Color.png", "Color.dds" ) ) goto end;

	if ( CompressNormals( "Normal.png", "Normal.dds" ) ) goto end;

	if ( CompressSurface( "Roughness.png", "Metalness.png", "AO.png", "Surface.dds" ) ) goto end;

	//if ( CompressRoughnessMetalness( "Roughness.png", "Metalness.png", "RoughnessMetalness.dds" ) ) goto end;
	//if ( CompressAO( "AO.png", "AO.dds" ) ) goto end;

	PRINTF( "Finished" );

end:
	system( "pause" );
	return 0;
}

#endif
