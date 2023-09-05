//
// Main Wicked MAX Software Entry Point
//

#define DPIAWARE

// Includes
#include "stdafx.h"
#include "main.h"
#include "master.h"
#include "globstruct.h"
#include "CGfxC.h"
#include "CFileC.h"
#include "timeapi.h"
#ifdef DPIAWARE
#include "shellscalingapi.h"
#endif
// Defines
#define MAX_LOADSTRING 100

// Global Variables
HINSTANCE hInst;                                // current instance
HWND hMainWnd;									// main hwnd (for now)
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
DWORD dwLaunchTimer = 0;

// Externals
extern GlobStruct*	g_pGlob;

// From DarkEXE
LPSTR				gRefCommandLineString = NULL;
int					iLastResolutionWidth = 0;
int					iLastResolutionHeight = 0;
bool				bSpecialStandalone = false;
bool				bReturnToWelcome = false;
bool				bSpecialEditorFromStandalone = false;
bool				bEnsureIntroVideoIsNotRun = false;
char				cSpecialStandaloneProject[MAX_PATH];

// Forward declarations of functions included in this code module
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
const char *pestrcasestr(const char *arg1, const char *arg2);

// global flag to reduce workload if window not in focus (solves stutter slowdown?)
bool g_bActiveApp = true;
bool g_bAppActiveStat = true;
bool g_bLostFocus = false;

// Encapsulates all other classes for Wicked Engine control
Master master;

// Functions

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	// parse command line parameters
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	wiStartupArguments::Parse(lpCmdLine);

	//Debug wicked. startup using DirectX debug layer.
	//WCHAR tmp[80];
	//wcscpy(tmp, L"debugdevice");
	//wiStartupArguments::Parse(&tmp[0]);

	//MessageBoxA(NULL, "WinMain", "Log", 0);

	// Command line store
	std::wstring your_wchar_in_ws(lpCmdLine);
	std::string your_wchar_in_str(your_wchar_in_ws.begin(), your_wchar_in_ws.end());
	char* your_wchar_in_char = (char*)your_wchar_in_str.c_str();
	gRefCommandLineString = new char[MAX_PATH];
	strcpy_s(gRefCommandLineString, MAX_PATH, your_wchar_in_char);

	// Launch Timer Started
	dwLaunchTimer = timeGetTime();

	//PE: If we dont have any command line parameters check if another copy is running.
	//PE: We could need multiply copies running when in standalone (but will always have parameters).
	CreateMutexA(0, FALSE, "Local\\$gamegurumaxrunningnow$");
	if (strlen(gRefCommandLineString) == 0)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			char launchname[MAX_PATH];
			GetModuleFileNameA(hInstance, &launchname[0], MAX_PATH);
			if (pestrcasestr(launchname, "gamegurumax.exe"))
			{
				MessageBoxA(NULL, "A copy of GameGuru MAX is already running.", "Error", 0); //"Cannot run standalone game!"
			}
			else
			{
				MessageBoxA(NULL, "You must close GameGuru MAX before running a standalone game.", "Cannot run standalone game!", 0);
			}
			ExitProcess(0);
		}
	}
	else
	{
		char *find = (char *) pestrcasestr(gRefCommandLineString, "project=");
		if (find)
		{
			if (find) find += 8;
			extern bool bStartInvulnerableMode;
			bStartInvulnerableMode = false;
			if (find[0] == '1') bStartInvulnerableMode = true;
			if (find[0] == '3') bStartInvulnerableMode = true;

			bReturnToWelcome = false;
			if (find[0] == '2') bReturnToWelcome = true;
			if (find[0] == '3') bReturnToWelcome = true;

			find++;
			strcpy(cSpecialStandaloneProject, find);
			bSpecialStandalone = true;
		}
		else if (find = (char *)pestrcasestr(gRefCommandLineString, "editorfromstandalone="))
		{
			if (find) find += 21;
			strcpy(cSpecialStandaloneProject, find);
			bSpecialEditorFromStandalone = true;
			bReturnToWelcome = false;
			//MessageBoxA(NULL, "editor", "edior", 0);
		}
		else if (find = (char *)pestrcasestr(gRefCommandLineString, "editorfromstandalone2="))
		{
			if (find) find += 22;
			strcpy(cSpecialStandaloneProject, find);
			bSpecialEditorFromStandalone = true;
			bReturnToWelcome = true;
		}
	}
	// As it is this early, check registry whether we ignore DPI Awareness
	#ifdef DPIAWARE
	char pDPINotAware[256];
	strcpy ( pDPINotAware, "0" );
	// read DPI Aware from registry (set by SETTINGS within the software but requires a relaunch)
	HKEY hKeyNames = 0;
	DWORD Status = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\GameGuruMAX", 0L, KEY_READ, &hKeyNames);
	if (Status == ERROR_SUCCESS)
	{
		DWORD Type = REG_SZ;
		DWORD Size = 256;
		Status = RegQueryValueExA(hKeyNames, "DPINotAware", NULL, &Type, NULL, &Size);
		if (Size < 255)
		{
			RegQueryValueExA(hKeyNames, "DPINotAware", NULL, &Type, (LPBYTE)pDPINotAware, &Size);
		}
		RegCloseKey(hKeyNames);
	}
	if ( pDPINotAware[0] == '0' )
	{
		SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	}
	#endif

	//PE: check graphics card. Workaround for amd issues.
	bool bUseAMDHotFIx = true;
	if (GG_FileExists("noamdfix.ini") == 1) bUseAMDHotFIx = false;
	if (bUseAMDHotFIx == true)
	{
		bool bIsAMDCard = false;
		for (int i = 0; i < 4; i++)
		{
			DISPLAY_DEVICE dd = { sizeof(dd), 0 };
			BOOL f = EnumDisplayDevices(NULL, i, &dd, EDD_GET_DEVICE_INTERFACE_NAME);
			if (!f) break;
			char cDeviceName[MAX_PATH];
			int length = wcstombs(cDeviceName, dd.DeviceString, MAX_PATH);
			/*
			AMD driver bug.
			PE: this is the card i noted had the problem, there might be more :)
			AMD Radeon RX 6600m
			Radeon RX 6950 XT
			RX 6900XT
			AMD 5600 XT
			AMD RX 6800
			AMD Radeon RX 7900
			Radeon RX 6700 XT
			AMD Radeon RX 6500 XT
			NOTE: make sure shaders\\d3d11.dll , shaders\\dxgi.dll get copied to the standalone.
			*/
			if (length > 0)
			{
				if (pestrcasestr(cDeviceName, "AMD") || pestrcasestr(cDeviceName, "Radeon"))
				{
					//PE: Take all 6900,5600,6800,6600 RX serie if amd.
					if (pestrcasestr(cDeviceName, "RX"))
					{
						// more reports coming in, this time "AMD ryzen 7 6000" so lets assume ALL RX need the fix!
						//if (pestrcasestr(cDeviceName, "89") || pestrcasestr(cDeviceName, "79") || pestrcasestr(cDeviceName, "69") || pestrcasestr(cDeviceName, "68") || pestrcasestr(cDeviceName, "66") || pestrcasestr(cDeviceName, "67") || pestrcasestr(cDeviceName, "65"))
						{
							bIsAMDCard = true;
							break;
						}
					}
					else
					{
						//PE: special case for amd 5600 xt
						if (pestrcasestr(cDeviceName, "5600"))
						{
							bIsAMDCard = true;
							break;
						}
					}
				}
			}
		}
		if (bIsAMDCard)
		{
			//copy d3d11.dll ,dxgi.dll to convert DX11 -> Vulkan.
			CopyFileA((LPSTR)"shaders\\d3d11.dll", "d3d11.dll", TRUE);
			bool bret = CopyFileA((LPSTR)"shaders\\dxgi.dll", "dxgi.dll", TRUE);

			//PE: If first time , sleep so defender can check and release it before we try to load it.
			if (bret) Sleep(2000);

			//PE: Also Include building editor.
			CopyFileA((LPSTR)"shaders\\d3d11.dll", "Tools\\Building Editor\\d3d11.dll", TRUE);
			CopyFileA((LPSTR)"shaders\\dxgi.dll", "Tools\\Building Editor\\dxgi.dll", TRUE);
		}
	}
	else
	{
		// if old AMD HOT FIX files present, and mode for this is OFF, remove them
		if (GG_FileExists("d3d11.dll") == 1 || GG_FileExists("dxgi.dll") == 1)
		{
			char pMsgToRemove[MAX_PATH];
			char pCurrentDir[MAX_PATH];
			GetCurrentDirectoryA(MAX_PATH, pCurrentDir);
			sprintf(pMsgToRemove, "You must manually delete the D3D11.DLL and DXGI.DLL from the '%s' root folder to remove the AMD hot fix, then relaunch GameGuru MAX", pCurrentDir);
			MessageBoxA(NULL, pMsgToRemove, "NO AMD Hot Fix Detected", MB_OK);
			return 0;
		}
	}

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_APP_CLASSNAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	// Need window dimensions before calling InitInstance
	int iScrWidth = GetSystemMetrics(SM_CXSCREEN);
	int iScrHeight = GetSystemMetrics(SM_CYSCREEN);
	g_pGlob->dwWindowX = 0;
	g_pGlob->dwWindowY = 0;
	g_pGlob->dwWindowWidth = iScrWidth;
	g_pGlob->dwWindowHeight = iScrHeight;
	g_pGlob->iScreenWidth = iScrWidth;
	g_pGlob->iScreenHeight = iScrHeight;

    // Perform application initialization
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	// Set resource paths to Max root folder
	wiRenderer::SetShaderPath("shaders/");
	//wiFont::SetFontPath("fonts/"); no longer exists

	// Main application loop
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			// Run the update and render loop
			if (g_bActiveApp == true)
			{
				// full tilt
				master.RunCustom();
			}
			else
			{
				int ImGui_GetActiveViewPorts(void);
				int iActiveViewPorts = ImGui_GetActiveViewPorts();
				if (iActiveViewPorts > 1) //PE: We cant sleep if we got more hwnd's.
				{
					g_bActiveApp = true;
					master.RunCustom();
				}
				// if not got focus, chill!
				Sleep(1);
			}
		}
	}

	// final closing acts after loop 
	master.Finish();

	// leelee, tried terrain experiment brach, restored master branch, and complete new download of the master repo,
	// it seems on MY PC this executable is not quitting, and stays active beyond this point!, so!...
	HANDLE hDamnProcess = GetCurrentProcess();
	TerminateProcess(hDamnProcess, 0);

	// Exit app with result param of exit message
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEGURUMAX));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// Create window for app
	HWND hWnd = CreateWindowW( szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU, g_pGlob->dwWindowX, g_pGlob->dwWindowY, g_pGlob->dwWindowWidth, g_pGlob->dwWindowHeight, nullptr, nullptr, hInstance, nullptr);
	//HWND hWnd = CreateWindowW( szWindowClass, szTitle, WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX, g_pGlob->dwWindowX, g_pGlob->dwWindowY, g_pGlob->dwWindowWidth, g_pGlob->dwWindowHeight, nullptr, nullptr, hInstance, nullptr);
	//HWND hWnd = CreateWindowW( szWindowClass, szTitle, WS_POPUP | WS_BORDER | WS_SYSMENU, g_pGlob->dwWindowX, g_pGlob->dwWindowY, g_pGlob->dwWindowWidth, g_pGlob->dwWindowHeight, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) return FALSE;

	// Show and update window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	#ifdef ENABLEIMGUI
	#ifndef USEOLDIDE
	//PE: Setup the window here. pos size. Docking ?
	g_pGlob->hWnd = hWnd;
	//SetWindowLong(g_pGlob->hWnd, GWLP_WNDPROC, (LONG)ImguiWindowProc);
	//SetWindowSettings(5, 1, 1);
	//SetForegroundWindow(g_pGlob->hWnd);
	//SetWindowSize(pref.vStartResolution.x, pref.vStartResolution.y);
	//float centerx = (g_pGlob->dwWindowWidth*0.5);// -(pref.vStartResolution.x*0.5);
	//float centery = ((float)(g_pGlob->dwWindowHeight*0.5));// -(float)(pref.vStartResolution.y*0.5)) * 0.5f;
	//if (centerx < 0) centerx = 0;
	//if (centery < 0) centery = 0;
	//SetWindowPosition(centerx, centery);
	//ShowWindow();
	//extern DWORD gWindowSizeAddY;
	//extern DWORD gWindowSizeAddX;
	//RECT clientrc;
	//GetClientRect(g_pGlob->hWnd, &clientrc);
	//gWindowSizeAddY = pref.vStartResolution.y - clientrc.bottom;
	//gWindowSizeAddX = pref.vStartResolution.x - clientrc.right;
	//if(pref.iMaximized > 0)
	//MaximiseWindow();
	//ShowWindow(hWnd, nCmdShow);
	//UpdateWindow(hWnd);
	//else
	//	RestoreWindow();
	#endif
	#endif

	// Give Window handle to Wicked
	master.SetWindow(hWnd);

	// Store instance handle in our global variable
	hInst = hInstance; 
	hMainWnd = hWnd;

	// Complete
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// special IMGUI message handling
	#ifdef ENABLEIMGUI
	extern bool bImGuiInTestGame;
	extern bool bRenderTargetModalMode;
	//PE: IMGUI handle messages.
	LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// ensures active message always observed (even in and out of test game)
	if (message == WM_ACTIVATE )
	{
		// nothing fancy for new engine - though would like to know if we are in the background?
		if (hWnd == hMainWnd)
		{
			extern bool g_bActiveApp;
			extern bool g_bAppActiveStat;
			g_bActiveApp = true;
			//g_bAppActiveStat = true;
			if (wParam == WA_INACTIVE)
			{
				g_bLostFocus = true;

				// LB: only do this functionality after 45 seconds, allowing the launch process to 
				// proceed uninterupted, even if user has not focused on the app
				if (timeGetTime() > dwLaunchTimer + 45000)
				{
					g_bActiveApp = false;
					g_bAppActiveStat = false;
				}
			}
			else
			{
				if (g_bLostFocus)
				{
					// HOT SWAP Feature
					g_bLostFocus = false;
					extern void CheckExistingFilesModified(bool);
					CheckExistingFilesModified(false);

					// also trigger a workshop item refresh if on that page
					extern bool g_bUpdateWorkshopItemList;
					g_bUpdateWorkshopItemList = true;
				}
			}
		}
	}

	if (!bImGuiInTestGame) 
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
	}
	else 
	{
		//PE: In standalone we need g_cInkeyCodeKey and all the others to be set.
		extern bool bRenderTabTab;
		extern bool bNeedImGuiInput;
		if (bNeedImGuiInput) 
		{
			//Also need to update imgui.
			ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
		}
		LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		if (message != WM_SIZE)
		{
			WindowProc(hWnd, message, wParam, lParam);
		}
	}
	if (bRenderTargetModalMode)
	{
		//When in welcome system and modal mode.
		if (message == WM_SYSCOMMAND && (wParam & SC_MOVE) == SC_MOVE)
		{
			return TRUE; // Ignore if move
		}
		if (message == WM_NCLBUTTONDBLCLK) //PE: prevent window resize by mouse doubleclick on titlebar.
		{
			return TRUE;
		}
		if (message == WM_SYSCOMMAND && (wParam == SC_RESTORE || wParam == SC_MINIMIZE || wParam == SC_MAXIMIZE) )
			return TRUE; //Ignore any resize of window.
	}
	#endif

	// regular message handling
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
	case WM_SIZE:
		{
			if ( master.is_window_active ) master.SetWindow( hWnd );
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_HOME:
			wiBackLog::Toggle();
			break;
		case VK_UP:
			if (wiBackLog::isActive())
				wiBackLog::historyPrev();
			break;
		case VK_DOWN:
			if (wiBackLog::isActive())
				wiBackLog::historyNext();
			break;
		case VK_NEXT:
			if (wiBackLog::isActive())
				wiBackLog::Scroll(10);
			break;
		case VK_PRIOR:
			if (wiBackLog::isActive())
				wiBackLog::Scroll(-10);
			break;
		default:
			break;
		}
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case VK_BACK:
			if (wiBackLog::isActive())
				wiBackLog::deletefromInput();
			wiTextInputField::DeleteFromInput();
			break;
		case VK_RETURN:
			if (wiBackLog::isActive())
				wiBackLog::acceptInput();
			break;
		default:
		{
			const char c = (const char)(TCHAR)wParam;
			if (wiBackLog::isActive())
			{
				wiBackLog::input(c);
			}
			wiTextInputField::AddInput(c);
		}
		break;
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_CLOSE:
		// if free trial, first attempt to quit simply calls up free trial window
		extern bool g_bFreeTrialVersion;
		if (g_bFreeTrialVersion == true)
		{
			extern bool g_bFreeTrialNowExitsApp;
			if (g_bFreeTrialNowExitsApp == false)
			{
				// call up free trial window once when try to exit
				g_bFreeTrialNowExitsApp = true;
				extern bool bFreeTrial_Window;
				bFreeTrial_Window = true;
			}
			else
			{
				// second attempt succeeds
				g_bFreeTrialVersion = false;
			}
		}
		if (g_bFreeTrialVersion == false)
		{
			// standard quit functionality
			extern bool g_bDisableQuitFlag;
			if (g_bDisableQuitFlag == false)
			{
				int iRet = 0;
				#ifdef STORYBOARD
				//PE: Save storyboard if changed.
				int autosave_storyboard_project(void);
				iRet = autosave_storyboard_project();
				if (iRet == 2) return(0); //Cancel.
				#endif
				//Save changed level.
				int AskSaveBeforeNewAction(void);
				iRet = AskSaveBeforeNewAction();
				if (iRet != 2)
				{
					return DefWindowProc(hWnd, message, wParam, lParam);
				}
				return(0);
			}
			else
			{
				//Ignore close.
				return(0);
			}
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
        break;

	case WM_SETCURSOR:
		{
			//PE: Only allow this if imgui pointer is Arrow.
			bool ImGui_ImplWin32_IsCursorArrow();
			if (ImGui_ImplWin32_IsCursorArrow()) {
				//PE: Allow windows to control cursor, if we have no special imgui cursor.
				//PE: To enable cursors while "resizing" main window.
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			return(0);
		}
		break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
