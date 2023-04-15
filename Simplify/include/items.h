#ifndef _ITEMS_H_
#define _ITEMS_H_

#include <strg.h>

typedef enum {OBJ_Base = 0, OBJ_Literal, OBJ_Number, OBJ_Symbol, OBJ_List, OBJ_Equ, OBJ_Add, OBJ_Sub, OBJ_Mult, OBJ_Frac,
				OBJ_Sign, OBJ_Sqrt, OBJ_CommFunc, OBJ_Pow, OBJ_Subscript, OBJ_Factorial, OBJ_Parentheses, OBJ_Sigma,
				OBJ_Integrate, OBJ_Derivative,
				// Extended Objects
				OBJ_Integer} ObjectType;
typedef enum { PROC_L_0 = 0, PROC_L_1, PROC_L_2, PROC_L_3, PROC_L_4, PROC_L_5, PROC_L_6
				, PROC_R_7, PROC_R_8, PROC_L_9, PROC_L_10, PROC_L_11 } ProcLevel;

typedef struct _Item Item;

typedef void (*destroyFunc) (Item* _this);
typedef void (*toStringFunc) (Item* _this, String* s);
typedef void (*writeRegelFunc) (Item* _this, String* ins, int level);

typedef struct _Item
{
	struct _Item* _left;
	struct _Item* _right;

	destroyFunc _destroyFunc;
	toStringFunc _toStringFunc;
	writeRegelFunc _writeRegelFunc;

	ObjectType _objectType;
	ProcLevel _procLevel;

} Item;

typedef struct
{
	Item _item;

	String* _str;
} ItemLiteral;

typedef struct
{
	Item _item;

	String* _str;
} ItemNumber;

typedef struct
{
	Item _item;

	wchar_t _ch;
} ItemSymbol;

typedef struct
{
	Item _item;
} ItemList;

typedef struct
{
	Item _item;
	wchar_t _sy;
} ItemEqu;

typedef struct
{
	Item _item;
} ItemAdd;

typedef struct
{
	Item _item;
} ItemSub;

typedef struct
{
	Item _item;
} ItemMult;

typedef struct
{
	Item _item;

	int _fHeight;
} ItemFrac;

typedef struct
{
	Item _item;

	wchar_t _sgn;
} ItemSign;

typedef struct
{
	Item _item;

	int _sHeight;
	int _sPadding;
} ItemSqrt;

typedef struct
{
	Item _item;

	wchar_t* _str;
	size_t _len;
} ItemCommFunc;

typedef struct
{
	Item _item;
} ItemPow;

typedef struct
{
	Item _item;
} ItemSubscript;

typedef struct
{
	Item _item;
} ItemFactorial;

typedef struct _ItemSigma
{
	Item _item;

	struct _Item* _bottom;
	struct _Item* _top;
} ItemSigma;

typedef struct _ItemIntegrate
{
	Item _item;

	struct _Item* _bottom;
	struct _Item* _top;
} ItemIntegrate;

typedef struct _ItemDerivative
{
	Item _item;
} ItemDerivative;

typedef struct _ItemLimit
{
	Item _item;

	struct _Item* _t1;
	struct _Item* _t2;
} ItemLimit;

ItemLiteral* ItemLiteral_init(const wchar_t* s);
ItemNumber* ItemNumber_init(const wchar_t* s);
ItemSymbol* ItemSymbol_init(const wchar_t ch);

ItemAdd* ItemAdd_init(Item* l, Item* r);
ItemSub* ItemSub_init(Item* l, Item* r);

ItemMult* ItemMult_init(Item* l, Item* r);
ItemFrac* ItemFrac_init(Item* l, Item* r);

ItemSign* ItemSign_init(Item* l, const wchar_t sgn);

ItemSqrt* ItemSqrt_init(Item* l);
ItemPow* ItemPow_init(Item* l, Item* r);
ItemSubscript* ItemSubscript_init(Item* l, Item* r);
ItemCommFunc* ItemCommFunc_init(Item* l, Item* r, const wchar_t* s);

ItemFactorial* ItemFactorial_init(Item* l);

ItemSigma* ItemSigma_init(Item* l, Item* b, Item* t);
ItemIntegrate* ItemIntegrate_init(Item* l, Item* r, Item* b, Item* t);
ItemDerivative* ItemDerivative_init(Item* l, Item* r);
ItemLimit* ItemLimit_init(Item* l, Item* t1, Item* t2);

int Item_isLeaf(Item* i);
void ItemTree_free(Item** Item);
Item* Item_getParent(Item* items, Item* i);
void Item_getLevelCount(Item* item, int* pi);

#endif /* _ITEMS_H_ */

