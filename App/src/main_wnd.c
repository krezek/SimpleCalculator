#include "platform.h"

#include <main_wnd.h>
#include <winutil.h>
#include <ribbon.h>
#include <panel.h>

static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Simple Computer Algebra System");

LRESULT CALLBACK DefaultWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT HandleMessage(MainWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT OnCreate(MainWindow* mw);
LRESULT OnDestroy(MainWindow* mw);
LRESULT OnSize(MainWindow* mw);
LRESULT OnRibbonHeightChanged(MainWindow* mw);

ATOM MainWindow_RegisterClass()
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = DefaultWindow_Proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassEx(&wcex);
}

LRESULT CALLBACK DefaultWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow* pThis;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MainWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->_hWnd = hWnd;
    }
    else
    {
        pThis = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
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

BOOL MainWindow_Create(MainWindow* _this)
{
    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        _this
    );

    _this->_hWnd = hWnd;

    return _this->_hWnd ? TRUE : FALSE;
}

MainWindow* MainWindow_init()
{
    MainWindow* mw = (MainWindow*)malloc(sizeof(MainWindow));
    assert(mw != NULL);

    mw->_HandleMessageFunc = HandleMessage;

    mw->_panelList = PanelList_init();

    return mw;
}

void MainWindow_free(MainWindow* mw)
{
    PanelList_free(mw->_panelList);
    free(mw);
}

static LRESULT HandleMessage(MainWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        return OnCreate(_this);

    case WM_DESTROY:
        return OnDestroy(_this);

    case WM_SIZE:
        _this->_client_width = LOWORD(lParam);
        _this->_client_height = HIWORD(lParam);
        return OnSize(_this);

    case WM_RIBBON_HEIGHT_CHANGED:
        _this->_ribbon_height = (int)wParam;
        return OnRibbonHeightChanged(_this);

    default:
        return DefWindowProc(_this->_hWnd, uMsg, wParam, lParam);
    }
}

LRESULT OnCreate(MainWindow* mw)
{
    if (CreateRibbon(mw->_hWnd))
    {
        ShowError(_T("MainWindow::OnCreate::unable to create ribbon!"));
        return -1;
    }

    mw->_hWndStatusBar = CreateWindowEx(
        0,
        STATUSCLASSNAME,
        (PCTSTR)NULL,
        SBARS_SIZEGRIP |
        WS_CHILD | WS_VISIBLE,
        0, 0, 0, 0,
        mw->_hWnd,
        NULL,
        (HINSTANCE)GetWindowLongPtr(mw->_hWnd, GWLP_HINSTANCE),
        NULL);

    if (!mw->_hWndStatusBar)
    {
        ShowError(_T("MainWindow::OnCreate::Unable to Create Statusbar"));
        return -1;
    }

    // Get StatusBar height
    RECT rc;
    SendMessage(mw->_hWndStatusBar, WM_SIZE, 0, 0);
    GetWindowRect(mw->_hWndStatusBar, &rc);
    mw->_statusbar_height = rc.bottom - rc.top;

    mw->_hWndVScrollBar = CreateWindowEx(
        0,
        L"SCROLLBAR",
        (PTSTR)NULL,
        WS_CHILD | WS_VISIBLE | SBS_VERT,
        0,
        0,
        0,
        0,
        mw->_hWnd,
        (HMENU)NULL,
        (HINSTANCE)GetWindowLongPtr(mw->_hWnd, GWLP_HINSTANCE),
        (PVOID)NULL
    );

    if (!mw->_hWndVScrollBar)
    {
        ShowError(_T("MainWindow::OnCreate::Unable to Create V ScrollBar"));
        return -1;
    }

    mw->_hWndHScrollBar = CreateWindowEx(
        0,
        L"SCROLLBAR",
        (PTSTR)NULL,
        WS_CHILD | WS_VISIBLE | SBS_HORZ,
        0,
        0,
        0,
        0,
        mw->_hWnd,
        (HMENU)NULL,
        (HINSTANCE)GetWindowLongPtr(mw->_hWnd, GWLP_HINSTANCE),
        (PVOID)NULL
    );
    
    if (!mw->_hWndHScrollBar)
    {
        ShowError(_T("MainWindow::OnCreate::Unable to Create H ScrollBar"));
        return -1;
    }

    mw->_hWndCorner = CreateWindowEx(
        0,
        L"BUTTON",
        (PTSTR)NULL,
        WS_CHILD | WS_VISIBLE | WS_DISABLED,
        0,
        0,
        0,
        0,
        mw->_hWnd,
        (HMENU)NULL,
        (HINSTANCE)GetWindowLongPtr(mw->_hWnd, GWLP_HINSTANCE),
        (PVOID)NULL
    );

    if (!mw->_hWndCorner)
    {
        ShowError(_T("MainWindow::OnCreate::Unable to Create Corner Button"));
        return -1;
    }

    return 0;
}

LRESULT OnDestroy(MainWindow* mw)
{
    DestroyRibbon();
    PostQuitMessage(0);

    return 0;
}

LRESULT OnSize(MainWindow* mw)
{
    if (!IsWindowVisible(mw->_hWnd))
        return 0;

    printf("size changed (%d, %d)\n", mw->_client_width, mw->_client_height);

    return 0;
}

LRESULT OnRibbonHeightChanged(MainWindow* mw)
{
    printf("Ribbon height changed %d\n", mw->_ribbon_height);
    return 0;
}
