#ifndef _BASE_WND_H_
#define _BASE_WND_H_

#include <cmp.h>

typedef struct _BaseWindow BaseWindow;

typedef void(*SetParentFunc)(BaseWindow* _this, HWND hWndParent);
typedef void(*SetIdFunc)(BaseWindow* _this, void* id);
typedef BOOL (*CreateFunc)(BaseWindow* _this);
typedef LRESULT (*HandleMessageFunc) (BaseWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct _BaseWindow
{
	Component _cmp;

	HWND _hWnd;
	HWND _hWndParent;
	HMENU _id;

	SetParentFunc _SetParentFunc;
	SetIdFunc _SetIdFunc;
	CreateFunc _CreateFunc;
	HandleMessageFunc _HandleMessageFunc;
} BaseWindow;

LRESULT CALLBACK BaseWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void BaseWindow_default(BaseWindow* _this);

void ShowError(const wchar_t* lpszFunction);

#endif /* _BASE_WND_H_*/

