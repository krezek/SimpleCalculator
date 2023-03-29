#include "platform.h"

#include <base_wnd.h>

void BaseWindow_SetParent(BaseWindow* _this, HWND hWndParent)
{
	_this->_hWndParent = hWndParent;
}

void BaseWindow_SetId(BaseWindow* _this, void* id)
{
	_this->_id = id;
}

LRESULT CALLBACK BaseWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BaseWindow* pThis;

	if (uMsg == WM_NCCREATE)
	{
		CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
		pThis = (BaseWindow*)pCreate->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

		pThis->_hWnd = hWnd;
	}
	else
	{
		pThis = (BaseWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	if (pThis)
	{
		return pThis->_HandleMessageFunc(pThis, uMsg, wParam, lParam);
	}
	else
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

BOOL BaseWindow_OnSizing(BaseWindow* _this, RECT* pRect)
{
	LONG nWidth = pRect->right - pRect->left;
	LONG nHeight = pRect->bottom - pRect->top;

	if ((nWidth < _this->_cmp._minWidth) || (nHeight < _this->_cmp._minHeight))
	{
		pRect->right = max(pRect->right, pRect->left + _this->_cmp._minWidth);
		pRect->bottom = max(pRect->bottom, pRect->top + _this->_cmp._minHeight);
	}

	return TRUE;
}

void BaseWindow_default(BaseWindow* _this)
{
	Component_default((Component*)_this);

	_this->_cmp._type = CMP_WINDOW;

	_this->_cmp._minWidth = 0;
	_this->_cmp._minHeight = 0;

	_this->_SetParentFunc = BaseWindow_SetParent;
	_this->_SetIdFunc = BaseWindow_SetId;

	_this->_CreateFunc = NULL;
	_this->_HandleMessageFunc = NULL;
}

void RedirectIOToConsole()
{
	int hConHandle;
	HANDLE hStdHandle;
	FILE* fp;

	AllocConsole();

	hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle((intptr_t)hStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	freopen_s(&fp, "CONOUT$", "w", stdout);

	hStdHandle = GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle((intptr_t)hStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "r");
	freopen_s(&fp, "CONIN$", "r", stdin);

	hStdHandle = GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle((intptr_t)hStdHandle, _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	freopen_s(&fp, "CONOUT$", "w", stderr);
}

void ShowError(const wchar_t* lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}
