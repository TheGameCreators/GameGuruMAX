// GameGuru Launcher.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "shellapi.h"
#include "GameGuru Launcher.h"

// Includes for DirectX Adapter Interrogation
#include <vector>
#include "dxgi.h"
#include "d3dcommon.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	/*
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GAMEGURULAUNCHER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAMEGURULAUNCHER));

    MSG msg;

	MessageBox(NULL, L"Offers user option to run VR or non-VR, change SETUP.INI settings, resolution, GPU adapter, etc. Click OK to launch GameGuru Project V3!", L"SETUP", MB_OK);
	PostQuitMessage(-1);

	// Create list of DirectX Adapters
	IDXGIAdapter* pAdapter = NULL;
	char m_pAdapterName[260];
	IDXGIFactory* pFactory = NULL;
	std::vector <LPSTR> AdapterList;
	AdapterList.clear();
	if (SUCCEEDED(CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&pFactory)))
	{
		for (int iAdapterIndex = 0; iAdapterIndex < 10; iAdapterIndex++)
		{
			if (pFactory->EnumAdapters(iAdapterIndex, &pAdapter) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC adapterDesc;
				pAdapter->GetDesc(&adapterDesc);
				memset(m_pAdapterName, 0, sizeof(m_pAdapterName));
				const int size = ::WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, NULL, 0, 0, NULL);
				::WideCharToMultiByte(CP_UTF8, 0, adapterDesc.Description, -1, m_pAdapterName, size, 0, NULL);
				LPSTR pAdapterName = new char[260];
				strcpy(pAdapterName, m_pAdapterName);
				AdapterList.push_back(pAdapterName);
			}
		}
	}
	FILE* dest = fopen("adapterlist.txt", "wb");
	for (int iOrdinal = 0; iOrdinal < AdapterList.size(); iOrdinal++)
	{
		char pLine[300];
		sprintf(pLine, "%d : %s\n", iOrdinal, AdapterList[iOrdinal]);
		fwrite(pLine, 1, strlen(pLine), dest);
	}
	fclose(dest);

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	*/

	// Launches real binary inside Core
	SetCurrentDirectoryA("Max");
	ShellExecuteA(NULL, "open", "GameGuruMAX.exe", NULL, NULL, SW_SHOW);
	return 0;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEGURULAUNCHER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAMEGURULAUNCHER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GAMEGURULAUNCHER));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
