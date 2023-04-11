#include "platform.h"

#include <editor.h>
#include "ids.h"

Editor* Editor_init()
{
	Editor* ed = (Editor*)malloc(sizeof(Editor));
	assert(ed != NULL);

	return ed;
}

void Editor_free(Editor* ed)
{
	free(ed);
}

void Editor_OnInit(Editor* ed, GList* gll)
{
	ed->_in_gitems_list = gll;
	ed->_current_glist = gll;

	if (ed->_in_gitems_list->_front)
	{
		ed->_current_glist->_current = ed->_current_glist->_front;
	}
	else
	{
		ed->_current_glist->_current = NULL;
	}
}

void Editor_UpdateCaret(Editor* ed, HWND hWnd)
{
	if (!ed->_current_glist->_current)
		return;

	CreateCaret(hWnd, (HBITMAP)NULL, 2, ed->_current_glist->_current->_pGItem->_height);
	SetCaretPos(ed->_current_glist->_current->_pGItem->_x0, ed->_current_glist->_current->_pGItem->_y0);
	ShowCaret(hWnd);
	
}

void Editor_OnSetFocus(Editor* ed, HWND hWnd)
{
	Editor_UpdateCaret(ed, hWnd);
}

void Editor_OnKillFocus(Editor* ed)
{
	DestroyCaret();
}

void Editor_OnKey_LeftArrow(Editor* ed, HWND hWnd)
{
	GNode* node = (GNode*)ed->_current_glist->_current;

	if (!node)
		return;

	if (!node->_prev)
	{
		GList* parent = ed->_current_glist->_parent;
		if (parent)
		{
			if (parent->_current->_pGItem->_type == GITEM_POWER)
			{
				ed->_current_glist = parent;
			}
			else if (parent->_current->_pGItem->_type == GITEM_FRACTION)
			{
				if (ed->_current_glist != ((GItemFraction*)parent->_current->_pGItem)->_numerator)
				{
					ed->_current_glist = ((GItemFraction*)parent->_current->_pGItem)->_numerator;
					ed->_current_glist->_current = ed->_current_glist->_rear;
				}
				else
				{
					ed->_current_glist = parent;
				}
			}
			else if (parent->_current->_pGItem->_type == GITEM_ROOT)
			{
				if (ed->_current_glist != ((GItemRoot*)parent->_current->_pGItem)->_root)
				{
					ed->_current_glist = ((GItemRoot*)parent->_current->_pGItem)->_root;
					ed->_current_glist->_current = ed->_current_glist->_rear;
				}
				else
				{
					ed->_current_glist = parent;
				}
			}
		}
	}
	else
	{
		if (node->_prev->_pGItem->_type == GITEM_CHAR)
		{
			ed->_current_glist->_current = node->_prev;
		}
		else if (node->_prev->_pGItem->_type == GITEM_POWER)
		{
			ed->_current_glist->_current = node->_prev;
			ed->_current_glist = ((GItemPower*)node->_prev->_pGItem)->_exponent;
			ed->_current_glist->_current = ed->_current_glist->_rear;
		}
		else if (node->_prev->_pGItem->_type == GITEM_FRACTION)
		{
			ed->_current_glist->_current = node->_prev;
			ed->_current_glist = ((GItemFraction*)node->_prev->_pGItem)->_denominator;
			ed->_current_glist->_current = ed->_current_glist->_rear;
		}
		else if (node->_prev->_pGItem->_type == GITEM_ROOT)
		{
			ed->_current_glist->_current = node->_prev;
			ed->_current_glist = ((GItemRoot*)node->_prev->_pGItem)->_expr;
			ed->_current_glist->_current = ed->_current_glist->_rear;
		}
	}

	Editor_UpdateCaret(ed, hWnd);
}

void Editor_OnKey_RightArrow(Editor* ed, HWND hWnd)
{
	GNode* node = (GNode*)ed->_current_glist->_current;

	if (!node)
		return;

	if (!node->_next)
	{
		GList* parent = ed->_current_glist->_parent;
		if (parent)
		{
			if (parent->_current->_pGItem->_type == GITEM_POWER)
			{
				ed->_current_glist = parent;
				ed->_current_glist->_current = ed->_current_glist->_current->_next;
			}
			else if (parent->_current->_pGItem->_type == GITEM_FRACTION)
			{
				if (ed->_current_glist != ((GItemFraction*)parent->_current->_pGItem)->_denominator)
				{
					ed->_current_glist = ((GItemFraction*)parent->_current->_pGItem)->_denominator;
					ed->_current_glist->_current = ed->_current_glist->_front;
				}
				else
				{
					ed->_current_glist = parent;
					ed->_current_glist->_current = ed->_current_glist->_current->_next;
				}
			}
			else if (parent->_current->_pGItem->_type == GITEM_ROOT)
			{
				if (ed->_current_glist != ((GItemRoot*)parent->_current->_pGItem)->_expr)
				{
					ed->_current_glist = ((GItemRoot*)parent->_current->_pGItem)->_expr;
					ed->_current_glist->_current = ed->_current_glist->_front;
				}
				else
				{
					ed->_current_glist = parent;
					ed->_current_glist->_current = ed->_current_glist->_current->_next;
				}
			}
		}
	}
	else
	{
		if (node->_pGItem->_type == GITEM_CHAR)
		{
			ed->_current_glist->_current = node->_next;
		}
		else if (node->_pGItem->_type == GITEM_POWER)
		{
			ed->_current_glist = ((GItemPower*)node->_pGItem)->_exponent;
			ed->_current_glist->_current = ed->_current_glist->_front;
		}
		else if (node->_pGItem->_type == GITEM_FRACTION)
		{
			ed->_current_glist = ((GItemFraction*)node->_pGItem)->_numerator;
			ed->_current_glist->_current = ed->_current_glist->_front;
		}
		else if (node->_pGItem->_type == GITEM_ROOT)
		{
			ed->_current_glist = ((GItemRoot*)node->_pGItem)->_root;
			ed->_current_glist->_current = ed->_current_glist->_front;
		}
	}

	Editor_UpdateCaret(ed, hWnd);
}

void Editor_OnChar_Default(Editor* ed, wchar_t ch, HWND hWnd)
{
	if (ch == L':')
	{
		GList* root = GList_init(ed->_current_glist);
		GList* expr = GList_init(ed->_current_glist);

		GList_pushback(root, (GItem*)GItemChar_init(0));
		GList_pushback(expr, (GItem*)GItemChar_init(0));

		GItemRoot* g = GItemRoot_init(expr, root);
		GNode* node = GNode_init((GItem*)g, ed->_current_glist->_current, ed->_current_glist->_current->_prev);
		if (ed->_current_glist->_current->_prev)
			ed->_current_glist->_current->_prev->_next = node;
		else
		{
			ed->_current_glist->_front = node;
		}
		ed->_current_glist->_current->_prev = node;
		ed->_current_glist->_current = node;

		// Move Cursor right
		Editor_OnKey_RightArrow(ed, hWnd);
	}
	else if (ch == L'/')
	{
		GList* num = GList_init(ed->_current_glist);
		GList* den = GList_init(ed->_current_glist);
		GList_pushback(num, (GItem*)GItemChar_init(0));
		GList_pushback(den, (GItem*)GItemChar_init(0));

		GItemFraction* g = GItemFraction_init(num, den);
		GNode* node = GNode_init((GItem*)g, ed->_current_glist->_current, ed->_current_glist->_current->_prev);
		if (ed->_current_glist->_current->_prev)
			ed->_current_glist->_current->_prev->_next = node;
		else
		{
			ed->_current_glist->_front = node;
		}
		ed->_current_glist->_current->_prev = node;
		ed->_current_glist->_current = node;

		// Move Cursor right
		Editor_OnKey_RightArrow(ed, hWnd);
	}
	else if (ch == L'^')
	{
		GList* exponent = GList_init(ed->_current_glist);
		GList_pushback(exponent, (GItem*)GItemChar_init(0));
		GItemPower* g = GItemPower_init(exponent);

		GNode* node = GNode_init((GItem*)g, ed->_current_glist->_current, ed->_current_glist->_current->_prev);
		if (ed->_current_glist->_current->_prev)
			ed->_current_glist->_current->_prev->_next = node;
		else
		{
			ed->_current_glist->_front = node;
		}
		ed->_current_glist->_current->_prev = node;
		ed->_current_glist->_current = node;

		// Move Cursor right
		Editor_OnKey_RightArrow(ed, hWnd);
	}
	else
	{
		if (ch == L'*')
			ch = L'\u00D7';

		GItemChar* g = GItemChar_init(ch);
		GNode* node = GNode_init((GItem*)g, ed->_current_glist->_current, ed->_current_glist->_current->_prev);
		if (ed->_current_glist->_current->_prev)
			ed->_current_glist->_current->_prev->_next = node;
		else
		{
			ed->_current_glist->_front = node;
		}
		ed->_current_glist->_current->_prev = node;
		ed->_current_glist->_current = node->_next;
	}
}

void Editor_OnChar_Backspace(Editor* ed)
{
	GNode* node = ed->_current_glist->_current->_prev;
	if (ed->_current_glist->_current->_prev)
	{
		if (ed->_current_glist->_current->_prev->_prev)
		{
			ed->_current_glist->_current->_prev = ed->_current_glist->_current->_prev->_prev;
			ed->_current_glist->_current->_prev->_next = ed->_current_glist->_current;
		}
		else
		{
			ed->_current_glist->_front = ed->_current_glist->_current;
			ed->_current_glist->_front->_prev = NULL;
		}

		GNode_free(node);
	}
	
}

void Editor_OnChar_Return(Editor* ed)
{
}

void Editor_OnCmd(Editor* ed, int cmd, HWND hWnd)
{
	if (cmd == cmdButtonFraction)
	{
		Editor_OnChar_Default(ed, L'/', hWnd);
	}
	else if (cmd == cmdButtonPower)
	{
		Editor_OnChar_Default(ed, L'^', hWnd);
	}
	else if (cmd == cmdButtonRoot)
	{
		Editor_OnChar_Default(ed, L':', hWnd);
	}
	else if (cmd == cmdButtonSin)
	{
		Editor_OnChar_Default(ed, L'S', hWnd);
		Editor_OnChar_Default(ed, L'i', hWnd);
		Editor_OnChar_Default(ed, L'n', hWnd);
		Editor_OnChar_Default(ed, L'(', hWnd);
		Editor_OnChar_Default(ed, L')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd);
	}
	else if (cmd == cmdButtonCos)
	{
		Editor_OnChar_Default(ed, L'C', hWnd);
		Editor_OnChar_Default(ed, L'o', hWnd);
		Editor_OnChar_Default(ed, L's', hWnd);
		Editor_OnChar_Default(ed, L'(', hWnd);
		Editor_OnChar_Default(ed, L')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd);
	}
	else if (cmd == cmdButtonAsin)
	{
		Editor_OnChar_Default(ed, L'A', hWnd);
		Editor_OnChar_Default(ed, L's', hWnd);
		Editor_OnChar_Default(ed, L'i', hWnd);
		Editor_OnChar_Default(ed, L'n', hWnd);
		Editor_OnChar_Default(ed, L'(', hWnd);
		Editor_OnChar_Default(ed, L')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd);
	}
	else if (cmd == cmdButtonAcos)
	{
		Editor_OnChar_Default(ed, L'A', hWnd);
		Editor_OnChar_Default(ed, L'c', hWnd);
		Editor_OnChar_Default(ed, L'o', hWnd);
		Editor_OnChar_Default(ed, L's', hWnd);
		Editor_OnChar_Default(ed, L'(', hWnd);
		Editor_OnChar_Default(ed, L')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd);
	}
	else if (cmd == cmdButtonTan)
	{
		Editor_OnChar_Default(ed, L'T', hWnd);
		Editor_OnChar_Default(ed, L'a', hWnd);
		Editor_OnChar_Default(ed, L'n', hWnd);
		Editor_OnChar_Default(ed, L'(', hWnd);
		Editor_OnChar_Default(ed, L')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd);
	}
	else if (cmd == cmdButtonAtan)
	{
		Editor_OnChar_Default(ed, L'A', hWnd);
		Editor_OnChar_Default(ed, L'T', hWnd);
		Editor_OnChar_Default(ed, L'a', hWnd);
		Editor_OnChar_Default(ed, L'n', hWnd);
		Editor_OnChar_Default(ed, L'(', hWnd);
		Editor_OnChar_Default(ed, L')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd);
	}
	else if (cmd == cmdButtonLog)
	{
		Editor_OnChar_Default(ed, L'L', hWnd);
		Editor_OnChar_Default(ed, L'o', hWnd);
		Editor_OnChar_Default(ed, L'g', hWnd);
		Editor_OnChar_Default(ed, L'(', hWnd);
		Editor_OnChar_Default(ed, L')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd);
	}
	else if (cmd == cmdButtonLn)
	{
		Editor_OnChar_Default(ed, L'L', hWnd);
		Editor_OnChar_Default(ed, L'n', hWnd);
		Editor_OnChar_Default(ed, L'(', hWnd);
		Editor_OnChar_Default(ed, L')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd);
	}
	else if (cmd == cmdButtonExp)
	{
		Editor_OnChar_Default(ed, L'E', hWnd);
		Editor_OnChar_Default(ed, L'x', hWnd);
		Editor_OnChar_Default(ed, L'p', hWnd);
		Editor_OnChar_Default(ed, L'(', hWnd);
		Editor_OnChar_Default(ed, L')', hWnd);

		// Move Cursor left
		Editor_OnKey_LeftArrow(ed, hWnd);
	}
	else if (cmd == cmdButtonPi)
	{
		Editor_OnChar_Default(ed, L'\u03C0', hWnd);
	}
	else if (cmd == cmdButtonEuler)
	{
		Editor_OnChar_Default(ed, L'e', hWnd);
	}
}

void Editor_OnChar_Delete(Editor* ed)
{
}
