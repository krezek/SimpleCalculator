#ifndef _MAIN_WND_H_
#define _MAIN_WND_H_

typedef struct _PanelList PanelList;
typedef struct _MainWindow MainWindow;

typedef LRESULT(*HandleMessageFunc) (MainWindow* _this, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct _MainWindow
{
	HWND _hWnd;
	HWND _hWndStatusBar;
	HWND _hWndVScrollBar, _hWndHScrollBar, _hWndCorner;
	
	int _ribbon_height, _statusbar_height;
	int _client_width, _client_height;
	int _x_current_pos, _y_current_pos;
	PanelList* _panelList;

	HandleMessageFunc _HandleMessageFunc;
} MainWindow;

ATOM MainWindow_RegisterClass();

MainWindow* MainWindow_init();
void MainWindow_free(MainWindow* mw);

BOOL MainWindow_Create(MainWindow* _this);

#endif /* _MAIN_WND_H_ */

