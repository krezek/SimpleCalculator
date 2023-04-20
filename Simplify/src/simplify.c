#include "platform.h"

#include <simplify.h>
#include <items.h>
#include <regel.h>

extern void parse_items(Item** pItems, const wchar_t* s);

static void Simplify_findRegel(Item** item, int* pctr);
static void Simplify_applyRegel(Item** item, const wchar_t* rgl, int* pctr);

wchar_t* do_simplify(const wchar_t* exp)
{
	Item* item = NULL;
	parse_items(&item, exp);

	if (item)
	{
		int counter = 1;
		while (counter)
		{
			counter = 0;
			Simplify_findRegel(&item, &counter);
		}

		String* str = String_init();

		item->_toStringFunc(item, str);
		wchar_t* rt = (wchar_t*)malloc((str->_len + 1) * sizeof(wchar_t));
		assert(rt != NULL);
		wcscpy(rt, str->_str);
		String_free(str);

		ItemTree_free(&item);

		return rt;
	}
	

	return NULL;
}

static void Simplify_findRegel(Item** item, int* pctr)
{
	int level = 0;
	Item_getLevelCount(*item, &level);
	for (int ix = 0; ix <= level; ++ix)
	{
		String* s = String_init();
		(*item)->_writeRegelFunc(*item, s, ix);
		Simplify_applyRegel(item, s->_str, pctr);
		String_free(s);
	}
	
	Item** left = &((*item)->_left);
	Item** right = &((*item)->_right);

	if (*left)
	{
		Simplify_findRegel(left, pctr);
	}

	if (*right)
	{
		Simplify_findRegel(right, pctr);
	}
}

static void Simplify_applyRegel(Item** item, const wchar_t* rgl, int* pctr)
{
	printf("Regel:%S\n", rgl);

	for (int ix = 0; ix < REGEL_COUNT; ++ix)
	{
		if (wcscmp(g_regles[ix]._rgl_str, rgl) == 0)
		{
			g_regles[ix]._rglFunc(item, pctr);
		}
	}
}
