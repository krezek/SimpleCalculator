#include "platform.h"

#include <gitems.h>

extern HFONT g_fontList[5];

GNode* GNode_init(GItem* pGItem, GNode* n, GNode* p)
{
	GNode* gn = (GNode*)malloc(sizeof(GNode));
	assert(gn);

	gn->_pGItem = pGItem;
	gn->_next = n;
	gn->_prev = p;

	return gn;
}

void GNode_free(GNode* pn)
{
	// call free function for each GItem object
	pn->_pGItem->_freeFunc(pn->_pGItem);

	// deallocate GItem object
	free(pn->_pGItem);
	pn->_pGItem = NULL;

	free(pn);
}

GList* GList_init(GList* parent)
{
	GList* gll = (GList*)malloc(sizeof(GList));
	assert(gll != NULL);

	gll->_front = NULL;
	gll->_rear = NULL;

	gll->_parent = parent;

	return gll;
}

void GList_free(GList* gll)
{
	if (gll)
	{
		if (gll->_front)
		{
			while (gll->_front)
			{
				GNode* gn = gll->_front;
				gll->_front = gll->_front->_next;

				GNode_free(gn);
			}

			gll->_front = NULL;
			gll->_rear = NULL;
		}

		free(gll);
	}
}

void GList_pushback(GList* gll, GItem* pGItem)
{
	if (gll->_rear == NULL)
	{
		assert(gll->_front == NULL);
		gll->_front = gll->_rear = GNode_init(pGItem, NULL, NULL);
	}
	else
	{
		GNode* i = GNode_init(pGItem, NULL, gll->_rear);
		gll->_rear->_next = i;
		gll->_rear = i;
	}
}

/*void GList_toString(GList* gll, String* str)
{
	if (gll)
	{
		if (gll->_front)
		{
			GNode* gn = gll->_front;
			while (gn)
			{
				gn->_pGItem->_toStringFunc(gn->_pGItem, str);

				gn = gn->_next;
			}
		}
	}
}*/

void GList_calcFontId(GList* gll, int id)
{
	if (gll)
	{
		if (gll->_front)
		{
			GNode* gn = gll->_front;
			while (gn)
			{
				gn->_pGItem->_fontIdFunc(gn->_pGItem, id);

				gn = gn->_next;
			}
		}
	}
}

void GList_calcWidth(GList* gll, HDC hdc)
{
	int width = 0;

	if (gll)
	{
		if (gll->_front)
		{
			GNode* gn = gll->_front;
			while (gn)
			{
				width += gn->_pGItem->_widthFunc(gn->_pGItem, hdc);

				gn = gn->_next;
			}
		}
		gll->_width = width;
	}
}

void GList_calcHeight(GList* gll, HDC hdc)
{
	int height = 0;

	if (gll)
	{
		if (gll->_front)
		{
			GNode* gn = gll->_front;
			while (gn)
			{
				height = max(gn->_pGItem->_heightFunc(gn->_pGItem, hdc), height);

				gn = gn->_next;
			}
		}
		gll->_height = height;
	}
}

void GList_calcBaseLineY(GList* gll, HDC hdc)
{
	int baseLineY = 0;

	if (gll)
	{
		if (gll->_front)
		{
			GNode* gn = gll->_front;
			while (gn)
			{
				baseLineY = max(gn->_pGItem->_baseLineYFunc(gn->_pGItem, hdc), baseLineY);

				gn = gn->_next;
			}
		}
		gll->_baseLineY = baseLineY;
	}
}

void GList_calcXY(GList* gll, HDC hdc, int x0, int y0)
{
	gll->_x0 = x0;
	gll->y0 = y0;

	int x = x0;

	if (gll)
	{
		if (gll->_front)
		{
			GNode* gn = gll->_front;
			while (gn)
			{
				gn->_pGItem->_calcXYFunc(gn->_pGItem, hdc, x, y0);
				x += gn->_pGItem->_width;

				gn = gn->_next;
			}
		}
	}
}

void GList_draw(GList* gll, HDC hdc)
{
	if (gll)
	{
		if (gll->_front)
		{
			GNode* gn = gll->_front;
			while (gn)
			{
				gn->_pGItem->_drawFunc(gn->_pGItem, hdc);

				gn = gn->_next;
			}
		}
	}
}

void GItemChar_free(GItem* _this)
{
	GItemChar* g = (GItemChar*)_this;
}

/*void GItemChar_toString(GItem* _this, String* str)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t gstr[] = {g->_ch, 0};

	if(g->_ch)
		String_cat(str, gstr);
}*/

void GItemChar_fontId(GItem* _this, int id)
{
	GItemChar* g = (GItemChar*)_this;

	_this->_fontId = (id >= (sizeof(g_fontList)/sizeof(g_fontList[0]))) 
		? (sizeof(g_fontList) / sizeof(g_fontList[0])) - 1 : id;
}

int GItemChar_width(GItem* _this, HDC hdc)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);

	_this->_width = s.cx;

	return _this->_width;
}

int GItemChar_height(GItem* _this, HDC hdc)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);
	_this->_height = s.cy;

	return _this->_height;
}

int GItemChar_baseline_y(GItem* _this, HDC hdc)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);
	_this->_baseLineY = s.cy / 2;

	return _this->_baseLineY;
}

void GItemChar_calcXY(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);

	_this->_x0 = x0;
	_this->_y0 = y0 - s.cy / 2;
}

void GItemChar_draw(GItem* _this, HDC hdc)
{
	GItemChar* g = (GItemChar*)_this;
	wchar_t str[2] = { g->_ch, 0 };

	SelectObject(hdc, g_fontList[_this->_fontId]);

	TextOut(hdc, _this->_x0, _this->_y0, str, 1);
}

GItemChar* GItemChar_init(const wchar_t ch)
{
	GItemChar* g = (GItemChar*)malloc(sizeof(GItemChar));
	assert(g != NULL);

	g->_gitem._type = GITEM_CHAR;

	g->_gitem._freeFunc = GItemChar_free;
	//g->_gitem._toStringFunc = GItemChar_toString;
	g->_gitem._fontIdFunc = GItemChar_fontId;
	g->_gitem._widthFunc = GItemChar_width;
	g->_gitem._heightFunc = GItemChar_height;
	g->_gitem._baseLineYFunc = GItemChar_baseline_y;
	g->_gitem._calcXYFunc = GItemChar_calcXY;
	g->_gitem._drawFunc = GItemChar_draw;

	g->_ch = ch;

	return g;
}

void GItemPower_free(GItem* _this)
{
	GItemPower* g = (GItemPower*)_this;

	GList_free(g->_exponent);
}

/*void GItemPower_toString(GItem* _this, String* str)
{
	GItemPower* g = (GItemPower*)_this;
	wchar_t gstr1[] = { L'^', L'(', 0};
	wchar_t gstr2[] = { L')', 0 };

	String_cat(str, gstr1);
	GList_toString(g->_exponent, str);
	String_cat(str, gstr2);
}*/

void GItemPower_fontId(GItem* _this, int id)
{
	GItemPower* g = (GItemPower*)_this;

	_this->_fontId = (id >= (sizeof(g_fontList) / sizeof(g_fontList[0])))
		? (sizeof(g_fontList) / sizeof(g_fontList[0])) - 1 : id;

	GList_calcFontId(g->_exponent, _this->_fontId + 1);
}

int GItemPower_width(GItem* _this, HDC hdc)
{
	GItemPower* g = (GItemPower*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);
	
	GList_calcWidth(g->_exponent, hdc);

	_this->_width = g->_exponent->_width;

	return _this->_width;
}

int GItemPower_height(GItem* _this, HDC hdc)
{
	GItemPower* g = (GItemPower*)_this;
	wchar_t str[2] = { L'A', 0};
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);
	_this->_height = s.cy;

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcHeight(g->_exponent, hdc);

	_this->_height += g->_exponent->_height - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;

	return _this->_height;
}

int GItemPower_baseline_y(GItem* _this, HDC hdc)
{
	GItemPower* g = (GItemPower*)_this;
	wchar_t str[2] = { L'A', 0};
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	_this->_baseLineY = s.cy / 2 + g->_exponent->_height - tm.tmDescent - tm.tmInternalLeading - tm.tmExternalLeading;

	GList_calcBaseLineY(g->_exponent, hdc);

	return _this->_baseLineY;
}

void GItemPower_calcXY(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemPower* g = (GItemPower*)_this;
	wchar_t str[2] = { L'A', 0};
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	_this->_x0 = x0;
	_this->_y0 = y0 - s.cy / 2 - g->_exponent->_height + tm.tmDescent + tm.tmInternalLeading + tm.tmExternalLeading;

	GList_calcXY(g->_exponent, hdc, _this->_x0, _this->_y0 + g->_exponent->_baseLineY);
}

void GItemPower_draw(GItem* _this, HDC hdc)
{
	GItemPower* g = (GItemPower*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	if (g->_exponent->_front == g->_exponent->_rear)
	{
		TextOut(hdc, _this->_x0, _this->_y0, L".", 1);
	}

	GList_draw(g->_exponent, hdc);
}

GItemPower* GItemPower_init(GList* exponent)
{
	GItemPower* g = (GItemPower*)malloc(sizeof(GItemPower));
	assert(g != NULL);

	g->_gitem._type = GITEM_POWER;

	g->_gitem._freeFunc = GItemPower_free;
	//g->_gitem._toStringFunc = GItemPower_toString;
	g->_gitem._fontIdFunc = GItemPower_fontId;
	g->_gitem._widthFunc = GItemPower_width;
	g->_gitem._heightFunc = GItemPower_height;
	g->_gitem._baseLineYFunc = GItemPower_baseline_y;
	g->_gitem._calcXYFunc = GItemPower_calcXY;
	g->_gitem._drawFunc = GItemPower_draw;

	g->_exponent = exponent;

	return g;
}

void GItemFraction_free(GItem* _this)
{
	GItemFraction* g = (GItemFraction*)_this;
	
	GList_free(g->_numerator);
	GList_free(g->_denominator);

	g->_numerator = NULL;
	g->_denominator = NULL;
}

/*void GItemFraction_toString(GItem* _this, String* str)
{
	GItemFraction* g = (GItemFraction*)_this;
	wchar_t gstr1[] = { L'(', 0 };
	wchar_t gstr2[] = { L')', 0 };
	wchar_t gstr3[] = { L'/', 0 };

	String_cat(str, gstr1);
	String_cat(str, gstr1);
	GList_toString(g->_numerator, str);
	String_cat(str, gstr2);
	String_cat(str, gstr3);
	String_cat(str, gstr1);
	GList_toString(g->_denominator, str);
	String_cat(str, gstr2);
	String_cat(str, gstr2);

}*/

void GItemFraction_fontId(GItem* _this, int id)
{
	GItemFraction* g = (GItemFraction*)_this;

	_this->_fontId = (id >= (sizeof(g_fontList) / sizeof(g_fontList[0])))
		? (sizeof(g_fontList) / sizeof(g_fontList[0])) - 1 : id;

	GList_calcFontId(g->_numerator, _this->_fontId);
	GList_calcFontId(g->_denominator, _this->_fontId);
}

int GItemFraction_width(GItem* _this, HDC hdc)
{
	GItemFraction* g = (GItemFraction*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_calcWidth(g->_numerator, hdc);
	GList_calcWidth(g->_denominator, hdc);

	_this->_width = FRACTION_PADDING * 2 + max(g->_numerator->_width, g->_denominator->_width);

	return _this->_width;
}

int GItemFraction_height(GItem* _this, HDC hdc)
{
	GItemFraction* g = (GItemFraction*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_calcHeight(g->_numerator, hdc);
	GList_calcHeight(g->_denominator, hdc);

	_this->_height = g->_numerator->_height + g->_denominator->_height;

	return _this->_height;
}

int GItemFraction_baseline_y(GItem* _this, HDC hdc)
{
	GItemFraction* g = (GItemFraction*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	_this->_baseLineY = g->_numerator->_height;

	GList_calcBaseLineY(g->_numerator, hdc);
	GList_calcBaseLineY(g->_denominator, hdc);

	return _this->_baseLineY;
}

void GItemFraction_calcXY(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemFraction* g = (GItemFraction*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	_this->_x0 = x0;
	_this->_y0 = y0 - g->_numerator->_height;

	int w1 = (_this->_width - g->_numerator->_width) / 2;
	int w2 = (_this->_width - g->_denominator->_width) / 2;

	GList_calcXY(g->_numerator, hdc, _this->_x0 + w1, _this->_y0 + g->_numerator->_baseLineY);
	GList_calcXY(g->_denominator, hdc, _this->_x0 + w2, y0 + g->_denominator->_baseLineY);
}

void GItemFraction_draw(GItem* _this, HDC hdc)
{
	GItemFraction* g = (GItemFraction*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_draw(g->_numerator, hdc);
	GList_draw(g->_denominator, hdc);

	MoveToEx(hdc, _this->_x0, _this->_y0 + g->_numerator->_height, NULL);
	LineTo(hdc, _this->_x0 + _this->_width, _this->_y0 + g->_numerator->_height);
}

GItemFraction* GItemFraction_init(GList* num, GList* den)
{
	GItemFraction* g = (GItemFraction*)malloc(sizeof(GItemFraction));
	assert(g != NULL);

	g->_gitem._type = GITEM_FRACTION;

	g->_gitem._freeFunc = GItemFraction_free;
	//g->_gitem._toStringFunc = GItemFraction_toString;
	g->_gitem._fontIdFunc = GItemFraction_fontId;
	g->_gitem._widthFunc = GItemFraction_width;
	g->_gitem._heightFunc = GItemFraction_height;
	g->_gitem._baseLineYFunc = GItemFraction_baseline_y;
	g->_gitem._calcXYFunc = GItemFraction_calcXY;
	g->_gitem._drawFunc = GItemFraction_draw;

	g->_numerator = num;
	g->_denominator = den;

	return g;
}

void GItemRoot_free(GItem* _this)
{
	GItemRoot* g = (GItemRoot*)_this;

	GList_free(g->_expr);
	GList_free(g->_root);
}

/*void GItemRoot_toString(GItem* _this, String* str)
{
	GItemRoot* g = (GItemRoot*)_this;
	wchar_t gstr1[] = { L'R', L'o', L'o', L't', L'(', 0 };
	wchar_t gstr2[] = { L';', 0 };
	wchar_t gstr3[] = { L')', 0 };
	wchar_t gstr4[] = { L'2', 0 };

	String_cat(str, gstr1);
	GList_toString(g->_expr, str);
	String_cat(str, gstr2);

	if (g->_root->_front == g->_root->_rear)
	{
		String_cat(str ,gstr4);
	}
	else
	{
		GList_toString(g->_root, str);
	}
	
	String_cat(str, gstr3);
}*/

void GItemRoot_fontId(GItem* _this, int id)
{
	GItemRoot* g = (GItemRoot*)_this;

	_this->_fontId = (id >= (sizeof(g_fontList) / sizeof(g_fontList[0])))
		? (sizeof(g_fontList) / sizeof(g_fontList[0])) - 1 : id;

	GList_calcFontId(g->_expr, _this->_fontId);
	GList_calcFontId(g->_root, _this->_fontId + 1);
}

int GItemRoot_width(GItem* _this, HDC hdc)
{
	GItemRoot* g = (GItemRoot*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_calcWidth(g->_expr, hdc);
	GList_calcWidth(g->_root, hdc);

	_this->_width = ROOT_PADDING * 2 + g->_expr->_width + g->_root->_width;

	return _this->_width;
}

int GItemRoot_height(GItem* _this, HDC hdc)
{
	GItemRoot* g = (GItemRoot*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);
	//_this->_height = s.cy;

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcHeight(g->_expr, hdc);
	GList_calcHeight(g->_root, hdc);

	_this->_height = max(g->_root->_height,g->_expr->_height);
	
	return _this->_height;
}

int GItemRoot_baseline_y(GItem* _this, HDC hdc)
{
	GItemRoot* g = (GItemRoot*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GList_calcBaseLineY(g->_expr, hdc);
	GList_calcBaseLineY(g->_root, hdc);

	_this->_baseLineY = g->_expr->_baseLineY;

	return _this->_baseLineY;
}

void GItemRoot_calcXY(GItem* _this, HDC hdc, int x0, int y0)
{
	GItemRoot* g = (GItemRoot*)_this;
	wchar_t str[2] = { L'A', 0 };
	SIZE s;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GetTextExtentPoint32(hdc, str, 1, &s);
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	_this->_x0 = x0;
	_this->_y0 = y0 - _this->_baseLineY;

	GList_calcXY(g->_root, hdc, _this->_x0, _this->_y0 + max(g->_gitem._height - g->_root->_height, g->_root->_baseLineY));
	GList_calcXY(g->_expr, hdc, _this->_x0 + g->_root->_width + ROOT_PADDING, _this->_y0 + g->_expr->_baseLineY);
}

void GItemRoot_draw(GItem* _this, HDC hdc)
{
	GItemRoot* g = (GItemRoot*)_this;

	SelectObject(hdc, g_fontList[_this->_fontId]);

	GList_draw(g->_root, hdc);
	GList_draw(g->_expr, hdc);

	MoveToEx(hdc, g->_gitem._x0 + g->_root->_width - 5, g->_gitem._y0 + g->_gitem._height - 5, NULL);
	LineTo(hdc, g->_gitem._x0 + g->_root->_width, g->_gitem._y0 + g->_gitem._height);
	LineTo(hdc, g->_gitem._x0 + g->_root->_width + ROOT_PADDING, g->_gitem._y0 + 5);
	LineTo(hdc, g->_gitem._x0 + g->_root->_width + g->_expr->_width + ROOT_PADDING, g->_gitem._y0 + 5);

}

GItemRoot* GItemRoot_init(GList* expr, GList* root)
{
	GItemRoot* g = (GItemRoot*)malloc(sizeof(GItemRoot));
	assert(g != NULL);

	g->_gitem._type = GITEM_ROOT;

	g->_gitem._freeFunc = GItemRoot_free;
	//g->_gitem._toStringFunc = GItemRoot_toString;
	g->_gitem._fontIdFunc = GItemRoot_fontId;
	g->_gitem._widthFunc = GItemRoot_width;
	g->_gitem._heightFunc = GItemRoot_height;
	g->_gitem._baseLineYFunc = GItemRoot_baseline_y;
	g->_gitem._calcXYFunc = GItemRoot_calcXY;
	g->_gitem._drawFunc = GItemRoot_draw;

	g->_expr = expr;
	g->_root = root;

	return g;
}