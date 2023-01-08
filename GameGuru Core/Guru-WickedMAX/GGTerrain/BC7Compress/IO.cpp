
#include "pch.h"
#include "IO.h"

#if defined(_WIN32)
#include <windows.h>
#pragma warning(push)
#pragma warning(disable : 4458)
#include <gdiplus.h>
#pragma warning(pop)
#pragma comment(lib, "gdiplus.lib")
#endif

#include <string>

#if !defined(OPTION_LIBRARY) && defined(WIN32)

bool ReadImage(const char* src_name, uint8_t* &pixels, int &width, int &height, bool flip)
{
	ULONG_PTR gdiplusToken;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		std::wstring wide_src_name;
		wide_src_name.resize(std::mbstowcs(nullptr, src_name, MAX_PATH));
		std::mbstowcs(&wide_src_name.front(), src_name, MAX_PATH);

		Gdiplus::Bitmap bitmap(wide_src_name.c_str(), FALSE);

		width = (int)bitmap.GetWidth();
		height = (int)bitmap.GetHeight();

		Gdiplus::Rect rect(0, 0, width, height);
		Gdiplus::BitmapData data;
		if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &data) == 0)
		{
			int stride = width << 2;

			pixels = new uint8_t[height * stride];

			uint8_t* w = pixels;
			for (int y = 0; y < height; y++)
			{
				const uint8_t* r = (const uint8_t*)data.Scan0 + (flip ? height - 1 - y : y) * data.Stride;
				memcpy(w, r, stride);
				w += stride;
			}

			bitmap.UnlockBits(&data);
		}
		else
		{
			pixels = nullptr;
		}
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);

	return pixels != nullptr;
}

void WriteImage(const char* dst_name, const uint8_t* pixels, int w, int h, bool flip)
{
	ULONG_PTR gdiplusToken;

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		Gdiplus::Bitmap bitmap(w, h, PixelFormat32bppARGB);

		Gdiplus::Rect rect(0, 0, w, h);
		Gdiplus::BitmapData data;
		if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &data) == 0)
		{
			for (int y = 0; y < h; y++)
			{
				memcpy((uint8_t*)data.Scan0 + (flip ? h - 1 - y : y) * data.Stride, pixels + y * w * 4, w * 4);
			}

			bitmap.UnlockBits(&data);
		}

		CLSID format;
		bool ok = false;
		{
			UINT num, size;
			Gdiplus::GetImageEncodersSize(&num, &size);
			if (size >= num * sizeof(Gdiplus::ImageCodecInfo))
			{
				Gdiplus::ImageCodecInfo* pArray = (Gdiplus::ImageCodecInfo*)new uint8_t[size];
				Gdiplus::GetImageEncoders(num, size, pArray);

				for (UINT i = 0; i < num; i++)
				{
					if (pArray[i].FormatID == Gdiplus::ImageFormatPNG)
					{
						format = pArray[i].Clsid;
						ok = true;
						break;
					}
				}

				delete[](uint8_t*)pArray;
			}
		}
		if (ok)
		{
			std::wstring wide_dst_name;
			wide_dst_name.resize(std::mbstowcs(nullptr, dst_name, MAX_PATH));
			std::mbstowcs(&wide_dst_name.front(), dst_name, MAX_PATH);

			ok = (bitmap.Save(wide_dst_name.c_str(), &format) == Gdiplus::Ok);
		}

		PRINTF(ok ? "  Saved %s" : "Lost %s", dst_name);
	}
	Gdiplus::GdiplusShutdown(gdiplusToken);
}

void LoadBc7(const char* name, int position, uint8_t* buffer, int size)
{
	std::wstring wName;

	HANDLE file = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	//HANDLE file = CreateFile(name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		SetFilePointer(file, position, NULL, FILE_BEGIN);

		DWORD transferred;
		BOOL ok = ReadFile(file, buffer, size, &transferred, NULL);

		CloseHandle(file);

		if (ok)
		{
			PRINTF("    Loaded %s", name);
		}
	}
}

void SaveBc7(const char* name, const uint8_t* head, int position, const uint8_t* buffer, int size)
{
	bool ok = false;

	HANDLE file = CreateFileA(name, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD transferred;
		ok = (WriteFile(file, head, position, &transferred, NULL) != 0);
		ok &= (WriteFile(file, buffer, size, &transferred, NULL) != 0);

		CloseHandle(file);
	}

	PRINTF(ok ? "    Saved %s" : "Lost %s", name);
}

#endif
