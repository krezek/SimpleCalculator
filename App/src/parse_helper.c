#include "platform.h"

#include <gitems.h>
#include <math_parser.h>

static void* list(void* v, void* v1)
{

}

static void* equ(void* v, void* v1, const wchar_t c)
{

}

static void* add(void* v, void* v1)
{

}

static void* sub (void* v, void* v1)
{

}

static void* frac (void* v, void* v1)
{

}

static void* mult (void* v, void* v1)
{

}

static void* sign (void* v, const wchar_t c)
{

}

static void* factorial (void* v)
{

}

static void* power (void* v, void* v1)
{

}

static void* subscript (void* v, void* v1)
{

}

static void* parentheses (void* v)
{

}

static void* common (void* v, void* v1, const wchar_t* s)
{

}

static void* sqrt (void* v)
{

}

static void* limit (void* v, void* v1, void* v2)
{

}

static void* derivative (void* v, void* v1)
{

}

static void* integrate (void* v, void* v1, void* v2, void* v3)
{

}

static void* sigma (void* v, void* v1, void* v2)
{

}

static void* number (const wchar_t* s)
{

}

static void* symbol (const wchar_t c)
{

}

static void* literal (const wchar_t* s)
{

}


void parse_test()
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
	mp->_sqrtFunc = sqrt;
	mp->_limitFunc = limit;
	mp->_derivativeFunc = derivative;
	mp->_integrateFunc = integrate;
	mp->_sigmaFunc = sigma;
	mp->_numberFunc = number;
	mp->_symbolFunc = symbol;
	mp->_literalFunc = literal;

	void* items;
	MParser_do(mp, &items, L"10+20");

	MParser_free(mp);
}
