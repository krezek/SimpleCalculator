#include "platform.h"

#include <main_wnd.h>
#include <resource.h>
#include <winutil.h>
#include <ribbon.h>
#include <panel.h>
#include <editor.h>
#include <simplify.h>
#include <calc.h>

#define SCROLLBAR_WIDE 20
#define INITIAL_FONTSIZE 16
#define WM_SETFONTSIZE WM_USER + 1

static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Simple Computer Algebra System");

HFONT g_math_font;
HFONT g_fontList[5];

LRESULT CALLBACK DefaultWindow_Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT HandleMessage(MainWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT OnCreate(MainWindow* mw);
LRESULT OnDestroy(MainWindow* mw);
LRESULT OnMouseWheel(MainWindow* mw, WPARAM wParam);
LRESULT OnSetFontSize(MainWindow* mw, int fsize);
LRESULT OnSize(MainWindow* mw);
LRESULT OnPaint(MainWindow* mw);
LRESULT OnRibbonHeightChanged(MainWindow* mw);
void SetScrollbarInfo(MainWindow* mw);
LRESULT OnVScroll(MainWindow* mw, WPARAM wParam);
LRESULT OnHScroll(MainWindow* mw, WPARAM wParam);
LRESULT OnMousLButtonDown(MainWindow* mw, int x, int y);
LRESULT OnSetFocus(MainWindow* mw);
LRESULT OnKillFocus(MainWindow* mw);
LRESULT OnKeyDown(MainWindow* mw, WPARAM wParam, LPARAM lParam);
LRESULT OnChar(MainWindow* mw, WPARAM wParam, LPARAM lParam);
void Simplify(MainWindow* mw);
void Calculate(MainWindow* mw);

void Graphics_fontList_init(HANDLE hFont);
void Graphics_fontList_free();

extern void parse_test(GList** ppGl, const wchar_t* s);

ATOM MainWindow_RegisterClass()
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DBLCLKS;
    wcex.lpfnWndProc = DefaultWindow_Proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetModuleHandle(NULL);
    wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SIMPLECAS));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SIMPLECAS));

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

    mw->_x_current_pos = mw->_y_current_pos = 0;

    mw->_panelList = PanelList_init();
    mw->_selected_panel = NULL;

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

    case WM_SETFONTSIZE:
        return OnSetFontSize(_this, (int)(wParam));

    case WM_RIBBON_HEIGHT_CHANGED:
        _this->_ribbon_height = (int)wParam;
        return OnRibbonHeightChanged(_this);

    case WM_PAINT:
        return OnPaint(_this);

    case WM_MOUSEWHEEL:
        return OnMouseWheel(_this, wParam);

    case WM_VSCROLL:
        return OnVScroll(_this, wParam);

    case WM_HSCROLL:
        return OnHScroll(_this, wParam);

    case WM_LBUTTONDOWN:
        return OnMousLButtonDown(_this, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    case WM_SETFOCUS:
        return OnSetFocus(_this);

    case WM_KILLFOCUS:
        return OnKillFocus(_this);

    case WM_KEYDOWN:
        return OnKeyDown(_this, wParam, lParam);

    case WM_CHAR:
    {
        static int x = 2;
        if (wParam == L'^')
        {
            --x;
            if (x) return 0;
            x = 2;
        }

        return OnChar(_this, wParam, lParam);
    }

    default:
        return DefWindowProc(_this->_hWnd, uMsg, wParam, lParam);
    }
}

LRESULT OnCreate(MainWindow* mw)
{
    // Create Font
    HDC hdc = GetDC(mw->_hWnd);
    int lfHeight = -MulDiv(INITIAL_FONTSIZE, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    g_math_font = CreateFont(lfHeight, 0, 0, 0, FALSE,
        FALSE, 0, 0, 0, 0, 0, 0, 0, L"Cambria");
    if (!g_math_font)
    {
        ShowError(_T("MainWindow::OnCreate::unable to create math font"));
        return -1;
    }

    ReleaseDC(mw->_hWnd, hdc);

    Graphics_fontList_init(g_math_font);

    // Create Ribbon
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

    PanelList_AddNewPanel(mw->_panelList, L"In:", L"Out:");
    PanelList_PropertyChangedEvent(mw->_panelList, mw->_hWnd, -mw->_x_current_pos,
        mw->_ribbon_height - mw->_y_current_pos);

    mw->_selected_panel = mw->_panelList->_front->_panel;

    return 0;
}

LRESULT OnDestroy(MainWindow* mw)
{
    Graphics_fontList_free();
    DeleteFont(g_math_font);
    DestroyRibbon();
    PostQuitMessage(0);

    return 0;
}

LRESULT OnMouseWheel(MainWindow* mw, WPARAM wParam)
{
    int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

    if (GetKeyState(VK_CONTROL) < 0)
    {
        static int fsize = INITIAL_FONTSIZE;

        if (zDelta < 0)
        {
            if (fsize > 12)
                --fsize;
            
            PostMessage(mw->_hWnd, WM_SETFONTSIZE, MAKEWPARAM(fsize, 0), 0);
        }
        else
        {
            if (fsize < 72)
                ++fsize;

            PostMessage(mw->_hWnd, WM_SETFONTSIZE, MAKEWPARAM(fsize, 0), 0);
        }
    }
    else
    {
        if (zDelta < 0)
            PostMessage(mw->_hWnd, WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), 0);
        else
            PostMessage(mw->_hWnd, WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), 0);
    }
    
    return 0;
}

LRESULT OnSetFontSize(MainWindow* mw, int fsize)
{
    if (g_math_font)
        DeleteFont(g_math_font);

    Graphics_fontList_free();

    // Create Font
    HDC hdc = GetDC(mw->_hWnd);
    int lfHeight = -MulDiv(fsize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    g_math_font = CreateFont(lfHeight, 0, 0, 0, FALSE,
        FALSE, 0, 0, 0, 0, 0, 0, 0, L"Cambria");
    if (!g_math_font)
    {
        ShowError(_T("MainWindow::OnCreate::unable to create math font"));
        return -1;
    }

    Graphics_fontList_init(g_math_font);

    PanelList_PropertyChangedEvent(mw->_panelList, mw->_hWnd , - mw->_x_current_pos,
        mw->_ribbon_height - mw->_y_current_pos);

    SetScrollbarInfo(mw);
    InvalidateRect(mw->_hWnd, NULL, TRUE);
    Editor_UpdateCaret(mw->_selected_panel->_editor, mw->_hWnd);

    return 0;
}

LRESULT OnSize(MainWindow* mw)
{
    if (!IsWindowVisible(mw->_hWnd))
        return 0;

    SendMessage(mw->_hWndStatusBar, WM_SIZE, 0, 0);

    MoveWindow(mw->_hWndVScrollBar,
        mw->_client_width - SCROLLBAR_WIDE,
        mw->_ribbon_height,
        SCROLLBAR_WIDE,
        mw->_client_height - mw->_statusbar_height - mw->_ribbon_height - SCROLLBAR_WIDE,
        TRUE);
    InvalidateRect(mw->_hWndVScrollBar, NULL, TRUE);
    
    MoveWindow(mw->_hWndHScrollBar,
        0,
        mw->_client_height - mw->_statusbar_height - SCROLLBAR_WIDE,
        mw->_client_width - SCROLLBAR_WIDE,
        SCROLLBAR_WIDE,
        TRUE);
    InvalidateRect(mw->_hWndHScrollBar, NULL, TRUE);

    MoveWindow(mw->_hWndCorner,
        mw->_client_width - SCROLLBAR_WIDE,
        mw->_client_height - mw->_statusbar_height - SCROLLBAR_WIDE,
        SCROLLBAR_WIDE,
        SCROLLBAR_WIDE,
        TRUE);
    InvalidateRect(mw->_hWndCorner, NULL, TRUE);

    SetScrollbarInfo(mw);
    InvalidateRect(mw->_hWnd, NULL, TRUE);

    return 0;
}

LRESULT OnRibbonHeightChanged(MainWindow* mw)
{
    if (!IsWindowVisible(mw->_hWnd))
        return 0;

    // Update Vertical ScrollBar when ribbon height changed
    MoveWindow(mw->_hWndVScrollBar,
        mw->_client_width - SCROLLBAR_WIDE,
        mw->_ribbon_height,
        SCROLLBAR_WIDE,
        mw->_client_height - mw->_statusbar_height - mw->_ribbon_height - SCROLLBAR_WIDE,
        TRUE);
    InvalidateRect(mw->_hWndVScrollBar, NULL, TRUE);

    SetScrollbarInfo(mw);
    PanelList_PropertyChangedEvent(mw->_panelList, mw->_hWnd, -mw->_x_current_pos,
        mw->_ribbon_height - mw->_y_current_pos);
    Editor_UpdateCaret(mw->_selected_panel->_editor, mw->_hWnd);
    InvalidateRect(mw->_hWnd, NULL, TRUE);

    return 0;
}

LRESULT OnPaint(MainWindow* mw)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(mw->_hWnd, &ps);

    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

    PanelList_Paint(mw->_panelList, 
        hdc, 
        &ps.rcPaint, 
        - mw->_x_current_pos, 
        mw->_ribbon_height - mw->_y_current_pos);

    EndPaint(mw->_hWnd, &ps);

    return 0;
}

void SetScrollbarInfo(MainWindow* mw)
{
    SCROLLINFO siv, sih;

    int v = PanelList_GetViewportHeight(mw->_panelList) -
        mw->_client_height + mw->_ribbon_height + mw->_statusbar_height + SCROLLBAR_WIDE;
    v = v > 0 ? v : 0;

    mw->_yMaxScroll = v;
    siv.cbSize = sizeof(siv);
    siv.fMask = SIF_RANGE | SIF_POS;
    siv.nMin = 0;
    siv.nMax = mw->_yMaxScroll;
    siv.nPos = mw->_y_current_pos;
    SetScrollInfo(mw->_hWndVScrollBar, SB_CTL, &siv, TRUE);

    int h = PanelList_GetViewportWidth(mw->_panelList) -
        mw->_client_width + SCROLLBAR_WIDE;
    h = h > 0 ? h : 0;

    mw->_xMaxScroll = h;
    sih.cbSize = sizeof(sih);
    sih.fMask = SIF_RANGE | SIF_POS;
    sih.nMin = 0;
    sih.nMax = mw->_xMaxScroll;
    sih.nPos = mw->_x_current_pos;
    SetScrollInfo(mw->_hWndHScrollBar, SB_CTL, &sih, TRUE);
}

LRESULT OnVScroll(MainWindow* mw, WPARAM wParam)
{
    int xDelta = 0;
    int yNewPos;
    int yDelta;

    switch (LOWORD(wParam))
    {
    case SB_PAGEUP:
        yNewPos = mw->_y_current_pos - 60;
        break;

    case SB_PAGEDOWN:
        yNewPos = mw->_y_current_pos + 60;
        break;

    case SB_LINEUP:
        yNewPos = mw->_y_current_pos - 20;
        break;

    case SB_LINEDOWN:
        yNewPos = mw->_y_current_pos + 20;
        break;

    case SB_THUMBPOSITION:
        yNewPos = HIWORD(wParam);
        break;

    default:
        yNewPos = mw->_y_current_pos;
    }

    yNewPos = max(0, yNewPos);
    yNewPos = min(mw->_yMaxScroll, yNewPos);

    // If the current position does not change, do not scroll.
    if (yNewPos == mw->_y_current_pos)
        return 0;

    // Determine the amount scrolled (in pixels). 
    yDelta = yNewPos - mw->_y_current_pos;

    // Reset the current scroll position. 
    mw->_y_current_pos = yNewPos;

    PanelList_PropertyChangedEvent(mw->_panelList, 
        mw->_hWnd, 
        - mw->_x_current_pos, 
        mw->_ribbon_height - mw->_y_current_pos);
    Editor_UpdateCaret(mw->_selected_panel->_editor, mw->_hWnd);

    RECT rc;
    rc.left = 0;
    rc.top = mw->_ribbon_height;
    rc.right = mw->_client_width - SCROLLBAR_WIDE;
    rc.bottom = mw->_client_height - mw->_statusbar_height - SCROLLBAR_WIDE;

    ScrollWindowEx(mw->_hWnd, -xDelta, -yDelta, &rc,
        &rc, (HRGN)NULL, (RECT*)NULL,
        SW_INVALIDATE);
    UpdateWindow(mw->_hWnd);

    // Reset the scroll bar. 
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    si.nPos = mw->_y_current_pos;
    SetScrollInfo(mw->_hWndVScrollBar, SB_CTL, &si, TRUE);

    return 0;
}

LRESULT OnHScroll(MainWindow* mw, WPARAM wParam)
{
    int xDelta;
    int xNewPos;
    int yDelta = 0;

    switch (LOWORD(wParam))
    {
    case SB_PAGEUP:
        xNewPos = mw->_x_current_pos - 60;
        break;

    case SB_PAGEDOWN:
        xNewPos = mw->_x_current_pos + 60;
        break;

    case SB_LINEUP:
        xNewPos = mw->_x_current_pos - 20;
        break;

    case SB_LINEDOWN:
        xNewPos = mw->_x_current_pos + 20;
        break;

    case SB_THUMBPOSITION:
        xNewPos = HIWORD(wParam);
        break;

    default:
        xNewPos = mw->_x_current_pos;
    }

    xNewPos = max(0, xNewPos);
    xNewPos = min(mw->_xMaxScroll, xNewPos);

    // If the current position does not change, do not scroll.
    if (xNewPos == mw->_x_current_pos)
        return 0;

    // Determine the amount scrolled (in pixels). 
    xDelta = xNewPos - mw->_x_current_pos;

    // Reset the current scroll position. 
    mw->_x_current_pos = xNewPos;

    PanelList_PropertyChangedEvent(mw->_panelList,
        mw->_hWnd,
        -mw->_x_current_pos,
        mw->_ribbon_height - mw->_y_current_pos);
    Editor_UpdateCaret(mw->_selected_panel->_editor, mw->_hWnd);

    RECT rc;
    rc.left = 0;
    rc.top = mw->_ribbon_height;
    rc.right = mw->_client_width - SCROLLBAR_WIDE;
    rc.bottom = mw->_client_height - mw->_statusbar_height - SCROLLBAR_WIDE;

    ScrollWindowEx(mw->_hWnd, -xDelta, -yDelta, &rc,
        &rc, (HRGN)NULL, (RECT*)NULL,
        SW_INVALIDATE);
    UpdateWindow(mw->_hWnd);

    // Reset the scroll bar. 
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS;
    si.nPos = mw->_x_current_pos;
    SetScrollInfo(mw->_hWndHScrollBar, SB_CTL, &si, TRUE);

    return 0;
}

void Graphics_fontList_init(HANDLE hFont)
{
    g_fontList[0] = hFont;

    for (int ix = 1; ix < sizeof(g_fontList) / sizeof(g_fontList[0]); ++ix)
    {
        LOGFONT logFont;
        GetObject(hFont, sizeof(LOGFONT), &logFont);
        logFont.lfHeight = (logFont.lfHeight + ix * 5 < 0) ? logFont.lfHeight + ix * 5 : -3;

        g_fontList[ix] = CreateFontIndirect(&logFont);
    }
}

void Graphics_fontList_free()
{
    for (int ix = 1; ix < sizeof(g_fontList) / sizeof(g_fontList[0]); ++ix)
    {

        DeleteObject(g_fontList[ix]);
    }
}

LRESULT OnMousLButtonDown(MainWindow* mw, int x, int y)
{
    Panel* p = PanelList_GetPanelFromPoint(mw->_panelList, 
        x + mw->_x_current_pos, 
        y - mw->_ribbon_height + mw->_y_current_pos);
    if (p)
    {
        mw->_selected_panel = p;
        Editor_UpdateCaret(mw->_selected_panel->_editor, mw->_hWnd);
    }

    return 0;
}

LRESULT OnSetFocus(MainWindow* mw)
{
    if(mw->_selected_panel)
        Editor_OnSetFocus(mw->_selected_panel->_editor, mw->_hWnd);

    return 0;
}

LRESULT OnKillFocus(MainWindow* mw)
{
    if(mw->_selected_panel)
        Editor_OnKillFocus(mw->_selected_panel->_editor);

    return 0;
}

LRESULT OnKeyDown(MainWindow* mw, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case VK_F1:
    {
        HINSTANCE r = ShellExecute(NULL, L"open", L"https://krezek.github.io/SimpleCalculator/index.html", NULL, NULL, SW_SHOWNORMAL);

        if (r <= (HINSTANCE)32)
        {
            ShowError(L"Could not open help file.");
        }
    }
    break;

    case VK_RETURN:
        if (GetKeyState(VK_SHIFT) < 0)
        {
            Calculate(mw);
        }
        else
        {
            Simplify(mw);
        }

        break;

    case VK_HOME:       // Home 
        break;

    case VK_END:        // End 
        break;

    case VK_PRIOR:      // Page Up 
        break;

    case VK_NEXT:       // Page Down 
        break;

    case VK_LEFT:       // Left arrow 
    {
        Editor_OnKey_LeftArrow(mw->_selected_panel->_editor, mw->_hWnd);
        break;
    }

    case VK_RIGHT:      // Right arrow
    {
        Editor_OnKey_RightArrow(mw->_selected_panel->_editor, mw->_hWnd);
        break;
    }
    case VK_UP:         // Up arrow 
        break;

    case VK_DOWN:       // Down arrow 
        break;

    case VK_DELETE:     // Delete 
    {
        Editor_OnChar_Delete(mw->_selected_panel->_editor);
    }
    break;
    }

    return 0;
}

LRESULT OnChar(MainWindow* mw, WPARAM wParam, LPARAM lParam)
{
    HideCaret(mw->_hWnd);

    switch (wParam)
    {
    case 0x08:          // Backspace 
    {
        Editor_OnChar_Backspace(mw->_selected_panel->_editor);
        PanelList_PropertyChangedEvent(mw->_panelList, mw->_hWnd, -mw->_x_current_pos,
            mw->_ribbon_height - mw->_y_current_pos);
        InvalidateRect(mw->_hWnd, NULL, TRUE);
        Editor_UpdateCaret(mw->_selected_panel->_editor, mw->_hWnd);

        break;
    }

    case 0x09:          // Tab 
    {
        // Todo: Implement Tab navigation
        if (GetKeyState(VK_SHIFT) < 0)
        {
        }
        else
        {
        }
    }
    break;

    case 0x0D:          // Carriage return 
        break;

    case 0x1B:        // Escape 
    case 0x0A:        // Linefeed 
        MessageBeep((UINT)-1);
        break;

    default:
    {
        Editor_OnChar_Default(mw->_selected_panel->_editor, (wchar_t)wParam, mw->_hWnd);
        PanelList_PropertyChangedEvent(mw->_panelList, mw->_hWnd, -mw->_x_current_pos,
            mw->_ribbon_height - mw->_y_current_pos);
        InvalidateRect(mw->_hWnd, NULL, TRUE);
        Editor_UpdateCaret(mw->_selected_panel->_editor, mw->_hWnd);

        break;
    }
    }

    ShowCaret(mw->_hWnd);

    return 0;
}

void Simplify(MainWindow* mw)
{
    String* inStr = String_init();
    GList_toString(mw->_selected_panel->_in_gitems_list, inStr);

    wchar_t* outStr = do_simplify(inStr->_str);

    if (outStr)
    {
        printf("In:%S\nOut:%S\n", inStr->_str, outStr);

        GList* gl = NULL;
        parse_test(&gl, outStr);
        if (gl)
        {
            GList_free(mw->_selected_panel->_out_gitems_list);
            mw->_selected_panel->_out_gitems_list = gl;

            PanelList_PropertyChangedEvent(mw->_panelList, mw->_hWnd, -mw->_x_current_pos,
                mw->_ribbon_height - mw->_y_current_pos);
            InvalidateRect(mw->_hWnd, NULL, TRUE);
            Editor_UpdateCaret(mw->_selected_panel->_editor, mw->_hWnd);

            free(outStr);
        }
    }

    String_free(inStr);
}

void Calculate(MainWindow* mw)
{
    String* inStr = String_init();
    GList_toString(mw->_selected_panel->_in_gitems_list, inStr);

    wchar_t* outStr = do_calc(inStr->_str);

    if (outStr)
    {
        GList* gl = NULL;
        parse_test(&gl, outStr);
        if (gl)
        {
            GList_free(mw->_selected_panel->_out_gitems_list);
            mw->_selected_panel->_out_gitems_list = gl;

            PanelList_PropertyChangedEvent(mw->_panelList, mw->_hWnd, -mw->_x_current_pos,
                mw->_ribbon_height - mw->_y_current_pos);
            InvalidateRect(mw->_hWnd, NULL, TRUE);
            Editor_UpdateCaret(mw->_selected_panel->_editor, mw->_hWnd);

            free(outStr);
        }
    }

    String_free(inStr);
}
