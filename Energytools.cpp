#include "stdafx.h"
#include "Energytools.h"
#include "process.h"    /* _beginthread, _endthread */
//#include "IOAccess.h"
#include <WinUser.h>
#include <windows.h>
#include <PowrProf.h>
#pragma comment(lib, "PowrProf.lib")

#define MAX_LOADSTRING 100
#define	WM_USER_SHELLICON WM_USER + 1

// Global Variables:
HINSTANCE hInst;	// current instance
HWND hWnd;
HWND _hWnd;
NOTIFYICONDATA nidApp;
HMENU hPopMenu;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szApplicationToolTip[MAX_LOADSTRING];	    // the main window class name
static const char *g_AppName = "Energytools";

void RunTest(TCHAR *AppName, TCHAR *CmdLine)
{
	ShellExecute(NULL, "runas", AppName, CmdLine, NULL, SW_HIDE);
}

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void MyThread(void *dummy);
void MyNagScreen(void *dummy);
void WinOff(void *dummy);
LRESULT CALLBACK	WndProc2(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ENERGYTOOLS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process. 
	OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
		&tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
		(PTOKEN_PRIVILEGES)NULL, 0);

	//_beginthread(MyThread, 0, (void *)(0));
	_beginthread(MyNagScreen, 0, (void *)(0));

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENERGYTOOLS));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

void WinOff(void *param)
{
	Sleep(1000 * 30 * 1 * 2L);
	//SetThreadExecutionState(ES_CONTINUOUS);
	// Shut down the system and force all applications to close. 
	ExitWindowsEx(EWX_POWEROFF | EWX_FORCEIFHUNG, 0);
	//Sleep(1000 * 60 * 1 * 2L);
	//SetThreadExecutionState(ES_CONTINUOUS);
	//Sleep(1000 * 60 * 1 * 2L);
	//ExitWindowsEx(EWX_POWEROFF | EWX_FORCEIFHUNG, 0);
}

void MyNagScreen(void *param)
{
	//int i = *(int *)param;    
	while (1) 
	{   
		/* Wait one second between loops. */
		Sleep( 1000*60*25*2L );
		DialogBox(hInst, MAKEINTRESOURCE(IDD_DONATION), hWnd, About); 
	}	
}

void MyThread(void *param)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc2;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = g_AppName;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wcex);

	_hWnd = CreateWindow(g_AppName, g_AppName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL, NULL, GetModuleHandle(NULL), NULL);

	ShowWindow(_hWnd, SW_HIDE);
	UpdateWindow(_hWnd);

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_QUIT)
		{
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

/*
This function is executed if the system sends a power event.
Parameters wParam and lParam define the type of event:

lParam: always 0
wParam:
PBT_APMQUERYSUSPEND             0x0000
PBT_APMQUERYSTANDBY             0x0001

PBT_APMQUERYSUSPENDFAILED       0x0002
PBT_APMQUERYSTANDBYFAILED       0x0003

PBT_APMSUSPEND                  0x0004
PBT_APMSTANDBY                  0x0005

PBT_APMRESUMECRITICAL           0x0006
PBT_APMRESUMESUSPEND            0x0007
PBT_APMRESUMESTANDBY            0x0008

PBTF_APMRESUMEFROMFAILURE       0x00000001

PBT_APMBATTERYLOW               0x0009
PBT_APMPOWERSTATUSCHANGE        0x000A

PBT_APMOEMEVENT                 0x000B
PBT_APMRESUMEAUTOMATIC          0x0012

Source: http://weblogs.asp.net/ralfw/archive/2003/09/09/26908.aspx
*/

static LRESULT WINAPI WndProc2(HWND _hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		//case WM_QUERYENDSESSION:
		//case WM_ENDSESSION:
		case WM_POWERBROADCAST:
		{
			//power management code here
			if (LOWORD(wParam) == 4 || LOWORD(wParam) == 5)
			{
				// hibernate now 
				//SetSuspendState(true, false, FALSE);

				//SetSuspendState(FALSE, TRUE, FALSE);
				// Shut down the system and force all applications to close. 
				ExitWindowsEx(EWX_POWEROFF | EWX_FORCEIFHUNG, 0);
			}
		}
	}
	return DefWindowProc(_hWnd, uMsg, wParam, lParam);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ENERGYTOOLS));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_ENERGYTOOLS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassEx(&wcex);
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
	//HWND hWnd;
	HICON hMainIcon;

	hInst = hInstance; // Store instance handle in our global variable
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	hMainIcon = LoadIcon(hInstance, (LPCTSTR)MAKEINTRESOURCE(IDI_ENERGYTOOLS));
	nidApp.cbSize = sizeof(NOTIFYICONDATA); // sizeof the struct in bytes 
	nidApp.hWnd = (HWND)hWnd;              //handle of the window which will process this app. messages 
	nidApp.uID = IDI_ENERGYTOOLS;           //ID of the icon that will appear in the system tray 
	nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP; //ORing of all the flags 
	nidApp.hIcon = hMainIcon; // handle of the Icon to be displayed, obtained from LoadIcon 
	nidApp.uCallbackMessage = WM_USER_SHELLICON;
	LoadString(hInstance, IDS_APPTOOLTIP, nidApp.szTip, MAX_LOADSTRING);
	Shell_NotifyIcon(NIM_ADD, &nidApp);
	return TRUE;
}

void Init()
{
	// user defined message that will be sent as the notification message to the Window Procedure 
}
//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	POINT lpClickPoint;
	MENUITEMINFO mii = { sizeof(MENUITEMINFO) };
	//BOOL status;

	switch (message)
	{
		
		//case WM_ENDSESSION:
		//case WM_QUERYENDSESSION:
		case WM_POWERBROADCAST:

			if (LOWORD(wParam) == 4 || LOWORD(wParam) == 5)
			{
				SetSuspendState(true, FALSE, FALSE);
				// Shut down the system and force all applications to close. 
				//ExitWindowsEx(EWX_POWEROFF | EWX_FORCEIFHUNG, 0);
				//SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
				//SetThreadExecutionState(ES_SYSTEM_REQUIRED);
				//SetSuspendState(false, false, FALSE);
				//_beginthread(WinOff, 0, (void *)(0));
				// send the deny message
				//return BROADCAST_QUERY_DENY;
				//ExitWindowsEx(EWX_POWEROFF | EWX_FORCEIFHUNG, 0);
			}
			break;
		
		case WM_USER_SHELLICON:

			// systray msg callback 
			switch (LOWORD(lParam))
			{
				case WM_RBUTTONDOWN:

					UINT uFlag = MF_BYPOSITION | MF_UNCHECKED | MF_STRING;
					GetCursorPos(&lpClickPoint);
					hPopMenu = CreatePopupMenu();

					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_ABOUT, _T("About"));
					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_SEPARATOR, IDM_SEP, _T("SEP"));
					InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("Exit"));

					//hMenu = GetMenu(hWnd);
					//mii.fMask = MIIM_STATE;
					//status = GetMenuItemInfo(hPopMenu, oldmenu, FALSE, &mii);
					//mii.fState ^= MFS_CHECKED;
					//status = SetMenuItemInfo(hPopMenu, oldmenu, FALSE, &mii);

					SetForegroundWindow(hWnd);
					TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);
					return TRUE;
				}
				break;

		case WM_COMMAND:

			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);

			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
					break;

				case IDM_EXIT:
					Shell_NotifyIcon(NIM_DELETE, &nidApp);
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
					break;
				}
				break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
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