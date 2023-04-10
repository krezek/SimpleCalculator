#ifndef _PANEL_H_
#define _PANEL_H_

typedef struct _Panel
{
	int _x0, _y0, _width, _height;
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

Panel* Panel_init();
void Panel_free(Panel* p);

PanelList* PanelList_init();
void PanelList_free(PanelList* pl);

#endif /* _PANEL_H_ */
