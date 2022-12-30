#include "platform.h"

#include "calcimpl.h"
#include "parser.h"
#include "panel.h"

static RECT GetRect(Panel* p);
static void OnInit(Panel* p);
static void OnPaint(Panel* p, HDC hdc);
static void OnPosChanged(Panel* p);
static void OnFontChanged(Panel* p);
static void OnSizeChanged(Panel* p);
static void CalcGItemsValues(Panel* p);
static void CalcPanelSize(Panel* p);

static void OnSetFocus(Panel* p);
static void OnKillFocus(Panel* p);

static void OnKey_LeftArrow(Panel* p);
static void OnKey_RightArrow(Panel* p);

static void OnChar_Default(Panel* p, wchar_t ch);
static void OnChar_Backspace(Panel* p);
static void OnChar_Return(Panel* p);
static void OnChar_Delete(Panel* p);

static void OnCmd(Panel* p, int cmd);

static const int g_margin_h = 10, g_margin_v = 10;

extern const wchar_t* g_in_str;
extern const wchar_t* g_out_str; 
extern int g_in_str_width, g_in_str_height;
extern int g_out_str_width, g_out_str_height;
extern int g_padding;

extern HFONT g_math_font;

Panel* Panel_init(HWND hWnd)
{
	Panel* p = (Panel*)malloc(sizeof(Panel));
	assert(p != NULL);

	p->_in_gitems_list = GList_init(NULL);
	p->_out_gitems_list = GList_init(NULL);

	p->_GetRectFunc = GetRect;

	p->_OnInitFunc = OnInit;
	p->_OnPaintFunc = OnPaint;
	p->_OnPosChangedFunc = OnPosChanged;
	p->_OnFontChangedFunc = OnFontChanged;
	p->_OnSizeChangedFunc = OnSizeChanged;
	
	p->_OnSetFocusFunc = OnSetFocus;
	p->_OnKillFocusFunc = OnKillFocus;
	
	p->_OnKey_LeftArrowFunc = OnKey_LeftArrow;
	p->_OnKey_RightArrowFunc = OnKey_RightArrow;
	p->_OnChar_DefaultFunc = OnChar_Default;
	p->_OnChar_BackspaceFunc = OnChar_Backspace;
	p->_OnChar_ReturnFunc = OnChar_Return;
	p->_OnChar_DeleteFunc = OnChar_Delete;

	p->_OnCmdFunc = OnCmd;

	p->_hWndParent = hWnd;

	p->_editor = Editor_init();

	return p;
}

void Panel_free(Panel* p)
{
	if (p->_editor)
	{
		Editor_free(p->_editor);
	}
	
	GList_free(p->_in_gitems_list);
	GList_free(p->_out_gitems_list);

	free(p);
}

static RECT GetRect(Panel* p)
{
	RECT rc;

	rc.left = p->_x0;
	rc.top = p->_y0;

	rc.right = rc.left + p->_width;
	rc.bottom = rc.top + p->_height;

	return rc;
}

static void OnInit(Panel* p)
{
	GList_pushback(p->_in_gitems_list, (GItem*)GItemChar_init(0));

	p->_editor->_OnInitFunc(p->_editor, p->_hWndParent, p->_in_gitems_list);

	CalcGItemsValues(p);
	CalcPanelSize(p);
}

static void OnPaint(Panel* p, HDC hdc)
{
	RECT rc;
	rc.left = p->_x0;
	rc.top = p->_y0;
	rc.right = rc.left + p->_width;
	rc.bottom = rc.top + p->_height;

	FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOWFRAME));

	HFONT hOldFont = SelectObject(hdc,  g_math_font);
	SelectObject(hdc, GetStockObject(DC_PEN));

	{
		SetDCPenColor(hdc, RGB(255, 0, 0));
		MoveToEx(hdc, p->_x0, p->_y0, NULL);
		LineTo(hdc, p->_x0, p->_y0 + p->_height);
		LineTo(hdc, p->_x0 + 10, p->_y0 + p->_height);
		LineTo(hdc, p->_x0, p->_y0 + p->_height - p->_x0);
		MoveToEx(hdc, p->_x0, p->_y0, NULL);
		LineTo(hdc, p->_x0 + 10, p->_y0);
		SetDCPenColor(hdc, RGB(0, 0, 0));
	}

	{
		SetTextColor(hdc, RGB(255, 0, 0));

		TextOut(hdc, p->_x0 + g_margin_v, p->_y0 + g_margin_h + 
			max(p->_in_gitems_list->_baseLineY, g_in_str_height / 2) - g_in_str_height / 2, g_in_str, (int)wcslen(g_in_str));
		TextOut(hdc, p->_x0 + g_margin_v, p->_y0 + 2 * g_margin_h + max(p->_in_gitems_list->_height, g_in_str_height) + 
			max(p->_out_gitems_list->_baseLineY, g_out_str_height / 2) - g_out_str_height / 2, g_out_str, (int)wcslen(g_out_str));

		SetTextColor(hdc, RGB(0, 0, 0));
	}

	if(p->_in_gitems_list)
	{
		GList_draw(p->_in_gitems_list, hdc);
	}

	if (p->_out_gitems_list)
	{
		GList_draw(p->_out_gitems_list, hdc);
	}

	SelectObject(hdc, hOldFont);
}

static void OnPosChanged(Panel* p)
{
	CalcGItemsValues(p);
}

static void OnFontChanged(Panel* p)
{
	CalcGItemsValues(p);
	CalcPanelSize(p);
}

static void OnSizeChanged(Panel* p)
{
	CalcGItemsValues(p);
	CalcPanelSize(p);
}

static void CalcGItemsValues(Panel* p)
{
	HDC hdc = GetDC(p->_hWndParent);
	SelectObject(hdc, g_math_font);

	SelectObject(hdc, GetStockObject(DC_PEN));

	GList_calcFontId(p->_in_gitems_list, 0);
	GList_calcWidth(p->_in_gitems_list, hdc);
	GList_calcHeight(p->_in_gitems_list, hdc);
	GList_calcBaseLineY(p->_in_gitems_list, hdc);
	GList_calcXY(p->_in_gitems_list, hdc,
		p->_x0 + g_margin_v + 100,
		p->_y0 + g_margin_h + p->_in_gitems_list->_baseLineY);

	GList_calcFontId(p->_out_gitems_list, 0);
	GList_calcWidth(p->_out_gitems_list, hdc);
	GList_calcHeight(p->_out_gitems_list, hdc);
	GList_calcBaseLineY(p->_out_gitems_list, hdc);
	GList_calcXY(p->_out_gitems_list, hdc,
		p->_x0 + g_margin_v + 100,
		p->_y0 + g_margin_h * 2 + p->_out_gitems_list->_baseLineY + p->_in_gitems_list->_height);

	ReleaseDC(p->_hWndParent, hdc);
}

static void CalcPanelSize(Panel* p)
{
	p->_width = 2 * g_margin_v + max(g_in_str_width, g_out_str_width) + g_padding + 
		max(p->_in_gitems_list->_width, p->_out_gitems_list->_height);
	p->_height = max(g_in_str_height, p->_in_gitems_list->_height) + 
		max(g_out_str_height, p->_out_gitems_list->_height) + 3 * g_margin_h;
}

static void OnSetFocus(Panel* p)
{
	if (p->_editor)
		p->_editor->_OnSetFocusFunc(p->_editor);
}

static void OnKillFocus(Panel* p)
{
	if (p->_editor)
		p->_editor->_OnKillFocusFunc(p->_editor);
}

static void OnKey_LeftArrow(Panel* p)
{
	if (p->_editor)
		p->_editor->_OnKey_LeftArrowFunc(p->_editor);
}

static void OnKey_RightArrow(Panel* p)
{
	if (p->_editor)
		p->_editor->_OnKey_RightArrowFunc(p->_editor);
}

static void OnChar_Default(Panel* p, wchar_t ch)
{
	if (p->_editor)
		p->_editor->_OnChar_DefaultFunc(p->_editor, ch);
}

static void OnChar_Backspace(Panel* p)
{
	if (p->_editor)
		p->_editor->_OnChar_BackspaceFunc(p->_editor);
}

static void OnChar_Return(Panel* p)
{
	String* str = String_init();
	GList_toString(p->_in_gitems_list, str);

	wprintf(L"%s\n", str->_str);

	Item* inItems;
	int rc = parse(&inItems, str->_str);
	if (rc)
	{
		wprintf(L"Parse error ...\n");
	}
	else
	{
		double result;
		wchar_t wst[255];

		rc = calculate(&inItems, &result);
		if (rc == 0)
		{
			long long lrs = (long long)result;

			if (result == (double)lrs)
			{
				swprintf_s(wst, 255, L"%lld", lrs);
			}
			else
			{
				swprintf_s(wst, 255, L"%e", result);
			}
		}
		else
		{
			swprintf_s(wst, 255, L"Invalid value");
		}

		GList_free(p->_out_gitems_list);
		p->_out_gitems_list = GList_init(NULL);

		for (int ix = 0; ix <= wcslen(wst); ++ix)
		{
			GList_pushback(p->_out_gitems_list, (GItem*)GItemChar_init(wst[ix]));
		}

		ItemTree_free(&inItems);
	}

	String_free(str);
}

static void OnCmd(Panel* p, int cmd)
{
	p->_editor->_OnCmdFunc(p->_editor, cmd);
}

static void OnChar_Delete(Panel* p)
{
	//p->_editor->_OnChar_DeleteFunc(p->_editor);
}
