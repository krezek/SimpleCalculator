#include "platform.h"

#include <simplify.h>
#include <items.h>

extern void parse_items(Item** pItems, const wchar_t* s);

wchar_t* do_simplify(const wchar_t* exp)
{
	Item* items = NULL;
	parse_items(&items, exp);

	if (items)
	{
		String* str = String_init();

		items->_toStringFunc(items, str);
		wchar_t* rt = (wchar_t*)malloc((str->_len + 1) * sizeof(wchar_t));
		assert(rt != NULL);
		wcscpy(rt, str->_str);
		String_free(str);

		ItemTree_free(&items);

		return rt;
	}
	

	return NULL;
}
