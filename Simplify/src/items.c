#include "platform.h"

#include <strg.h>
#include <items.h>

// Item

void Item_writeRegel(Item* _this, String* ins, int level, const wchar_t* opStr)
{
	if (!level)
	{
		String_cat(ins, L"expr");
		return;
	}

	String* ls = String_init();
	String* rs = String_init();

	String_cat(ins, opStr);
	String_cat(ins, L"(");
	
	if (_this->_left)
	{
		_this->_left->_writeRegelFunc(_this->_left, ls, level - 1);
		String_cat(ins, ls->_str);
	}
	else
	{
		String_cat(ins, L"NULL");
	}

	String_cat(ins, L",");

	if (_this->_right)
	{
		_this->_right->_writeRegelFunc(_this->_right, rs, level - 1);
		String_cat(ins, rs->_str);
	}
	else
	{
		String_cat(ins, L"NULL");
	}

	String_cat(ins, L")");

	String_free(rs);
	String_free(ls);
}

void Item_destroy(Item* i)
{

}

// Boolean retrun value
int Item_isLeaf(Item* i)
{
	if ((i->_left == NULL) && (i->_right == NULL))
		return 1;
	else
		return 0;
}

void Item_toString(Item* __this, String* s)
{
	String_cpy(s, L"");
}

Item* Item_init(Item* l, Item* r)
{
	Item* i = (Item*)malloc(sizeof(Item));
	assert(i != 0);
	
	i->_left = l;
	i->_right = r;

	i->_destroyFunc = Item_destroy;
	i->_toStringFunc = Item_toString;

	i->_objectType = OBJ_Base;
	i->_procLevel = PROC_L_0;

	return i;
}

// Literal

void ItemLiteral_destroy(Item* _this)
{
	ItemLiteral* i = (ItemLiteral*)_this;

	String_free(i->_str);
}

void ItemLiteral_toString(Item* _this, String* s)
{
	ItemLiteral* i = (ItemLiteral*)_this;

	String_cpy(s, i->_str->_str);
}

void ItemLiteral_writeRegel(Item* _this, String* ins, int level)
{
	ItemLiteral* i = (ItemLiteral*)_this;

	String_cpy(ins, L"L");
}

ItemLiteral* ItemLiteral_init(const wchar_t* s)
{
	ItemLiteral* i = (ItemLiteral*)malloc(sizeof(ItemLiteral));
	assert(i != 0);

	i->_item._left = NULL;
	i->_item._right = NULL;

	i->_item._destroyFunc = ItemLiteral_destroy;
	i->_item._toStringFunc = ItemLiteral_toString;
	i->_item._writeRegelFunc = ItemLiteral_writeRegel;

	i->_item._objectType = OBJ_Literal;
	i->_item._procLevel = PROC_L_11;

	i->_str = String_init();
	
	String_cpy(i->_str, s);
	
	return i;
}

// Number

void ItemNumber_destroy(Item* _this)
{
	ItemNumber* i = (ItemNumber*)_this;

	String_free(i->_str);
}

void ItemNumber_toString(Item* _this, String* s)
{
	ItemNumber* i = (ItemNumber*)_this;

	String_cpy(s, i->_str->_str);
}

void ItemNumber_writeRegel (Item* _this, String* ins, int level)
{
	ItemNumber* i = (ItemNumber*)_this;

	String_cpy(ins, L"N");
}

ItemNumber* ItemNumber_init(const wchar_t* s)
{
	ItemNumber* i = (ItemNumber*)malloc(sizeof(ItemNumber));
	assert(i != 0);

	i->_item._left = NULL;
	i->_item._right = NULL;

	i->_item._destroyFunc = ItemNumber_destroy;
	i->_item._toStringFunc = ItemNumber_toString;
	i->_item._writeRegelFunc = ItemNumber_writeRegel;

	i->_item._objectType = OBJ_Number;
	i->_item._procLevel = PROC_L_11;

	i->_str = String_init();
	String_cpy(i->_str, s);

	return i;
}

// Symbol

void ItemSymbol_toString(Item* _this, String* s)
{
	ItemSymbol* i = (ItemSymbol*)_this;

	wchar_t str[2];
	str[0] = i->_ch;
	str[1] = 0;

	String_cpy(s, str);
}

void ItemSymbol_writeRegel(Item* _this, String* ins, int level)
{
	ItemSymbol* i = (ItemSymbol*)_this;

	String_cpy(ins, L"S");
}

ItemSymbol* ItemSymbol_init(const wchar_t ch)
{
	ItemSymbol* i = (ItemSymbol*)malloc(sizeof(ItemSymbol));
	assert(i != 0);

	i->_item._left = NULL;
	i->_item._right = NULL;

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemSymbol_toString;
	i->_item._writeRegelFunc = ItemSymbol_writeRegel;

	i->_item._objectType = OBJ_Symbol;
	i->_item._procLevel = PROC_L_11;

	i->_ch = ch;

	return i;
}
// List

void ItemList_toString(Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);
	_this->_right->_toStringFunc(_this->_right, s2);

	String_cpy(s, s1->_str);
	String_cat(s, L",");
	String_cat(s, s2->_str);

	String_free(s1);
	String_free(s2);
}

void ItemList_writeRegel(Item* _this, String* ins, int level)
{
}

ItemList* ItemList_init(Item* l, Item* r)
{
	ItemList* i = (ItemList*)malloc(sizeof(ItemList));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemList_toString;
	i->_item._writeRegelFunc = ItemList_writeRegel;

	i->_item._objectType = OBJ_List;
	i->_item._procLevel = PROC_L_1;

	return i;
}

// Equ

void ItemEqu_toString(Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);
	_this->_right->_toStringFunc(_this->_right, s2);

	String_cpy(s, s1->_str);
	String_cat(s, L"=");
	String_cat(s, s2->_str);

	String_free(s1);
	String_free(s2);
}

void ItemEqu_writeRegel(Item* _this, String* ins, int level)
{
}

ItemEqu* ItemEqu_init(Item* l, Item* r, wchar_t sy)
{
	ItemEqu* i = (ItemEqu*)malloc(sizeof(ItemEqu));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemEqu_toString;
	i->_item._writeRegelFunc = ItemEqu_writeRegel;

	i->_item._objectType = OBJ_Equ;
	i->_item._procLevel = PROC_L_2;

	i->_sy = sy;

	return i;
}

ItemEqu* ItemEqu_init_eq(Item* l, Item* r)
{
	return ItemEqu_init(l, r, L'=');
}

ItemEqu* ItemEqu_init_nq(Item* l, Item* r)
{
	return ItemEqu_init(l, r, L'\u2260');
}

ItemEqu* ItemEqu_init_g(Item* l, Item* r)
{
	return ItemEqu_init(l, r, L'>');
}

ItemEqu* ItemEqu_init_l(Item* l, Item* r)
{
	return ItemEqu_init(l, r, L'<');
}

ItemEqu* ItemEqu_init_eg(Item* l, Item* r)
{
	return ItemEqu_init(l, r, L'\u2265');
}

ItemEqu* ItemEqu_init_el(Item* l, Item* r)
{
	return ItemEqu_init(l, r, L'\u2264');
}

// Add

void ItemAdd_toString(Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);
	_this->_right->_toStringFunc(_this->_right, s2);
	
	String_cat(s, s1->_str);
	String_cat(s, L"+");
	String_cat(s, s2->_str);

	String_free(s1);
	String_free(s2);
}

void ItemAdd_writeRegel(Item* _this, String* ins, int level)
{
	Item_writeRegel(_this, ins, level, L"+");
}

ItemAdd* ItemAdd_init(Item* l, Item* r)
{
	ItemAdd* i = (ItemAdd*)malloc(sizeof(ItemAdd));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemAdd_toString;
	i->_item._writeRegelFunc = ItemAdd_writeRegel;

	i->_item._objectType = OBJ_Add;
	i->_item._procLevel = PROC_L_3;

	return i;
}

// Sub

void ItemSub_toString(Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);
	_this->_right->_toStringFunc(_this->_right, s2);

	String_cat(s, s1->_str);
	String_cat(s, L"-");
	String_cat(s, s2->_str);

	String_free(s1);
	String_free(s2);
}

void ItemSub_writeRegel(Item* _this, String* ins, int level)
{
	Item_writeRegel(_this, ins, level, L"-");
}

ItemSub* ItemSub_init(Item* l, Item* r)
{
	ItemSub* i = (ItemSub*)malloc(sizeof(ItemSub));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemSub_toString;
	i->_item._writeRegelFunc = ItemSub_writeRegel;

	i->_item._objectType = OBJ_Sub;
	i->_item._procLevel = PROC_L_3;

	return i;
}

// Mult

void ItemMult_toString(Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);
	_this->_right->_toStringFunc(_this->_right, s2);

	String_cat(s, s1->_str);
	String_cat(s, L"*");
	String_cat(s, s2->_str);

	String_free(s1);
	String_free(s2);
}

void ItemMult_writeRegel(Item* _this, String* ins, int level)
{
	Item_writeRegel(_this, ins, level, L"*");
}

ItemMult* ItemMult_init(Item* l, Item* r)
{
	ItemMult* i = (ItemMult*)malloc(sizeof(ItemMult));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemMult_toString;
	i->_item._writeRegelFunc = ItemMult_writeRegel;

	i->_item._objectType = OBJ_Mult;
	i->_item._procLevel = PROC_L_4;

	return i;
}

// Frac

void ItemFrac_toString(Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);
	_this->_right->_toStringFunc(_this->_right, s2);

	if(!Item_isLeaf(_this->_left))
		String_cat(s, L"(");
	String_cat(s, s1->_str);
	if (!Item_isLeaf(_this->_left))
		String_cat(s, L")");

	String_cat(s, L"/");

	if (!Item_isLeaf(_this->_right))
		String_cat(s, L"(");
	String_cat(s, s2->_str);
	if (!Item_isLeaf(_this->_right))
		String_cat(s, L")");

	String_free(s1);
	String_free(s2);
}

void ItemFrac_writeRegel(Item* _this, String* ins, int level)
{
	Item_writeRegel(_this, ins, level, L"/");
}

ItemFrac* ItemFrac_init(Item* l, Item* r)
{
	ItemFrac* i = (ItemFrac*)malloc(sizeof(ItemFrac));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemFrac_toString;
	i->_item._writeRegelFunc = ItemFrac_writeRegel;

	i->_item._objectType = OBJ_Frac;
	i->_item._procLevel = PROC_L_4;

	i->_fHeight = 8;

	return i;
}

// Sign

void ItemSign_toString(Item* _this, String* s)
{
	String* s1 = String_init();

	ItemSign* i = (ItemSign*)_this; 

	wchar_t str[2];
	str[0] = i->_sgn;
	str[1] = 0;

	String_cat(s, str);
	
	_this->_left->_toStringFunc(_this->_left, s1);

	String_cat(s, s1->_str);

	String_free(s1);
}

void ItemSign_writeRegel(Item* _this, String* ins, int level)
{
	Item_writeRegel(_this, ins, level, L"Sign");
}

ItemSign* ItemSign_init(Item* l, const wchar_t sgn)
{
	ItemSign* i = (ItemSign*)malloc(sizeof(ItemSign));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = NULL;

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemSign_toString;
	i->_item._writeRegelFunc = ItemSign_writeRegel;

	i->_item._objectType = OBJ_Sign;
	i->_item._procLevel = PROC_L_5;

	i->_sgn = sgn;

	return i;
}

ItemSign* ItemSignMinus_init(Item* l)
{
	return ItemSign_init(l, L'-');
}

ItemSign* ItemSignPlus_init(Item* l)
{
	return ItemSign_init(l, L'+');
}

// Sqrt

void ItemSqrt_toString(Item* _this, String* s)
{
	String* s1 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);

	String_cpy(s, L"Sqrt(");
	String_cat(s, s1->_str);
	String_cat(s, L")");

	String_free(s1);
}

void ItemSqrt_writeRegel(Item* _this, String* ins, int level)
{
}

ItemSqrt* ItemSqrt_init(Item* l)
{
	ItemSqrt* i = (ItemSqrt*)malloc(sizeof(ItemSqrt));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = NULL;

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemSqrt_toString;
	i->_item._writeRegelFunc = ItemSqrt_writeRegel;

	i->_item._objectType = OBJ_Sqrt;
	i->_item._procLevel = PROC_L_9;

	i->_sHeight = 0;
	i->_sPadding = 20;

	return i;
}

// Pow

void ItemPow_toString(Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);
	_this->_right->_toStringFunc(_this->_right, s2);

	String_cpy(s, s1->_str);
	String_cat(s, L"^");

	if (!Item_isLeaf(_this->_right))
		String_cat(s, L"(");
	String_cat(s, s2->_str);
	if (!Item_isLeaf(_this->_right))
		String_cat(s, L")");

	String_free(s1);
	String_free(s2);
}

void ItemPow_writeRegel(Item* _this, String* ins, int level)
{
	Item_writeRegel(_this, ins, level, L"^");
}

ItemPow* ItemPow_init(Item* l, Item* r)
{
	ItemPow* i = (ItemPow*)malloc(sizeof(ItemPow));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemPow_toString;
	i->_item._writeRegelFunc = ItemPow_writeRegel;

	i->_item._objectType = OBJ_Pow;
	i->_item._procLevel = PROC_R_7;
	
	return i;
}

// Subscript

void ItemSubscript_toString(Item* _this, String* s)
{
	String* s1 = String_init();
	String* s2 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);
	_this->_right->_toStringFunc(_this->_right, s2);

	String_cpy(s, s1->_str);
	String_cat(s, L"_");

	if (!Item_isLeaf(_this->_right))
		String_cat(s, L"(");
	String_cat(s, s2->_str);
	if (!Item_isLeaf(_this->_right))
		String_cat(s, L")");

	String_free(s1);
	String_free(s2);
}

void ItemSubscript_writeRegel(Item* _this, String* ins, int level)
{
	Item_writeRegel(_this, ins, level, L"_");
}

ItemSubscript* ItemSubscript_init(Item* l, Item* r)
{
	ItemSubscript* i = (ItemSubscript*)malloc(sizeof(ItemSubscript));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemSubscript_toString;
	i->_item._writeRegelFunc = ItemSubscript_writeRegel;

	i->_item._objectType = OBJ_Subscript;
	i->_item._procLevel = PROC_R_8;

	return i;
}

// CommFunc

void ItemCommFunc_toString(Item* _this, String* s)
{
	String* s1 = String_init();
	ItemCommFunc* i = (ItemCommFunc*)_this;

	_this->_left->_toStringFunc(_this->_left, s1);

	String_cpy(s, i->_str);
	String_cat(s, L"(");
	String_cat(s, s1->_str);
	String_cat(s, L")");

	String_free(s1);
}

void ItemCommFunc_destroy(Item* _this)
{
	ItemCommFunc* i = (ItemCommFunc*)_this;
	free(i->_str);
}

void ItemCommFunc_writeRegel(Item* _this, String* ins, int level)
{
	Item_writeRegel(_this, ins, level, L"fn");
}

ItemCommFunc* ItemCommFunc_init(Item* l, Item* r, const wchar_t* s)
{
	ItemCommFunc* i = (ItemCommFunc*)malloc(sizeof(ItemCommFunc));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = r;

	i->_item._destroyFunc = ItemCommFunc_destroy;
	i->_item._toStringFunc = ItemCommFunc_toString;
	i->_item._writeRegelFunc = ItemCommFunc_writeRegel;

	i->_item._objectType = OBJ_CommFunc;
	i->_item._procLevel = PROC_L_9;

	i->_len = wcslen(s);
	i->_str = (wchar_t*)malloc(sizeof(wchar_t) * (i->_len + 1));
	assert(i->_str != 0);

	memset(i->_str, 0, i->_len + 1);
	wcscpy_s(i->_str, i->_len + 1, s);

	return i;
}

ItemCommFunc* ItemSinFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Sin");
}

ItemCommFunc* ItemCosFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Cos");
}

ItemCommFunc* ItemTanFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Tan");
}

ItemCommFunc* ItemLogFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Log");
}

ItemCommFunc* ItemExpFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Exp");
}

ItemCommFunc* ItemAsinFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Asin");
}

ItemCommFunc* ItemAcosFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Acos");
}

ItemCommFunc* ItemAtanFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Atan");
}

ItemCommFunc* ItemLnFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Ln");
}

ItemCommFunc* ItemRootFunc_init(Item* l)
{
	return ItemCommFunc_init(l, NULL, L"Root");
}

// Factorial

void ItemFactorial_toString(Item* _this, String* s)
{
	String* s1 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);

	String_cpy(s, s1->_str);
	String_cat(s, L"!");

	String_free(s1);
}

void ItemFactorial_writeRegel(Item* _this, String* ins, int level)
{
	Item_writeRegel(_this, ins, level, L"!");
}

ItemFactorial* ItemFactorial_init(Item* l)
{
	ItemFactorial* i = (ItemFactorial*)malloc(sizeof(ItemFactorial));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = NULL;

	i->_item._destroyFunc = Item_destroy; 
	i->_item._toStringFunc = ItemFactorial_toString;
	i->_item._writeRegelFunc = ItemFactorial_writeRegel;

	i->_item._objectType = OBJ_Factorial;
	i->_item._procLevel = PROC_L_6;

	return i;
}

// Sigma

void ItemSigma_destroy(Item* _this)
{
	ItemSigma* i = (ItemSigma*)_this;

	if (i->_bottom)
	{
		ItemTree_free(&(i->_bottom));
	}

	if (i->_top)
	{
		ItemTree_free(&(i->_top));
	}
}

void ItemSigma_toString(Item* _this, String* s)
{
	ItemSigma* i = (ItemSigma*)_this;

	String* s1 = String_init();
	String* s2 = String_init();
	String* s3 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);

	String_cpy(s, L"Sigma(");
	String_cat(s, s1->_str);

	if (i->_bottom)
	{
		i->_bottom->_toStringFunc(i->_bottom, s2);
		String_cat(s, L";");
		String_cat(s, s2->_str);
	}

	if (i->_top)
	{
		i->_top->_toStringFunc(i->_top, s3);
		String_cat(s, L";");
		String_cat(s, s3->_str);
	}

	String_cat(s, L")");

	String_free(s1);
	String_free(s2);
	String_free(s3);
}

void ItemSigma_writeRegel(Item* _this, String* ins, int level)
{
}

ItemSigma* ItemSigma_init(Item* l, Item* b, Item* t)
{
	ItemSigma* i = (ItemSigma*)malloc(sizeof(ItemSigma));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = NULL;

	i->_bottom = b;
	i->_top = t;

	i->_item._destroyFunc = ItemSigma_destroy;
	i->_item._toStringFunc = ItemSigma_toString;
	i->_item._writeRegelFunc = ItemSigma_writeRegel;

	i->_item._objectType = OBJ_Sigma;
	i->_item._procLevel = PROC_L_9;

	return i;
}

// Integrate

void ItemIntegrate_destroy(Item* _this)
{
	ItemIntegrate* i = (ItemIntegrate*)_this;

	if (i->_bottom)
	{
		ItemTree_free(&(i->_bottom));
	}

	if (i->_top)
	{
		ItemTree_free(&(i->_top));
	}
}

void ItemIntegrate_toString(Item* _this, String* s)
{
	ItemSigma* i = (ItemSigma*)_this;

	String* s1 = String_init();
	String* s2 = String_init();
	String* s3 = String_init();
	String* s4 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);

	String_cpy(s, L"Integrate(");
	String_cat(s, s1->_str);

	if (i->_item._right)
	{
		i->_item._right->_toStringFunc(i->_item._right, s4);
		String_cat(s, L";");
		String_cat(s, s4->_str);
	}

	if (i->_bottom)
	{
		i->_bottom->_toStringFunc(i->_bottom, s2);
		String_cat(s, L";");
		String_cat(s, s2->_str);
	}

	if (i->_top)
	{
		i->_top->_toStringFunc(i->_top, s3);
		String_cat(s, L";");
		String_cat(s, s3->_str);
	}

	String_cat(s, L")");

	String_free(s1);
	String_free(s2);
	String_free(s3);
	String_free(s4);
}

void ItemIntegrate_writeRegel(Item* _this, String* ins, int level)
{
}

ItemIntegrate* ItemIntegrate_init(Item* l, Item* r, Item* b, Item* t)
{
	ItemIntegrate* i = (ItemIntegrate*)malloc(sizeof(ItemIntegrate));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_bottom = b;
	i->_top = t;

	i->_item._destroyFunc = ItemIntegrate_destroy;
	i->_item._toStringFunc = ItemIntegrate_toString;
	i->_item._writeRegelFunc = ItemIntegrate_writeRegel;


	i->_item._objectType = OBJ_Integrate;
	i->_item._procLevel = PROC_L_9;

	return i;
}

// Derivative

void ItemDerivative_toString(Item* _this, String* s)
{
	String* s1 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);

	String_cpy(s, L"Derivative(");
	String_cat(s, s1->_str);
	String_cat(s, L")");

	String_free(s1);
}

void ItemDerivative_writeRegel(Item* _this, String* ins, int level)
{
}

ItemDerivative* ItemDerivative_init(Item* l, Item* r)
{
	ItemDerivative* i = (ItemDerivative*)malloc(sizeof(ItemDerivative));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = (r != NULL) ? r : Item_init(NULL, NULL);

	i->_item._destroyFunc = Item_destroy;
	i->_item._toStringFunc = ItemDerivative_toString;
	i->_item._writeRegelFunc = ItemDerivative_writeRegel;

	i->_item._objectType = OBJ_Derivative;
	i->_item._procLevel = PROC_L_9;

	return i;
}

// Limit

void ItemLimit_destroy(Item* _this)
{
	ItemLimit* i = (ItemLimit*)_this;

	if (i->_t1)
	{
		ItemTree_free(&(i->_t1));
	}

	if (i->_t2)
	{
		ItemTree_free(&(i->_t2));
	}
}

void ItemLimit_toString(Item* _this, String* s)
{
	ItemLimit* i = (ItemLimit*)_this;

	String* s1 = String_init();
	String* s2 = String_init();
	String* s3 = String_init();

	_this->_left->_toStringFunc(_this->_left, s1);

	String_cpy(s, L"Limit(");
	String_cat(s, s1->_str);

	if (i->_t1)
	{
		i->_t1->_toStringFunc(i->_t1, s2);
		String_cat(s, L";");
		String_cat(s, s2->_str);
	}

	if (i->_t2)
	{
		i->_t2->_toStringFunc(i->_t2, s3);
		String_cat(s, L";");
		String_cat(s, s3->_str);
	}

	String_cat(s, L")");

	String_free(s1);
	String_free(s2);
	String_free(s3);
}

void ItemLimit_writeRegel(Item* _this, String* ins, int level)
{
}

ItemLimit* ItemLimit_init(Item* l, Item* t1, Item* t2)
{
	ItemLimit* i = (ItemLimit*)malloc(sizeof(ItemLimit));
	assert(i != 0);

	i->_item._left = (l != NULL) ? l : Item_init(NULL, NULL);
	i->_item._right = NULL;

	i->_t1 = t1;
	i->_t2 = t2;

	i->_item._destroyFunc = ItemLimit_destroy;
	i->_item._toStringFunc = ItemLimit_toString;
	i->_item._writeRegelFunc = ItemLimit_writeRegel;

	i->_item._objectType = OBJ_Sigma;
	i->_item._procLevel = PROC_L_9;

	return i;
}

void ItemTreeChild_free(Item* item)
{
	Item* left = item->_left;
	Item* right = item->_right;

	if (left)
	{
		ItemTreeChild_free(left);
		left->_destroyFunc(left);
		free(left);
	}

	if (right)
	{
		ItemTreeChild_free(right);
		right->_destroyFunc(right); 
		free(right);
	}
}

void ItemTree_free(Item** Item)
{
	ItemTreeChild_free(*Item);
	(*Item)->_destroyFunc(*Item);
	free(*Item);
}

Item* Item_getParent(Item* items, Item* i)
{
	Item* left = items->_left;
	Item* right = items->_right;

	if (items == i)
		return NULL;
	
	if (left)
	{
		if (left == i)
			return items;

		Item* r = Item_getParent(left, i);
		if (r)
			return r;
	}

	if (right)
	{
		if (right == i)
			return items;

		Item* r = Item_getParent(right, i);
		if (r)
			return r;
	}

	return NULL;
}

void Item_getLevelCount(Item* item, int* pi)
{
	Item* left = item->_left;
	Item* right = item->_right;

	if (left)
	{
		Item_getLevelCount(left, pi);
	}

	if (right)
	{
		Item_getLevelCount(right, pi);
	}

	if (left || right)
		++(*pi);
}
