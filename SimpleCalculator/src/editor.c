#include "platform.h"

#include "editor.h"
#include "ids.h"

static void OnInit(Editor* ed, HWND hWnd, GList* gll);
static void OnStopEditing(Editor* ed);

static void UpdateCaret(Editor* ed);
static void OnSetFocus(Editor* ed);
static void OnKillFocus(Editor* ed);

static void OnKey_LeftArrow(Editor* ed);
static void OnKey_RightArrow(Editor* ed);
static void OnChar_Default(Editor* ed, wchar_t ch);
static void OnChar_Backspace(Editor* ed);
static void OnChar_Return(Editor* ed);
static void OnChar_Delete(Editor* ed);

static void OnCmd(Editor* ed, int cmd);

Editor* Editor_init()
{
	Editor* ed = (Editor*)malloc(sizeof(Editor));
	assert(ed != NULL);
	
	ed->_OnInitFunc = OnInit;
	ed->_OnStopEditingFunc = OnStopEditing;

	ed->_OnSetFocusFunc = OnSetFocus;
	ed->_OnKillFocusFunc = OnKillFocus;
	ed->_OnUpdateCaret = UpdateCaret;

	ed->_OnKey_LeftArrowFunc = OnKey_LeftArrow;
	ed->_OnKey_RightArrowFunc = OnKey_RightArrow;
	ed->_OnChar_DefaultFunc = OnChar_Default;
	ed->_OnChar_BackspaceFunc = OnChar_Backspace;
	ed->_OnChar_ReturnFunc = OnChar_Return;
	ed->_OnChar_DeleteFunc = OnChar_Delete;

	ed->_OnCmdFunc = OnCmd;

	return ed;
}

void Editor_free(Editor* ed)
{
	/*if (ed->_itemsOrder)
	{
		LinkedList_free(ed->_itemsOrder);
		ed->_itemsOrder = NULL;
	}
	*/
	free(ed);
}

static void OnInit(Editor* ed, HWND hWnd, GList* gll)
{
	ed->_hWnd = hWnd;
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

static void OnStopEditing(Editor* ed)
{

}


static void UpdateCaret(Editor* ed)
{
	if (!ed->_current_glist->_current)
		return;

	CreateCaret(ed->_hWnd, (HBITMAP)NULL, 2, ed->_current_glist->_current->_pGItem->_height);
	SetCaretPos(ed->_current_glist->_current->_pGItem->_x0, ed->_current_glist->_current->_pGItem->_y0);
	ShowCaret(ed->_hWnd);
	
}

static void OnSetFocus(Editor* ed)
{
	UpdateCaret(ed);
}

static void OnKillFocus(Editor* ed)
{
	DestroyCaret();
}

static void OnKey_LeftArrow(Editor* ed)
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

	UpdateCaret(ed);
}

static void OnKey_RightArrow(Editor* ed)
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

	UpdateCaret(ed);
}

static void OnChar_Default(Editor* ed, wchar_t ch)
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
		OnKey_RightArrow(ed);
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
		OnKey_RightArrow(ed);
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
		OnKey_RightArrow(ed);
	}
	else
	{
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

static void OnChar_Backspace(Editor* ed)
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

static void OnChar_Return(Editor* ed)
{
}

static void OnCmd(Editor* ed, int cmd)
{
	if (cmd == cmdButtonFraction)
	{
		OnChar_Default(ed, L'/');
	}
	else if (cmd == cmdButtonPower)
	{
		OnChar_Default(ed, L'^');
	}
	else if (cmd == cmdButtonRoot)
	{
		OnChar_Default(ed, L':');
	}
	else if (cmd == cmdButtonSin)
	{
		OnChar_Default(ed, L'S');
		OnChar_Default(ed, L'i');
		OnChar_Default(ed, L'n');
		OnChar_Default(ed, L'(');
		OnChar_Default(ed, L')');

		// Move Cursor left
		OnKey_LeftArrow(ed);
	}
	else if (cmd == cmdButtonCos)
	{
		OnChar_Default(ed, L'C');
		OnChar_Default(ed, L'o');
		OnChar_Default(ed, L's');
		OnChar_Default(ed, L'(');
		OnChar_Default(ed, L')');

		// Move Cursor left
		OnKey_LeftArrow(ed);
	}
	else if (cmd == cmdButtonAsin)
	{
		OnChar_Default(ed, L'A');
		OnChar_Default(ed, L's');
		OnChar_Default(ed, L'i');
		OnChar_Default(ed, L'n');
		OnChar_Default(ed, L'(');
		OnChar_Default(ed, L')');

		// Move Cursor left
		OnKey_LeftArrow(ed);
	}
	else if (cmd == cmdButtonAcos)
	{
		OnChar_Default(ed, L'A');
		OnChar_Default(ed, L'c');
		OnChar_Default(ed, L'o');
		OnChar_Default(ed, L's');
		OnChar_Default(ed, L'(');
		OnChar_Default(ed, L')');

		// Move Cursor left
		OnKey_LeftArrow(ed);
	}
	else if (cmd == cmdButtonTan)
	{
		OnChar_Default(ed, L'T');
		OnChar_Default(ed, L'a');
		OnChar_Default(ed, L'n');
		OnChar_Default(ed, L'(');
		OnChar_Default(ed, L')');

		// Move Cursor left
		OnKey_LeftArrow(ed);
	}
	else if (cmd == cmdButtonAtan)
	{
		OnChar_Default(ed, L'A');
		OnChar_Default(ed, L'T');
		OnChar_Default(ed, L'a');
		OnChar_Default(ed, L'n');
		OnChar_Default(ed, L'(');
		OnChar_Default(ed, L')');

		// Move Cursor left
		OnKey_LeftArrow(ed);
	}
	else if (cmd == cmdButtonLog)
	{
		OnChar_Default(ed, L'L');
		OnChar_Default(ed, L'o');
		OnChar_Default(ed, L'g');
		OnChar_Default(ed, L'(');
		OnChar_Default(ed, L')');

		// Move Cursor left
		OnKey_LeftArrow(ed);
	}
	else if (cmd == cmdButtonLn)
	{
		OnChar_Default(ed, L'L');
		OnChar_Default(ed, L'n');
		OnChar_Default(ed, L'(');
		OnChar_Default(ed, L')');

		// Move Cursor left
		OnKey_LeftArrow(ed);
	}
	else if (cmd == cmdButtonExp)
	{
		OnChar_Default(ed, L'E');
		OnChar_Default(ed, L'x');
		OnChar_Default(ed, L'p');
		OnChar_Default(ed, L'(');
		OnChar_Default(ed, L')');

		// Move Cursor left
		OnKey_LeftArrow(ed);
	}
	else if (cmd == cmdButtonPi)
	{
	OnChar_Default(ed, L'P');
	OnChar_Default(ed, L'i');
	}
	else if (cmd == cmdButtonEuler)
	{
	OnChar_Default(ed, L'e');
	}
}

static void OnChar_Delete(Editor* ed)
{
}
