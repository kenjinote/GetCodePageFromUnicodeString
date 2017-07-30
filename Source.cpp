#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <Mlang.h>
#include <locale.h>
#include <initializer_list>

TCHAR szClassName[] = TEXT("Window");

BOOL CheckCodePage(LPCWSTR lpszStringW, int nCodePage)
{
	int nLength = WideCharToMultiByte(nCodePage, 0, lpszStringW, -1, 0, 0, 0, 0);
	LPSTR lpszStringA = (LPSTR)GlobalAlloc(GPTR, nLength);
	WideCharToMultiByte(nCodePage, 0, lpszStringW, -1, lpszStringA, nLength, 0, 0);
	nLength = MultiByteToWideChar(nCodePage, 0, lpszStringA, -1, 0, 0);
	LPWSTR lpszNewStringW = (LPWSTR)GlobalAlloc(GPTR, nLength * sizeof(WCHAR));
	MultiByteToWideChar(nCodePage, 0, lpszStringA, -1, lpszNewStringW, nLength);
	BOOL bReturnValue = (lstrcmpW(lpszStringW, lpszNewStringW) == 0);
	GlobalFree(lpszNewStringW);
	GlobalFree(lpszStringA);
	return bReturnValue;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HFONT hFont;
	static HWND hCombo;
	static HWND hEdit;
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hFont = CreateFont(28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("メイリオ"));
		hCombo = CreateWindow(L"COMBOBOX", 0, WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hCombo, WM_SETFONT, (WPARAM)hFont, 0);
		SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("こんにちは"));
		SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("안녕하세요"));
		SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)TEXT("您好"));
		SendMessage(hCombo, CB_SETCURSEL, 0, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("コードページ"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit = CreateWindow(TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_READONLY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, 0);
		break;
	case WM_SIZE:
		MoveWindow(hCombo, 10, 10, LOWORD(lParam) - 20, 32, TRUE);
		MoveWindow(hButton, 10, 50, 256, 32, TRUE);
		MoveWindow(hEdit, 10, 90, LOWORD(lParam) - 20, HIWORD(lParam) - 100, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			SetWindowText(hEdit, 0);
			const int nSize = GetWindowTextLengthW(hCombo);
			LPWSTR lpszTextW = (LPWSTR)GlobalAlloc(GPTR, (nSize + 1) * sizeof(WCHAR));
			GetWindowTextW(hCombo, lpszTextW, nSize + 1);
			WCHAR szText[1024];
			for (auto nCodePage : { 932, 949, 936 })
			{
				wsprintfW(szText, TEXT("CheckCodePage(\"%s\", %d) → %s\r\n"), lpszTextW, nCodePage, CheckCodePage(lpszTextW, nCodePage) ? TEXT("TRUE") : TEXT("FALSE"));
				SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)szText);
			}
			GlobalFree(lpszTextW);
		}
		break;
	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("入力された文字列のコードページをチェック"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
