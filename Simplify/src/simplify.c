#include "platform.h"

#include <simplify.h>
#include <items.h>

extern void parse_items(Item** pItems, const wchar_t* s);

wchar_t* do_simplify(const wchar_t* exp)
{
	Item* items = NULL;
	parse_items(&items, L"1+2*3+4");

	if (items)
	{
		{
			String* s = String_init();
			items->_writeRegelFunc(items, s, 20);
			printf("Regel(%d):%S\n", 0, s->_str);

			String_free(s);
		}


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
