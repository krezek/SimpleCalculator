#include "platform.h"

#include <items_ext.h>
#include <regel.h>

void rgl_0(Item** item, int* pctr);
void rgl_1(Item** item, int* pctr);
void rgl_2(Item** item, int* pctr);
void rgl_3(Item** item, int* pctr);
void rgl_4(Item** item, int* pctr);
void rgl_5(Item** item, int* pctr);
void rgl_6(Item** item, int* pctr);
void rgl_7(Item** item, int* pctr);
void rgl_8(Item** item, int* pctr);

struct Regels g_regles[REGEL_COUNT] =
{
	{ L"N", rgl_0 },
	{ L"Sign(I,NULL)", rgl_1 },
	{ L"+(I,I)", rgl_2 },
	{ L"-(I,I)", rgl_3 },
	{ L"*(I,I)", rgl_4 },
	{ L"/(I,I)", rgl_5 },
	{ L"+(/(I,I),I)", rgl_6 },
	{ L"+(I,/(I,I))", rgl_7 },
	{ L"+(/(I,I),/(I,I))", rgl_8 }
};

__int64 _gcd(__int64 a, __int64 b)
{
	__int64 temp;
	while (b != 0)
	{
		temp = a % b;

		a = b;
		b = temp;
	}
	return a;
}

__int64 Fraction_Simplify(ItemFrac* i)
{
	assert(i->_item._left->_objectType == OBJ_Integer);
	assert(i->_item._right->_objectType == OBJ_Integer);

	__int64 gcd = _gcd(((ItemInteger*)i->_item._left)->_value,
		((ItemInteger*)i->_item._right)->_value);
	((ItemInteger*)i->_item._left)->_value = ((ItemInteger*)i->_item._left)->_value / gcd;
	((ItemInteger*)i->_item._right)->_value = ((ItemInteger*)i->_item._right)->_value / gcd;

	return gcd;
}

// Rgl: N
void rgl_0(Item** item, int* pctr)
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
		ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(_wtoi64(s1->_str)),
			(Item*)ItemInteger_init(_wtoi64(s2->_str)));
		Fraction_Simplify(n);
		*item = (Item*)n;
		String_free(s1);
		String_free(s2);
	}
	else
	{
		*item = (Item*)ItemInteger_init(_wtoi64(i->_str->_str));
	}

	ItemTree_free(&tmp);

	*pctr += 1;
}

// Rgl: Sign(I,NULL)
void rgl_1(Item** item, int* pctr)
{
	printf("apply: rgl_1\n");
	assert((*item)->_objectType == OBJ_Sign);
	assert((*item)->_left->_objectType == OBJ_Integer);
	assert((*item)->_right == NULL);

	Item* tmp = *item;
	ItemSign* i = (ItemSign*)*item;
	if(i->_sgn == L'-')
		*item = (Item*)ItemInteger_init(-1 * ((ItemInteger*)i->_item._left)->_value);
	else
		*item = (Item*)ItemInteger_init(((ItemInteger*)i->_item._left)->_value);

	ItemTree_free(&tmp);

	*pctr += 1;
}

// Rgl: +(I,I)
void rgl_2(Item** item, int* pctr)
{
	printf("apply: rgl_2\n");
	assert((*item)->_objectType == OBJ_Add);

	Item* tmp = *item;
	ItemAdd* i = (ItemAdd*)*item;
	*item = (Item*)ItemInteger_init(((ItemInteger*)i->_item._left)->_value +
		((ItemInteger*)i->_item._right)->_value);

	ItemTree_free(&tmp);

	*pctr += 1;
}

// Rgl: -(I,I)
void rgl_3(Item** item, int* pctr)
{
	printf("apply: rgl_3\n");
	assert((*item)->_objectType == OBJ_Sub);

	Item* tmp = *item;
	ItemSub* i = (ItemSub*)*item;
	*item = (Item*)ItemInteger_init(((ItemInteger*)i->_item._left)->_value -
		((ItemInteger*)i->_item._right)->_value);

	ItemTree_free(&tmp);

	*pctr += 1;
}

// Rgl: *(I,I)
void rgl_4(Item** item, int* pctr)
{
	printf("apply: rgl_4\n");
	assert((*item)->_objectType == OBJ_Mult);

	Item* tmp = *item;
	ItemMult* i = (ItemMult*)*item;
	*item = (Item*)ItemInteger_init(((ItemInteger*)i->_item._left)->_value *
		((ItemInteger*)i->_item._right)->_value);

	ItemTree_free(&tmp);

	*pctr += 1;
}

// Rgl: /(I,I)
void rgl_5(Item** item, int* pctr)
{
	printf("apply: rgl_5\n");
	assert((*item)->_objectType == OBJ_Frac);
	assert((*item)->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Integer);

	__int64 gcd = Fraction_Simplify((ItemFrac*)*item);
	if (((ItemInteger*)(*item)->_right)->_value == 1)
	{
		Item* tmp = *item;
		ItemInteger* n = ItemInteger_init(((ItemInteger*)(*item)->_left)->_value);
		*item = (Item*)n;
		ItemTree_free(&tmp);
		*pctr += 1;
	}

	if (gcd != 1)
		*pctr += 1;
}

// Rgl: +(/(I,I),I)
void rgl_6(Item** item, int* pctr)
{
	printf("apply: rgl_6\n");
	assert((*item)->_objectType == OBJ_Add);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_right->_objectType == OBJ_Integer);

	ItemInteger* i1 = (ItemInteger*)(*item)->_left->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_left->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_right);
	
	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(i2->_value * i3->_value + i1->_value),
		(Item*)ItemInteger_init(i2->_value));
	Fraction_Simplify(n);
	*item = (Item*)n;
	ItemTree_free(&tmp);

	*pctr += 1;
}

// Rgl: +(I,/(I,I))
void rgl_7(Item** item, int* pctr)
{
	printf("apply: rgl_7\n");
	assert((*item)->_objectType == OBJ_Add);
	assert((*item)->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Frac);

	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left);

	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(i2->_value * i3->_value + i1->_value),
		(Item*)ItemInteger_init(i2->_value));
	Fraction_Simplify(n);
	*item = (Item*)n;
	ItemTree_free(&tmp);

	*pctr += 1;
}

// Rgl: +(/(I,I),/(I,I))
void rgl_8(Item** item, int* pctr)
{
	printf("apply: rgl_8\n");
	assert((*item)->_objectType == OBJ_Add);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_right->_objectType == OBJ_Frac);

	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left->_left);
	ItemInteger* i4 = (ItemInteger*)((*item)->_left->_right);

	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(i2->_value * i3->_value + i1->_value * i4->_value),
		(Item*)ItemInteger_init(i2->_value * i4->_value));
	Fraction_Simplify(n);
	*item = (Item*)n;
	ItemTree_free(&tmp);

	*pctr += 1;
}

