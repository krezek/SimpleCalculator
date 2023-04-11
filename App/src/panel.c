#include "platform.h"

#include <panel.h>

#define PANEL_LIST_MARGIN_H 10
#define PANEL_LIST_MARGIN_V 10
#define PANEL_MARGIN_H 5
#define PANEL_MARGIN_V 5

extern HFONT g_math_font;

Panel* Panel_init(const wchar_t* inStr, const wchar_t* outStr)
{
	Panel* p = (Panel*)malloc(sizeof(Panel));
	assert(p != NULL);

	p->_inStr = (wchar_t*)malloc((wcslen(inStr) + 1) * sizeof(wchar_t));
	assert(p->_inStr != NULL);

	p->_outStr = (wchar_t*)malloc((wcslen(outStr) + 1) * sizeof(wchar_t));
	assert(p->_outStr != NULL);

	wcscpy(p->_inStr, inStr);
	wcscpy(p->_outStr, outStr);

	return p;
}

void Panel_free(Panel* p)
{
	free(p->_outStr);
	free(p->_inStr);

	free(p);
}

void Panel_Paint(Panel* p, HDC hdc, int x0, int y0)
{
	RECT rc;
	rc.left = x0 + p->_x0;
	rc.top = y0 + p->_y0;
	rc.right = rc.left + p->_width;
	rc.bottom = rc.top + p->_height;

	FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOWFRAME));
	HFONT hOldFont = SelectObject(hdc, g_math_font);
	SelectObject(hdc, GetStockObject(DC_PEN));

	{
		SetDCPenColor(hdc, RGB(255, 0, 0));
		MoveToEx(hdc, x0 + p->_x0, y0 + p->_y0, NULL);
		LineTo(hdc, x0 + p->_x0, y0 + p->_y0 + p->_height);
		LineTo(hdc, x0 + p->_x0 + 10, y0 + p->_y0 + p->_height);
		LineTo(hdc, x0 + p->_x0, y0 + p->_y0 + p->_height - x0 - p->_x0);
		MoveToEx(hdc, x0 + p->_x0, y0 + p->_y0, NULL);
		LineTo(hdc, x0 + p->_x0 + 10, y0 + p->_y0);
	}

	{
		SetDCPenColor(hdc, RGB(255, 0, 0));
		MoveToEx(hdc, x0 + p->_x0 + p->_width - 10, y0 + p->_y0, NULL);
		LineTo(hdc, x0 + p->_x0 + p->_width, y0 + p->_y0);
		LineTo(hdc, x0 + p->_x0 + p->_width, y0 + p->_y0 + p->_height);
		LineTo(hdc, x0 + p->_x0 + p->_width - 10, y0 + p->_y0 + p->_height);
	}

	{
		SetTextColor(hdc, RGB(255, 0, 0));
		TextOut(hdc, x0 + p->_x0 + PANEL_MARGIN_V, y0 + p->_y0 + PANEL_MARGIN_H
			, p->_inStr, (int)wcslen(p->_inStr));
		TextOut(hdc, x0 + p->_x0 + PANEL_MARGIN_V, y0 + p->_y0 + 2 * PANEL_MARGIN_H + p->_inStrSize.cy
			, p->_outStr, (int)wcslen(p->_outStr));
	}

	SelectObject(hdc, hOldFont);
}

void Panel_fontChangedEvent(Panel* p, HWND hWnd)
{
	HDC hdc = GetDC(hWnd);
	SelectObject(hdc, g_math_font);

	GetTextExtentPoint(hdc, p->_inStr, (int)wcslen(p->_inStr), &p->_inStrSize);
	GetTextExtentPoint(hdc, p->_outStr, (int)wcslen(p->_outStr), &p->_outStrSize);
	GetTextExtentPoint(hdc, L"W", 1, &p->_paddingSize);

	ReleaseDC(hWnd, hdc);

	p->_width = max(p->_inStrSize.cx, p->_outStrSize.cx) + p->_paddingSize.cx + 2 * PANEL_LIST_MARGIN_V;
	p->_height = p->_inStrSize.cy + p->_outStrSize.cy + 3 * PANEL_MARGIN_H;
}

PanelNode* PanelNode_init(Panel* p, PanelNode* nxt, PanelNode* prv)
{
	PanelNode* pn = (PanelNode*)malloc(sizeof(PanelNode));
	assert(pn);

	pn->_panel = p;
	pn->_next = nxt;
	pn->_prev = prv;

	return pn;
}

void PanelNode_free(PanelNode* pn)
{
	if (pn->_panel)
	{
		Panel_free(pn->_panel);
	}

	free(pn);
}

PanelList* PanelList_init()
{
	PanelList* pl = (PanelList*)malloc(sizeof(PanelList));
	assert(pl != NULL);

	pl->_front = NULL;
	pl->_rear = NULL;

	return pl;
}

void PanelList_free(PanelList* pl)
{
	if (pl)
	{
		if (pl->_front)
		{
			while (pl->_front)
			{
				PanelNode* pn = pl->_front;
				pl->_front = pl->_front->_next;

				PanelNode_free(pn);
			}

			pl->_front = NULL;
			pl->_rear = NULL;
		}

		free(pl);
	}
}

void PanelList_pushpack(PanelList* pl, Panel* p)
{
	if (pl->_rear == NULL)
	{
		assert(pl->_front == NULL);
		pl->_front = pl->_rear = PanelNode_init(p, NULL, NULL);
	}
	else
	{
		PanelNode* pn = PanelNode_init(p, NULL, pl->_rear);
		pl->_rear->_next = pn;
		pl->_rear = pn;
	}
}

void PanelList_AddNewPanel(PanelList* pl, const wchar_t* inStr, const wchar_t* outStr)
{
	Panel* p = Panel_init(inStr, outStr);
	PanelList_pushpack(pl, p);
}

int PanelList_GetViewportWidth(PanelList* pl)
{
	int w = 0;

	if (pl)
	{
		if (pl->_front)
		{
			PanelNode* pn = pl->_front;
			while (pn)
			{
				w = max(w, pn->_panel->_width);
				pn = pn->_next;
			}
		}
	}

	w += PANEL_LIST_MARGIN_V * 2;

	return w;
}

int PanelList_GetViewportHeight(PanelList* pl)
{
	int y = PANEL_LIST_MARGIN_H;

	if (pl)
	{
		if (pl->_front)
		{
			PanelNode* pn = pl->_front;
			while (pn)
			{
				y += pn->_panel->_height + PANEL_LIST_MARGIN_H;
				pn = pn->_next;
			}
		}
		else
			y += PANEL_LIST_MARGIN_H;
	}

	return y;
}

void PanelList_Paint(PanelList* pl, HDC hdc, RECT* rcPaint, int x0, int y0)
{
	if (pl)
	{
		if (pl->_front)
		{
			PanelNode* pn = pl->_front;
			while (pn)
			{
				Panel* p = pn->_panel;
				RECT rc, pRect;

				pRect.left = x0 + p->_x0;
				pRect.top = y0 + p->_y0;
				pRect.right = pRect.left + p->_width;
				pRect.bottom = pRect.top + p->_height;

				if (IntersectRect(&rc, rcPaint, &pRect))
				{
					Panel_Paint(pn->_panel, hdc, x0, y0);
				}

				pn = pn->_next;
			}
		}
	}
}

void PanelList_fontChangedEvent(PanelList* pl, HWND hWnd)
{
	int x = PANEL_LIST_MARGIN_V;
	int y = PANEL_LIST_MARGIN_H;

	if (pl)
	{
		if (pl->_front)
		{
			PanelNode* pn = pl->_front;
			while (pn)
			{
				pn->_panel->_x0 = x;
				pn->_panel->_y0 = y;

				Panel_fontChangedEvent(pn->_panel, hWnd);

				y += pn->_panel->_height + PANEL_LIST_MARGIN_H;

				pn = pn->_next;
			}
		}
	}
}
