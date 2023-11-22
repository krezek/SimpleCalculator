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
void rgl_9(Item** item, int* pctr);
void rgl_10(Item** item, int* pctr);
void rgl_11(Item** item, int* pctr);
void rgl_12(Item** item, int* pctr);
void rgl_13(Item** item, int* pctr);
void rgl_14(Item** item, int* pctr);
void rgl_15(Item** item, int* pctr);
void rgl_16(Item** item, int* pctr);
void rgl_17(Item** item, int* pctr);
void rgl_18(Item** item, int* pctr);
void rgl_100(Item** item, int* pctr);

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
	{ L"+(/(I,I),/(I,I))", rgl_8 },
	{ L"-(/(I,I),I)", rgl_9 },
	{ L"-(I,/(I,I))", rgl_10 },
	{ L"-(/(I,I),/(I,I))", rgl_11 },
	{ L"*(/(I,I),I)", rgl_12 },
	{ L"*(I,/(I,I))", rgl_13 },
	{ L"*(/(I,I),/(I,I))", rgl_14 },
	{ L"/(/(I,I),I)", rgl_15 },
	{ L"/(I,/(I,I))", rgl_16 },
	{ L"/(/(I,I),/(I,I))", rgl_17 },
	{ L"Sign(/(I,I),NULL)", rgl_18 },

	{ L"^(I,I)", rgl_100 }
};

int Fraction_Simplify(ItemFrac* i)
{
	assert(i->_item._left->_objectType == OBJ_Integer);
	assert(i->_item._right->_objectType == OBJ_Integer);

	mpz_t gcd, div1, div2;

	mpz_init(gcd);
	mpz_init(div1);
	mpz_init(div2);

	mpz_gcd(gcd, 
		((ItemInteger*)i->_item._left)->_value, 
		((ItemInteger*)i->_item._right)->_value);

	int rt = mpz_cmp_ui(gcd, 1);

	mpz_div(div1, ((ItemInteger*)i->_item._left)->_value, gcd);
	mpz_div(div2, ((ItemInteger*)i->_item._right)->_value, gcd);

	mpz_set(((ItemInteger*)i->_item._left)->_value, div1);
	mpz_set(((ItemInteger*)i->_item._right)->_value, div2);

	mpz_clear(gcd);
	mpz_clear(div1);
	mpz_clear(div2);

	return rt;
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

		ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init_str(s1->_str),
			(Item*)ItemInteger_init_str(s2->_str));
		Fraction_Simplify(n);
		*item = (Item*)n;
		String_free(s1);
		String_free(s2);
	}
	else
	{
		*item = (Item*)ItemInteger_init_str(i->_str->_str);
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
	if (i->_sgn == L'-')
	{
		mpz_t a;
		mpz_init(a);

		mpz_neg(a, ((ItemInteger*)i->_item._left)->_value);
		*item = (Item*)ItemInteger_init(a);
		mpz_clear(a);
	}
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

	mpz_t a;
	mpz_init(a);

	mpz_add(a,
		((ItemInteger*)i->_item._left)->_value,
		((ItemInteger*)i->_item._right)->_value);

	*item = (Item*)ItemInteger_init(a);

	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: -(I,I)
void rgl_3(Item** item, int* pctr)
{
	printf("apply: rgl_3\n");
	assert((*item)->_objectType == OBJ_Sub);

	Item* tmp = *item;
	ItemSub* i = (ItemSub*)*item;
	
	mpz_t a;
	mpz_init(a);

	mpz_sub(a,
		((ItemInteger*)i->_item._left)->_value,
		((ItemInteger*)i->_item._right)->_value);

	*item = (Item*)ItemInteger_init(a);

	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: *(I,I)
void rgl_4(Item** item, int* pctr)
{
	printf("apply: rgl_4\n");
	assert((*item)->_objectType == OBJ_Mult);

	Item* tmp = *item;
	ItemMult* i = (ItemMult*)*item;
	
	mpz_t a;
	mpz_init(a);

	mpz_mul(a,
		((ItemInteger*)i->_item._left)->_value,
		((ItemInteger*)i->_item._right)->_value);

	*item = (Item*)ItemInteger_init(a);

	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: /(I,I)
void rgl_5(Item** item, int* pctr)
{
	printf("apply: rgl_5\n");
	assert((*item)->_objectType == OBJ_Frac);
	assert((*item)->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Integer);

	int gcd = Fraction_Simplify((ItemFrac*)*item);
	if (mpz_cmp_ui(((ItemInteger*)(*item)->_right)->_value, 1) == 0)
	{
		Item* tmp = *item;
		ItemInteger* n = ItemInteger_init(((ItemInteger*)(*item)->_left)->_value);
		*item = (Item*)n;
		ItemTree_free(&tmp);
		*pctr += 1;
	}

	if (gcd)
		*pctr += 1;
}

// Rgl: +(/(I,I),I)
void rgl_6(Item** item, int* pctr)
{
	printf("apply: rgl_6\n");
	assert((*item)->_objectType == OBJ_Add);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_left->_left->_objectType == OBJ_Integer);
	assert((*item)->_left->_right->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Integer);

	ItemInteger* i1 = (ItemInteger*)(*item)->_left->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_left->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_right);

	mpz_t a;
	mpz_init(a);

	mpz_mul(a, i2->_value, i3->_value);
	mpz_add(a, a, i1->_value);
	
	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(i2->_value));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: +(I,/(I,I))
void rgl_7(Item** item, int* pctr)
{
	printf("apply: rgl_7\n");
	assert((*item)->_objectType == OBJ_Add);
	assert((*item)->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Frac);
	assert((*item)->_right->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_right->_objectType == OBJ_Integer);


	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left);

	mpz_t a;
	mpz_init(a);

	mpz_mul(a, i2->_value, i3->_value);
	mpz_add(a, a, i1->_value);

	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(i2->_value));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: +(/(I,I),/(I,I))
void rgl_8(Item** item, int* pctr)
{
	printf("apply: rgl_8\n");
	assert((*item)->_objectType == OBJ_Add);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_left->_left->_objectType == OBJ_Integer);
	assert((*item)->_left->_right->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Frac);
	assert((*item)->_right->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_right->_objectType == OBJ_Integer);

	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left->_left);
	ItemInteger* i4 = (ItemInteger*)((*item)->_left->_right);

	mpz_t a, a1, a2, b;
	mpz_init(a);
	mpz_init(a1);
	mpz_init(a2);
	mpz_init(b);

	mpz_mul(a1, i2->_value, i3->_value);
	mpz_mul(a2, i1->_value, i4->_value);
	mpz_add(a, a1, a2);
	mpz_mul(b, i2->_value, i4->_value);

	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(b));
	Fraction_Simplify(n);
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);
	mpz_clear(a1);
	mpz_clear(a2);
	mpz_clear(b);

	*pctr += 1;
}

// Rgl: -(/(I,I),I)
void rgl_9(Item** item, int* pctr)
{
	printf("apply: rgl_9\n");
	assert((*item)->_objectType == OBJ_Sub);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_left->_left->_objectType == OBJ_Integer);
	assert((*item)->_left->_right->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Integer);

	ItemInteger* i1 = (ItemInteger*)(*item)->_left->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_left->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_right);

	mpz_t a;
	mpz_init(a);

	mpz_mul(a, i2->_value, i3->_value);
	mpz_sub(a, i1->_value, a);

	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(i2->_value));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: -(I,/(I,I))
void rgl_10(Item** item, int* pctr)
{
	printf("apply: rgl_10\n");
	assert((*item)->_objectType == OBJ_Sub);
	assert((*item)->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Frac);
	assert((*item)->_right->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_right->_objectType == OBJ_Integer);


	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left);

	mpz_t a;
	mpz_init(a);

	mpz_mul(a, i2->_value, i3->_value);
	mpz_sub(a, a, i1->_value);

	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(i2->_value));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: -(/(I,I),/(I,I))
void rgl_11(Item** item, int* pctr)
{
	printf("apply: rgl_11\n");
	assert((*item)->_objectType == OBJ_Sub);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_left->_left->_objectType == OBJ_Integer);
	assert((*item)->_left->_right->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Frac);
	assert((*item)->_right->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_right->_objectType == OBJ_Integer);

	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left->_left);
	ItemInteger* i4 = (ItemInteger*)((*item)->_left->_right);

	mpz_t a, a1, a2, b;
	mpz_init(a);
	mpz_init(a1);
	mpz_init(a2);
	mpz_init(b);

	mpz_mul(a1, i2->_value, i3->_value);
	mpz_mul(a2, i1->_value, i4->_value);
	mpz_sub(a, a1, a2);
	mpz_mul(b, i2->_value, i4->_value);

	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(b));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);
	mpz_clear(a1);
	mpz_clear(a2);
	mpz_clear(b);

	*pctr += 1;
}

// Rgl: *(/(I,I),I)
void rgl_12(Item** item, int* pctr)
{
	printf("apply: rgl_12\n");
	assert((*item)->_objectType == OBJ_Mult);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_left->_left->_objectType == OBJ_Integer);
	assert((*item)->_left->_right->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Integer);

	ItemInteger* i1 = (ItemInteger*)(*item)->_left->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_left->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_right);

	mpz_t a;
	mpz_init(a);

	mpz_mul(a, i1->_value, i3->_value); 
	
	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(i2->_value));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: *(I,/(I,I))
void rgl_13(Item** item, int* pctr)
{
	printf("apply: rgl_13\n");
	assert((*item)->_objectType == OBJ_Mult);
	assert((*item)->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Frac);
	assert((*item)->_right->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_right->_objectType == OBJ_Integer);


	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left);

	mpz_t a;
	mpz_init(a);

	mpz_mul(a, i1->_value, i3->_value); 
	
	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(i2->_value));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: *(/(I,I),/(I,I))
void rgl_14(Item** item, int* pctr)
{
	printf("apply: rgl_14\n");
	assert((*item)->_objectType == OBJ_Mult);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_left->_left->_objectType == OBJ_Integer);
	assert((*item)->_left->_right->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Frac);
	assert((*item)->_right->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_right->_objectType == OBJ_Integer);

	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left->_left);
	ItemInteger* i4 = (ItemInteger*)((*item)->_left->_right);

	mpz_t a, b;
	mpz_init(a);
	mpz_init(b);

	mpz_mul(a, i1->_value, i3->_value);
	mpz_mul(b, i2->_value, i4->_value);
	
	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(b));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);
	mpz_clear(b); 
	
	*pctr += 1;
}

// Rgl: /(/(I,I),I)
void rgl_15(Item** item, int* pctr)
{
	printf("apply: rgl_15\n");
	assert((*item)->_objectType == OBJ_Frac);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_left->_left->_objectType == OBJ_Integer);
	assert((*item)->_left->_right->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Integer);

	ItemInteger* i1 = (ItemInteger*)(*item)->_left->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_left->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_right);

	mpz_t b;
	mpz_init(b);

	mpz_mul(b, i2->_value, i3->_value); 
	
	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(i1->_value),
		(Item*)ItemInteger_init(b));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(b);

	*pctr += 1;
}

// Rgl: /(I,/(I,I))
void rgl_16(Item** item, int* pctr)
{
	printf("apply: rgl_16\n");
	assert((*item)->_objectType == OBJ_Frac);
	assert((*item)->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Frac);
	assert((*item)->_right->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_right->_objectType == OBJ_Integer);


	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left);

	mpz_t a;
	mpz_init(a);

	mpz_mul(a, i2->_value, i3->_value);

	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(i1->_value));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);

	*pctr += 1;
}

// Rgl: /(/(I,I),/(I,I))
void rgl_17(Item** item, int* pctr)
{
	printf("apply: rgl_17\n");
	assert((*item)->_objectType == OBJ_Frac);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_left->_left->_objectType == OBJ_Integer);
	assert((*item)->_left->_right->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Frac);
	assert((*item)->_right->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_right->_objectType == OBJ_Integer);

	ItemInteger* i1 = (ItemInteger*)(*item)->_right->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_right->_right;
	ItemInteger* i3 = (ItemInteger*)((*item)->_left->_left);
	ItemInteger* i4 = (ItemInteger*)((*item)->_left->_right);

	mpz_t a, b;
	mpz_init(a);
	mpz_init(b);

	mpz_mul(a, i2->_value, i3->_value);
	mpz_mul(b, i1->_value, i4->_value);

	Item* tmp = *item;
	ItemFrac* n = ItemFrac_init((Item*)ItemInteger_init(a),
		(Item*)ItemInteger_init(b));
	*item = (Item*)n;
	ItemTree_free(&tmp);
	mpz_clear(a);
	mpz_clear(b);

	*pctr += 1;
}

// Rgl: Sign(/(I,I),NULL)
void rgl_18(Item** item, int* pctr)
{
	printf("apply: rgl_18\n");
	assert((*item)->_objectType == OBJ_Sign);
	assert((*item)->_left->_objectType == OBJ_Frac);
	assert((*item)->_left->_left->_objectType == OBJ_Integer);
	assert((*item)->_left->_right->_objectType == OBJ_Integer);
	assert((*item)->_right == NULL);

	ItemInteger* i1 = (ItemInteger*)(*item)->_left->_left;
	ItemInteger* i2 = (ItemInteger*)(*item)->_left->_right;

	Item* tmp = *item;
	ItemFrac* n;
	if (((ItemSign*)(*item))->_sgn == L'-')
	{
		mpz_t a;
		mpz_init(a);

		mpz_neg(a, i1->_value);

		n = ItemFrac_init((Item*)ItemInteger_init(a),
			(Item*)ItemInteger_init(i2->_value));

		mpz_clear(a);
	}
	else
		n = ItemFrac_init((Item*)ItemInteger_init(i1->_value),
			(Item*)ItemInteger_init(i2->_value));
	*item = (Item*)n;
	ItemTree_free(&tmp);

	*pctr += 1;
}

/*__int64 pow64(__int64 base, __int64 exponent)
{
	__int64 rt = 1;
	for (__int64 ix = exponent; ix > 0; --ix)
	{
		rt *= base;
	}

	return rt;
}*/

// Rgl: ^(I,I)
void rgl_100(Item** item, int* pctr)
{
	/*printf("apply: rgl_100\n");
	assert((*item)->_objectType == OBJ_Pow);
	assert((*item)->_left->_objectType == OBJ_Integer);
	assert((*item)->_right->_objectType == OBJ_Integer);
	
	ItemInteger* i = (ItemInteger*)(*item)->_left;
	ItemInteger* p = (ItemInteger*)(*item)->_right;

	if (p->_value > 0)
	{
		Item* tmp = *item;
		*item = (Item*)ItemInteger_init(pow64(i->_value, p->_value));
		ItemTree_free(&tmp);
	}
	else if (p->_value < 0)
	{
		Item* tmp = *item;
		*item = (Item*)ItemFrac_init((Item*)ItemInteger_init(1),
			(Item*)ItemInteger_init(pow64(i->_value, -1 * p->_value)));
		ItemTree_free(&tmp);
	}
	else
	{
		Item* tmp = *item;
		*item = (Item*)ItemInteger_init(1);
		ItemTree_free(&tmp);
	}

	*pctr += 1;*/
}
