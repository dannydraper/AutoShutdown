#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <Powrprof.h>
#include "resource.h"

// Constant definitions
#define SIZE_LARGESTRING	2048
#define SIZE_STRING	1024
#define SIZE_NAME		64

#define ACTION_STANDBY	100
#define ACTION_HIBERNATE	101
#define ACTION_SHUTDOWN	102

#define IDC_BTNCANCEL 1000
#define IDC_TIMERLABEL 1001
#define IDC_TIMERCOUNT 1002
#define IDT_COUNTDOWN 1003
#define ID_OPTCOUNTDOWN	1004
#define ID_OPTTIME	1005
#define ID_COUNTDOWNLABEL	1006
#define ID_TIMELABEL	1007
#define ID_COUNTDOWNSELECT		1008
#define ID_TIMESELECT		1009
#define ID_STATIC			1010
#define ID_OPTSHUTDOWN	1011
#define ID_OPTSTANDBY		1012
#define ID_OPTHIBERNATE		1013
#define ID_ACTIVE				1014

// Global variables
HWND g_hwndCancelbutton;
HWND g_hwndTimerlabel;
HWND g_hwndTimercount;
HWND g_hwndoptcountdown;
HWND g_hwndopttime;
HWND g_hwndcountdownlabel;
HWND g_hwndtimelabel;
HWND g_countdownselect;
HWND g_timeselect;
HWND g_static;
HWND g_hwndactive;

HWND g_optshutdown;
HWND g_optstandby;
HWND g_opthibernate;
HWND g_hwnd;

int g_iHours = 1;
int g_iMinutes = 59;
int g_iSeconds = 59;

HBITMAP g_hbmBanner;

bool g_busecountdown = true;
int g_action = ACTION_SHUTDOWN;
bool g_active = false;


// Prototypes

void Activate ();
void Deactivate ();

// Implementation


BOOL MySystemShutdown()
{
	if (g_action == ACTION_SHUTDOWN) {
		HANDLE hToken; 
		TOKEN_PRIVILEGES tkp; 
		
		// Get a token for this process. 
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 
		
		// Get the LUID for the shutdown privilege. 
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,  &tkp.Privileges[0].Luid); 
		
		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
		
		// Get the shutdown privilege for this process. 
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
		
		if (GetLastError() != ERROR_SUCCESS) 
		return FALSE; 
		
		// Shut down and poweroff the system
		if (!ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, 0))
		return FALSE; 
	}
	
	if (g_action == ACTION_STANDBY) {
		SetSuspendState (FALSE, FALSE, FALSE);
	}

	if (g_action == ACTION_HIBERNATE) {
		SetSuspendState (TRUE, FALSE, FALSE);
	}

	return TRUE;
}

void SetTimerOptions ()
{
	if (g_busecountdown == true) {	
		ShowWindow (g_hwndtimelabel, SW_HIDE);
		ShowWindow (g_hwndcountdownlabel, SW_SHOW);
		ShowWindow (g_countdownselect, SW_SHOW);
		ShowWindow (g_timeselect, SW_HIDE);
		
	} else {		
		ShowWindow (g_hwndtimelabel, SW_SHOW);
		ShowWindow (g_hwndcountdownlabel, SW_HIDE);
		ShowWindow (g_countdownselect, SW_HIDE);
		ShowWindow (g_timeselect, SW_SHOW);
	}
}

void LoadSettings()
{
	g_busecountdown = true;
	SendMessage (g_hwndoptcountdown, BM_SETCHECK, 1, 0);
	SendMessage (g_hwndopttime, BM_SETCHECK, 0, 0);

	SendMessage (g_optstandby, BM_SETCHECK, 0, 0);
	SendMessage (g_opthibernate, BM_SETCHECK, 0, 0);
	SendMessage (g_optshutdown, BM_SETCHECK, 1, 0);
	g_action = ACTION_SHUTDOWN;
	Deactivate ();

}

void Getdatapart (char *szdataline, int idatapart, char *szsepchar, char *szresult)
{
	char sztempchar[8];
	ZeroMemory (sztempchar, 8);
	
	char szfinalstring[SIZE_LARGESTRING];
	ZeroMemory (szfinalstring, SIZE_LARGESTRING);

	char sztdataline[SIZE_LARGESTRING];
	ZeroMemory (sztdataline, SIZE_LARGESTRING);
	
	strcpy (sztdataline, szdataline);

	int datalen = 0;
	int c = 0;
	int sepcount = 0;
	
	strcat (sztdataline, szsepchar);
	datalen = strlen (sztdataline);

	for (c=0;c<datalen;c++) {
		strncpy (sztempchar, sztdataline+c, 1);
		if (strcmp (sztempchar, szsepchar) == 0) {
			sepcount++;
			if (sepcount == idatapart) {
				strcpy (szresult, szfinalstring);
				return;
			} else {
				ZeroMemory (szfinalstring, SIZE_LARGESTRING);
			}
		} else {
			strcat (szfinalstring, sztempchar);
		}
	}
	strcpy (szresult, szfinalstring);
}

void PopulateCountdown ()
{
	char szHour[SIZE_NAME];

	char szFulltime[SIZE_NAME];

	for (int h=0;h<24;h++)
	{
		ZeroMemory (szHour, SIZE_NAME);
		itoa (h, szHour, 10);

		if (h != 0) {
			ZeroMemory (szFulltime, SIZE_NAME);
			strcpy_s (szFulltime, SIZE_NAME, szHour);
			strcat_s (szFulltime, SIZE_NAME, ":00:00");
			SendMessage (g_countdownselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);	
		}

		ZeroMemory (szFulltime, SIZE_NAME);
		strcpy_s (szFulltime, SIZE_NAME, szHour);
		strcat_s (szFulltime, SIZE_NAME, ":10:00");
		SendMessage (g_countdownselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);	

		ZeroMemory (szFulltime, SIZE_NAME);
		strcpy_s (szFulltime, SIZE_NAME, szHour);
		strcat_s (szFulltime, SIZE_NAME, ":20:00");
		SendMessage (g_countdownselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);	

		ZeroMemory (szFulltime, SIZE_NAME);
		strcpy_s (szFulltime, SIZE_NAME, szHour);
		strcat_s (szFulltime, SIZE_NAME, ":30:00");
		SendMessage (g_countdownselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);	

		ZeroMemory (szFulltime, SIZE_NAME);
		strcpy_s (szFulltime, SIZE_NAME, szHour);
		strcat_s (szFulltime, SIZE_NAME, ":40:00");
		SendMessage (g_countdownselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);	

		ZeroMemory (szFulltime, SIZE_NAME);
		strcpy_s (szFulltime, SIZE_NAME, szHour);
		strcat_s (szFulltime, SIZE_NAME, ":50:00");
		SendMessage (g_countdownselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);		
	}

	SendMessage (g_countdownselect, CB_SETCURSEL, 0, 0); // Set the current selection to the first item in the list
}

void PopulateTimes ()
{
	char szHour[SIZE_NAME];

	char szFulltime[SIZE_NAME];

	for (int h=0;h<24;h++)
	{
		ZeroMemory (szHour, SIZE_NAME);
		itoa (h, szHour, 10);
	
		ZeroMemory (szFulltime, SIZE_NAME);
		strcpy_s (szFulltime, SIZE_NAME, szHour);
		strcat_s (szFulltime, SIZE_NAME, ":00:00");
		SendMessage (g_timeselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);	
	

		ZeroMemory (szFulltime, SIZE_NAME);
		strcpy_s (szFulltime, SIZE_NAME, szHour);
		strcat_s (szFulltime, SIZE_NAME, ":15:00");
		SendMessage (g_timeselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);	

		ZeroMemory (szFulltime, SIZE_NAME);
		strcpy_s (szFulltime, SIZE_NAME, szHour);
		strcat_s (szFulltime, SIZE_NAME, ":30:00");
		SendMessage (g_timeselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);	

		ZeroMemory (szFulltime, SIZE_NAME);
		strcpy_s (szFulltime, SIZE_NAME, szHour);
		strcat_s (szFulltime, SIZE_NAME, ":45:00");
		SendMessage (g_timeselect, CB_ADDSTRING, 0, (LPARAM) (LPCTSTR) szFulltime);	
		
	}

	SendMessage (g_timeselect, CB_SETCURSEL, 0, 0); // Set the current selection to the first item in the list
}

void Activate ()
{
	char szCountdown[SIZE_NAME];
	ZeroMemory (szCountdown, SIZE_NAME);

	if (g_busecountdown == true) {

		//g_hwndTimerlabel
		ShowWindow (g_hwndTimerlabel, SW_SHOW);
		ShowWindow (g_hwndTimercount, SW_SHOW);


		SetDlgItemText (g_hwnd, IDC_TIMERLABEL, "Countdown: ");

		GetDlgItemText (g_hwnd, ID_COUNTDOWNSELECT, szCountdown, SIZE_NAME);

	} else {
		ShowWindow (g_hwndTimerlabel, SW_SHOW);
		ShowWindow (g_hwndTimercount, SW_SHOW);

		GetDlgItemText (g_hwnd, ID_TIMESELECT, szCountdown, SIZE_NAME);

		SetDlgItemText (g_hwnd, IDC_TIMERLABEL, "Current time: ");
	}


	char szHour[SIZE_NAME];
	ZeroMemory (szHour, SIZE_NAME);

	char szMinute[SIZE_NAME];
	ZeroMemory (szMinute, SIZE_NAME);

	char szSecond[SIZE_NAME];
	ZeroMemory (szSecond, SIZE_NAME);

	Getdatapart (szCountdown, 1, ":", szHour);
	Getdatapart (szCountdown, 2, ":", szMinute);
	Getdatapart (szCountdown, 3, ":", szSecond);


	g_iHours = atoi (szHour);
	g_iMinutes =atoi (szMinute);
	g_iSeconds = atoi (szSecond);


	//g_hwndTimercount
	SetTimer (g_hwnd, IDT_COUNTDOWN, 1000, NULL);


}

void Deactivate ()
{
	ShowWindow (g_hwndTimerlabel, SW_HIDE);
	ShowWindow (g_hwndTimercount, SW_HIDE);
	KillTimer (g_hwnd, IDT_COUNTDOWN);
}

LRESULT WINAPI WndProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch( msg )
    {
		case WM_CREATE:
		{
			g_hwnd = hWnd;

			g_hbmBanner = LoadBitmap (GetModuleHandle (NULL), MAKEINTRESOURCE (IDI_BANNER));
			if (g_hbmBanner == NULL) {
				MessageBox (hWnd, "Could not load IDI_BANNER resource!", "Error", MB_OK | MB_ICONEXCLAMATION);
			}

			HFONT hfDefault;
			hfDefault = (HFONT) GetStockObject (DEFAULT_GUI_FONT);

			g_static = CreateWindow ("button", "Timer Mode", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, 60, 409, 55, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
			SendMessage (g_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_static = CreateWindow ("button", "Set the time / countdown", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, 124, 409, 55, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
			SendMessage (g_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_static = CreateWindow ("button", "Action", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 12, 190, 409, 55, hWnd, (HMENU) ID_STATIC, GetModuleHandle (NULL), NULL);
			SendMessage (g_static, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));



			g_hwndoptcountdown = CreateWindow ("button", "When the countdown expires", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 40, 83, 160, 16, hWnd, (HMENU) ID_OPTCOUNTDOWN, GetModuleHandle (NULL), NULL);
			SendMessage (g_hwndoptcountdown, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_hwndopttime = CreateWindow ("button", "At the specified time", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 230, 83, 140, 16, hWnd, (HMENU) ID_OPTTIME, GetModuleHandle (NULL), NULL);
			SendMessage (g_hwndopttime, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));



			g_optshutdown = CreateWindow ("button", "Shutdown", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 70, 212, 80, 16, hWnd, (HMENU) ID_OPTSHUTDOWN, GetModuleHandle (NULL), NULL);
			SendMessage (g_optshutdown, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_optstandby = CreateWindow ("button", "Standby", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 170, 212, 80, 16, hWnd, (HMENU) ID_OPTSTANDBY, GetModuleHandle (NULL), NULL);
			SendMessage (g_optstandby, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_opthibernate = CreateWindow ("button", "Hibernate", WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON, 260, 212, 80, 16, hWnd, (HMENU) ID_OPTHIBERNATE, GetModuleHandle (NULL), NULL);
			SendMessage (g_opthibernate, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));


			g_hwndcountdownlabel = CreateWindow ("static", "After the specified amount of time:", WS_CHILD | WS_VISIBLE, 43, 151, 220, 15, hWnd, (HMENU) ID_COUNTDOWNLABEL, GetModuleHandle (NULL), NULL);
			SendMessage (g_hwndcountdownlabel, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_hwndtimelabel = CreateWindow ("static", "At the specified time:", WS_CHILD | WS_VISIBLE, 43, 151, 220, 15, hWnd, (HMENU) ID_TIMELABEL, GetModuleHandle (NULL), NULL);
			SendMessage (g_hwndtimelabel, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_countdownselect = CreateWindow ("ComboBox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | CBS_DROPDOWN, 220, 147, 100, 19, hWnd, (HMENU) ID_COUNTDOWNSELECT, GetModuleHandle (NULL), NULL);
			SendMessage (g_countdownselect, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_timeselect = CreateWindow ("ComboBox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | CBS_DROPDOWN, 150, 147, 100, 19, hWnd, (HMENU) ID_TIMESELECT, GetModuleHandle (NULL), NULL);
			SendMessage (g_timeselect, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));


			g_hwndactive = CreateWindow ("button", "Enabled", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 12, 255, 90, 19, hWnd, (HMENU) ID_ACTIVE, GetModuleHandle (NULL), NULL);
			SendMessage (g_hwndactive, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));


			//g_hwndCancelbutton = CreateWindow ("button", "Abort Shutdown", WS_CHILD | WS_VISIBLE, 140, 170, 170, 40, hWnd, (HMENU) IDC_BTNCANCEL, GetModuleHandle (NULL), NULL);
			//SendMessage (g_hwndCancelbutton, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_hwndTimerlabel = CreateWindow ("static", "Time until Autoshutdown:", WS_CHILD | WS_VISIBLE, 125, 257, 220, 15, hWnd, (HMENU) IDC_TIMERLABEL, GetModuleHandle (NULL), NULL);
			SendMessage (g_hwndTimerlabel, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));

			g_hwndTimercount = CreateWindow ("static", "00:00:00", WS_CHILD | WS_VISIBLE, 210, 257, 240, 18, hWnd, (HMENU) IDC_TIMERCOUNT, GetModuleHandle (NULL), NULL);
			SendMessage (g_hwndTimercount, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM (FALSE, 0));
			

			PopulateCountdown ();
			PopulateTimes ();

			LoadSettings();
			SetTimerOptions ();
			
		}
		break;
		case WM_COMMAND:
		{
			switch (LOWORD (wParam))
			{
				case IDC_BTNCANCEL:
				{
					// Now quit the program.
					PostQuitMessage (0);
				}
				break;
				case ID_OPTCOUNTDOWN:
				{
					g_busecountdown = true;
					SendMessage (g_hwndoptcountdown, BM_SETCHECK, 1, 0);
					SendMessage (g_hwndopttime, BM_SETCHECK, 0, 0);

					SetTimerOptions ();
				}
				break;
				case ID_OPTTIME:
				{
					g_busecountdown = false;

					SendMessage (g_hwndoptcountdown, BM_SETCHECK, 0, 0);
					SendMessage (g_hwndopttime, BM_SETCHECK, 1, 0);

					SetTimerOptions ();
				}
				break;

				case ID_OPTSTANDBY:
				{
					SendMessage (g_optstandby, BM_SETCHECK, 1, 0);
					SendMessage (g_opthibernate, BM_SETCHECK, 0, 0);
					SendMessage (g_optshutdown, BM_SETCHECK, 0, 0);
					g_action = ACTION_STANDBY;
				}
				break;
				case ID_OPTHIBERNATE:
				{
					SendMessage (g_optstandby, BM_SETCHECK, 0, 0);
					SendMessage (g_opthibernate, BM_SETCHECK, 1, 0);
					SendMessage (g_optshutdown, BM_SETCHECK, 0, 0);
					g_action = ACTION_HIBERNATE;
				}
				break;
				case ID_OPTSHUTDOWN:
				{
					SendMessage (g_optstandby, BM_SETCHECK, 0, 0);
					SendMessage (g_opthibernate, BM_SETCHECK, 0, 0);
					SendMessage (g_optshutdown, BM_SETCHECK, 1, 0);
					g_action = ACTION_SHUTDOWN;
				}
				break;
				case ID_ACTIVE:
					{
						if (g_active == true) {
							g_active = false;
							SendMessage (g_hwndactive, BM_SETCHECK, 0, 0);
							Deactivate();
						} else {
							g_active = true;
							SendMessage (g_hwndactive, BM_SETCHECK, 1, 0);
							Activate();
						}																		
					}
					break;
			}
		}
		break;
		case WM_TIMER:
		{
			char szCountdown [1024];
			ZeroMemory (szCountdown, 1024);

			if (g_busecountdown == true) {
				// Shutting down on a countdown

				if (g_iHours == 0 && g_iMinutes == 0 && g_iSeconds == 0) {
					Deactivate ();
					//MessageBox (NULL, "Time up!", "Timer", MB_OK);
					MySystemShutdown ();
				}

				if (g_iSeconds == 0) {
					g_iSeconds=59;
					if (g_iMinutes == 0) {
						g_iMinutes = 59;
						g_iHours--;
					}
					g_iMinutes--;
				}
				g_iSeconds--;

				
				sprintf (szCountdown, "%d:%d:%d", g_iHours, g_iMinutes, g_iSeconds);
			} else {

				// Shutting down at a specific time
				SYSTEMTIME systemtime;
				GetLocalTime (&systemtime);
					
				int iHour = systemtime.wHour;
				int iMinute = systemtime.wMinute;
				

				if (iHour == g_iHours) {
					if (iMinute == g_iMinutes) {
						Deactivate();
						MySystemShutdown();
					}
				}

				sprintf (szCountdown, "%d:%d:%d, Specified Time: %d:%d:%d", iHour, iMinute, 0, g_iHours, g_iMinutes, 0);

			}
			
			
			SetDlgItemText (hWnd, IDC_TIMERCOUNT, szCountdown);
			
		}
		break;
		case WM_POWERBROADCAST:
		{				
			switch (wParam)
			{
				case PBT_APMRESUMESUSPEND:
				{
					if (g_active == true) {
						Activate ();
					}
				}
				break;
			}		
			return TRUE;
		}
		break;
		case WM_PAINT:
		{
			BITMAP bm;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint (hWnd, &ps);
			
			HDC hdcMem = CreateCompatibleDC (hdc);
			HBITMAP hbmOld = (HBITMAP) SelectObject (hdcMem, g_hbmBanner);

			GetObject (g_hbmBanner, sizeof (bm), &bm);
			BitBlt (hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject (hdcMem, hbmOld);
			DeleteDC (hdcMem);
			
			EndPaint (hWnd, &ps);
		}
		break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	InitCommonControls ();

	WNDCLASSEX wc;
	HWND hWnd;
	MSG msg;

	HICON hAppIcon = LoadIcon (GetModuleHandle (NULL), MAKEINTRESOURCE (IDI_ICON));

	wc.cbSize = sizeof (WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0L;
	wc.cbWndExtra = 0L;
	wc.hInstance = GetModuleHandle (NULL);
	wc.hIcon = hAppIcon;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "ASDWndClass";
	wc.hIconSm = hAppIcon;

	if (RegisterClassEx (&wc) == 0) {
		MessageBox (NULL, "Class registration failed!", "ASDWndClass", MB_OK);
		return -1;
	}
	
	hWnd = CreateWindowEx (WS_EX_WINDOWEDGE, "ASDWndClass", "Auto Shutdown", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 450, 325,  NULL, NULL, wc.hInstance, NULL);

	if (hWnd == NULL) {
		MessageBox (NULL, "Window creation failed!", "Auto Shutdown", MB_OK);
		return -2;
	}

    // Show the window
    ShowWindow(hWnd, SW_SHOWDEFAULT);
    UpdateWindow(hWnd);

    // Enter the message loop
    while( GetMessage(&msg, NULL, 0, 0 ))
    {
        TranslateMessage (&msg);
        DispatchMessage (&msg);
    }

	UnregisterClass("ASDWndClass", wc.hInstance);
	return 0;
}