#include "platform.h"

#include <simplify.h>
#include <items.h>

extern void parse_items(Item** pItems, const wchar_t* s);

wchar_t* do_simplify(const wchar_t* exp)
{
	Item* items = NULL;
	parse_items(&items, L"2*3+4");

	if (items)
	{
		int level = 0;
		Item_getLevelCount(items, &level);
		for (int ix = 1; ix <= level; ++ix)
		{
			String* s = String_init();
			items->_writeRegelFunc(items, s, ix);
			printf("Regel(%d):%S\n", ix, s->_str);

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
