#ifndef _PANEL_H_
#define _PANEL_H_

typedef struct _Panel
{
	int _x0, _y0, _width, _height;
	wchar_t* _inStr, * _outStr;
} Panel;

typedef struct _PanelNode
{
	Panel* _panel;

	struct _PanelNode* _next;
	struct _PanelNode* _prev;
} PanelNode;

typedef struct _PanelList
{
	PanelNode* _front;
	PanelNode* _rear;
} PanelList;

Panel* Panel_init(const wchar_t* inStr, const wchar_t* outStr);
void Panel_free(Panel* p);

PanelList* PanelList_init();
void PanelList_free(PanelList* pl);
void PanelList_AddNewPanel(PanelList* pl, const wchar_t* inStr, const wchar_t* outStr);
int PanelList_GetViewportWidth(PanelList* pl);
int PanelList_GetViewportHeight(PanelList* pl);
void PanelList_Paint(PanelList* pl, HDC hdc, RECT* rcPaint, int x0, int y0);

#endif /* _PANEL_H_ */
