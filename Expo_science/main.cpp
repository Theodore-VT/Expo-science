

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 


#include <Windows.h>
#include <shlwapi.h>
#include <windowsx.h>
#include <Commctrl.h>
#include <vector>
#include "Grid.hpp"
#include "guicon.hpp"
#include "main.h"

#include "Algorithm.hpp"
#include "Dummy.hpp"
#include "Recursive_backtracker.hpp"
#include "Recursive_division.hpp"
#include "A_Star.hpp"
#include "Stay_left.hpp"

#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define IDC_EXIT_BUTTON 51

char New_width[10], New_height[10];

// Background color
#define B_R_ 0
#define B_G_ 100
#define B_B_ 0

#define W_WIDTH  800
#define W_HEIGHT 880

#define NODES_WIDTH 15
#define NODES_HEIGHT 15

HINSTANCE hInst;
HWND Window_handle;

PAINTSTRUCT ps;
Grid grid(NODES_WIDTH, NODES_HEIGHT, W_WIDTH, W_HEIGHT, &ps);
int StopDrawing = false;
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK Size_Change(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	RedirectIOToConsole();
	
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
#endif

	grid.Load(std::string(lpCmdLine).c_str()); // Might be an overly complicated typecast

	hInst = hInstance;
	WNDCLASSEX wc;
	MSG msg;
	const wchar_t ClassName[] = L"Main_Window";

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = MainWindow;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = CreateSolidBrush(RGB(B_R_, B_G_, B_B_));
	wc.lpszMenuName = MAKEINTRESOURCEW(IDC_EXPO_SCIENCE);
	wc.lpszClassName = ClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
		exit(EXIT_FAILURE);
	}

	RECT wr = { 0, 0, W_WIDTH, W_HEIGHT };    // set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, TRUE);    // adjust the size

	Window_handle = CreateWindowEx(WS_EX_CLIENTEDGE, ClassName, L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);

	if (Window_handle == NULL)
	{
		MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
		exit(EXIT_FAILURE);
	}

	//HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hInst, NULL);

	SetTimer(Window_handle,	NULL, 15, NULL);

	ShowWindow(Window_handle, nCmdShow);
	UpdateWindow(Window_handle);
	
	SendMessage(Window_handle, WM_CREATE, NULL, NULL);
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.message;
}

LRESULT CALLBACK MainWindow(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_MOUSEMOVE || msg == WM_LBUTTONDOWN)
	{
		bool Overide_No_blinking_security = !(msg == WM_MOUSEMOVE);
		if (wParam & MK_LBUTTON)
		{
			int x = GET_X_LPARAM(lParam);
			int y = GET_Y_LPARAM(lParam);
			//OutputDebugStringA(std::string(std::to_string(grid.GetInd_from_2dPos(x, y)) + "\n").c_str());
			int ind = grid.GetInd_from_2dPos(x, y);
			
			if (ind != -1)
				grid.Notify_wall_shift(ind, Overide_No_blinking_security);
		}
	}

	if (StopDrawing && msg == WM_EXITSIZEMOVE)
	{
		StopDrawing = false;
		//SendMessage(hwnd, WM_CREATE, wParam, lParam);
	}
	if (msg == WM_MOVING)
	{
		StopDrawing++;
	}

	if (!StopDrawing && msg == WM_TIMER)// && msg == WM_RBUTTONDOWN)//*/
	{
		grid.Update();
		grid.Show(hwnd);
	}

	switch (msg)
	{
	case WM_CREATE:
	{
		grid.Show(hwnd, true);
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_EXIT_BUTTON:
		{
			SendMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hwnd);
			break;
		case IDM_FILE_SIZE_CHANGE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SIZE_CHANGE), hwnd, Size_Change);
			break;
		case ID_IMPORT_MAZE:
		{
			OPENFILENAMEA ofn = { 0 };
			char Buffer[300];
			std::fill(Buffer, Buffer + 300, '\0');
			ofn.lStructSize = sizeof(OPENFILENAMEA);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = Buffer;
			ofn.nMaxFile = 300;
			ofn.Flags = OFN_EXPLORER;
			ofn.lpstrFilter = NULL;
			ofn.lpstrCustomFilter = NULL;
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.lpstrInitialDir = NULL;
			ofn.lpstrTitle = "Import maze!";
			GetOpenFileNameA(&ofn);
			grid.Load(Buffer);
			SendMessage(hwnd, WM_CREATE, wParam, lParam);
		}
			break;
		case ID_EXPORT_MAZE:
		{
			OPENFILENAMEA ofn = { 0 };
			char Buffer[300];
			std::fill(Buffer, Buffer + 300, '\0');
			ofn.lStructSize = sizeof(OPENFILENAMEA);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = Buffer;
			ofn.nMaxFile = 300;
			ofn.Flags = OFN_EXPLORER;
			ofn.lpstrFilter = NULL;
			ofn.lpstrCustomFilter = NULL;
			ofn.nFilterIndex = 0;
			ofn.lpstrFileTitle = NULL;
			ofn.lpstrInitialDir = NULL;
			ofn.lpstrTitle = "Export maze!";
			GetSaveFileNameA(&ofn);
			OutputDebugStringA(Buffer);
			grid.Save(Buffer);
		}
		break;
		case ID_FILE_CLEAR:
		{
			grid.Reset();
			SendMessage(hwnd, WM_CREATE, wParam, lParam);
		}
		break;
		case IDM_MAZE_RECUSIVEDIVISION:
		{
			SendMessage(hwnd, ID_FILE_CLEAR, wParam, lParam);
			grid.Add_algorithm_to_queue(new Recursive_division(1, 1, grid.Width_nd(), grid.Height_nd()));
		}
		break;
		case IDM_MAZE_RECURSIVEBACKTRACKER:
		{
			SendMessage(hwnd, ID_FILE_CLEAR, wParam, lParam);
			grid.Add_algorithm_to_queue(new Recursive_backtracker(1, 1, grid.Width_nd(), grid.Height_nd()));
		}
		break;
		case IDM_RESOLVE_A_STAR:
		{
			grid.AddPath(0);
			grid.Add_algorithm_to_queue(new A_Star(1, 1, grid.Width_nd(), grid.Height_nd(), grid.GetPath(LAST_PATH)));
			grid.Add_algortihm_notifier(grid.GetAlgorithmID(LAST_ELEM), 2);
		}
		break;
		case IDM_RESOLVE_TURN_RIGHT:
		{
			grid.AddPath(0);
			grid.Add_algorithm_to_queue(new Stay_left(1, 1, grid.Width_nd(), grid.Height_nd(), grid.GetPath(LAST_PATH)));
			grid.Add_algortihm_notifier(grid.GetAlgorithmID(LAST_ELEM), 2);
		}
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
	break;
	case WM_CTLCOLORBTN: //In order to make those edges invisble when we use RoundRect(),
	{                //we make the color of our button's background match window's background
		return (LRESULT)GetSysColorBrush(COLOR_WINDOW + 1);
	}
	break;
	case WM_CLOSE:
	{
		DestroyWindow(hwnd);
		return 0;
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{	
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
		print(std::to_string(LOWORD(wParam)) + ",  " + std::to_string(message) + "\n");
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Size_Change(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);



	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemTextA(hDlg, IDC_EDIT_WIDTH, "15");
		SetDlgItemTextA(hDlg, IDC_EDIT_HEIGHT, "15");
		
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));

			GetDlgItemTextA(hDlg, IDC_EDIT_HEIGHT, &New_height[0], 5);
			GetDlgItemTextA(hDlg, IDC_EDIT_WIDTH, &New_width[0], 5);

			int Width = std::atoi(New_width);
			int Height = std::atoi(New_height);
			
			OutputDebugStringA(std::string(std::to_string(grid.Width_px()) + ",  " + std::to_string(grid.Height_px()) + "\n").c_str());
			grid.Reset(Width, Height, grid.Width_px(), grid.Height_px());
			grid.Show(Window_handle, true);
			return (INT_PTR)TRUE;
		}
		else if(LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));

			return (INT_PTR)TRUE;
		}

		break;
	}
	return (INT_PTR)FALSE;
}