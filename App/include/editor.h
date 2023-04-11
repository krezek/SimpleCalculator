#ifndef _EDITOR_H_
#define _EDITOR_H_

#include <gitems.h>

typedef struct _Editor Editor;

typedef void (*OnEditorInitializeFunc) (Editor* ed, GList* gll);
typedef void (*OnStopEditingFunc) (Editor* ed);

typedef void (*OnEditorSetFocusFunc)(Editor* ed, HWND hWnd);
typedef void (*OnEditorKillFocusFunc)(Editor* ed);
typedef void (*OnEditorUpdateCaretFunc)(Editor* ed, HWND hWnd);

typedef void (*OnKey_EditorLeftArrowFunc)(Editor* ed, HWND hWnd);
typedef void (*OnKey_EditorRightArrowFunc)(Editor* ed, HWND hWnd);
typedef void (*OnChar_EditorDefaultFunc)(Editor* ed, wchar_t ch, HWND hWnd);
typedef void (*OnChar_EditorBackspaceFunc)(Editor* ed);
typedef void (*OnChar_EditorReturnFunc)(Editor* ed);
typedef void (*OnChar_EditorDeleteFunc)(Editor* ed);

typedef void (*OnEditor_CmdFunc)(Editor* ed, int cmd, HWND hWnd);

typedef struct _Editor
{
	GList* _in_gitems_list;
	GList* _current_glist;

	OnEditorInitializeFunc _OnInitFunc;
	OnStopEditingFunc _OnStopEditingFunc;

	OnEditorSetFocusFunc _OnSetFocusFunc;
	OnEditorKillFocusFunc _OnKillFocusFunc;
	OnEditorUpdateCaretFunc _OnUpdateCaret;

	OnKey_EditorLeftArrowFunc _OnKey_LeftArrowFunc;
	OnKey_EditorRightArrowFunc _OnKey_RightArrowFunc;
	OnChar_EditorDefaultFunc _OnChar_DefaultFunc;
	OnChar_EditorBackspaceFunc _OnChar_BackspaceFunc;
	OnChar_EditorReturnFunc _OnChar_ReturnFunc;
	OnChar_EditorDeleteFunc _OnChar_DeleteFunc;

	OnEditor_CmdFunc _OnCmdFunc;
} Editor;

Editor* Editor_init();
void Editor_free(Editor* ed);


#endif /* _EDITOR_H_ */

