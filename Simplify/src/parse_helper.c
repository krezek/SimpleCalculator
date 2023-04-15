#include "platform.h"

#include <items.h>
#include <math_parser.h>

static void* list(void* v, void* v1)
{
	return v;
}

static void* equ(void* v, void* v1, const wchar_t c)
{
	return v;
}

static void* add(void* v, void* v1)
{
	return ItemAdd_init(v, v1);
}

static void* sub (void* v, void* v1)
{
	return ItemSub_init(v, v1);
}

static void* frac (void* v, void* v1)
{
	return ItemFrac_init(v, v1);
}

static void* mult (void* v, void* v1)
{
	return ItemMult_init(v, v1);
}

static void* sign (void* v, const wchar_t c)
{
	return ItemSign_init(v, c);
}

static void* factorial (void* v)
{
	return ItemFactorial_init(v);
}

static void* power (void* v, void* v1)
{
	return ItemPow_init(v, v1);
}

static void* subscript (void* v, void* v1)
{
	return ItemSubscript_init(v, v1);
}

static void* parentheses (void* v)
{
	return v;
}

static void* common (void* v, void* v1, const wchar_t* s)
{
	return ItemCommFunc_init(v, v1, s);
}

static void* _sqrt (void* v)
{
	return ItemSqrt_init(v);
}

static void* limit (void* v, void* v1, void* v2)
{
	return v;
}

static void* derivative (void* v, void* v1)
{
	return v;
}

static void* integrate (void* v, void* v1, void* v2, void* v3)
{
	return v;
}

static void* sigma (void* v, void* v1, void* v2)
{
	return v;
}

static void* number (const wchar_t* s)
{
	return ItemNumber_init(s, 0);
}

static void* symbol (const wchar_t c)
{
	return ItemSymbol_init(c);
}

static void* literal (const wchar_t* s)
{
	return ItemLiteral_init(s);
}


void parse_items(Item** pItems, const wchar_t* s)
{
	MParser* mp = MParser_init();

	mp->_listFunc = list;
	mp->_equFunc = equ;
	mp->_addFunc = add;
	mp->_subFunc = sub;
	mp->_fracFunc = frac;
	mp->_multFunc = mult;
	mp->_signFunc = sign;
	mp->_factorialFunc = factorial;
	mp->_powerFunc = power;
	mp->_subscriptFunc = subscript;
	mp->_parenthesesFunc = parentheses;
	mp->_commonFunc = common;
	mp->_sqrtFunc = _sqrt;
	mp->_limitFunc = limit;
	mp->_derivativeFunc = derivative;
	mp->_integrateFunc = integrate;
	mp->_sigmaFunc = sigma;
	mp->_numberFunc = number;
	mp->_symbolFunc = symbol;
	mp->_literalFunc = literal;

	Item* i = NULL;
	int rs = MParser_do(mp, &i, s);
	if (rs)
	{
		if(i)
			ItemTree_free(&i);
	}
	else
		*pItems = i;

	MParser_free(mp);
}
