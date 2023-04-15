#include "platform.h"

#include <items_ext.h>
#include <regel.h>

void rgl_0(Item** item);
void rgl_1(Item** item);

struct Regels g_regles[REGEL_COUNT] =
{
	{ L"N", rgl_0 },
	{ L"+(I,I)", rgl_1 }
};

// Rgl: N
void rgl_0(Item** item)
{
	printf("apply: rgl_0\n");
	assert((*item)->_objectType == OBJ_Number);

	Item* tmp = *item;
	ItemNumber* i = (ItemNumber*)*item;

	wchar_t* pptr;
	if ((pptr = wcschr(i->_str->_str, L'.')))
	{
		size_t z = wcslen(pptr + 1);
		String* s1 = String_init();
		String* s2 = String_init();
		String_cpy(s1, i->_str->_str);
		String_delete_c(s1, pptr - i->_str->_str);
		String_cpy(s2, L"1");
		while (z--)
			String_cat(s2, L"0");
		*item = (Item*)ItemFrac_init((Item*)ItemInteger_init(_wtoi64(s1->_str)),
			(Item*)ItemInteger_init(_wtoi64(s2->_str)));
		String_free(s1);
		String_free(s2);
	}
	else
	{
		*item = (Item*)ItemInteger_init(_wtoi64(i->_str->_str));
	}

	ItemTree_free(&tmp);
}

// Rgl: +(I,I)
void rgl_1(Item** item)
{
	printf("apply: rgl_1\n");
	assert((*item)->_objectType == OBJ_Add);

	Item* tmp = *item;
	ItemAdd* i = (ItemAdd*)*item;
	*item = (Item*)ItemInteger_init(((ItemInteger*)i->_item._left)->_value +
		((ItemInteger*)i->_item._right)->_value);

	ItemTree_free(&tmp);
}
