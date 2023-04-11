#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <gitems.h>

typedef struct _Editor
{
	GList* _in_gitems_list;
	GList* _current_glist;
} Editor;

Editor* Editor_init();
void Editor_free(Editor* ed);

void Editor_OnInit(Editor* ed, GList* gll);
void Editor_UpdateCaret(Editor* ed, HWND hWnd);
void Editor_OnSetFocus(Editor* ed, HWND hWnd);
void Editor_OnKillFocus(Editor* ed);

void Editor_OnKey_LeftArrow(Editor* ed, HWND hWnd);
void Editor_OnKey_RightArrow(Editor* ed, HWND hWnd);
void Editor_OnChar_Default(Editor* ed, wchar_t ch, HWND hWnd);
void Editor_OnChar_Backspace(Editor* ed);
void Editor_OnChar_Return(Editor* ed);
void Editor_OnChar_Delete(Editor* ed);

void Editor_OnCmd(Editor* ed, int cmd, HWND hWnd);

#endif /* _EDITOR_H_ */

