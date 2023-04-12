#include "platform.h"

#include <gitems.h>
#include <math_parser.h>

void GList_append(GList* src, GList* dist)
{
	if (src)
	{
		if (src->_front)
		{
			GNode* gn = src->_front;
			while (gn)
			{
				GList_pushback(dist, gn->_pGItem);
				gn->_pGItem = NULL;

				gn = gn->_next;
			}
		}
	}
}

void GList_append_str(GList* dist, const wchar_t* s)
{
	for (size_t ix = 0, len = wcslen(s); ix < len; ++ix)
		GList_pushback(dist, (GItem*)GItemChar_init(s[ix]));
}

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
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(gl, L"+");

	GList_append(gl1, gl);
	GList_free(gl1);

	return gl;
}

static void* sub (void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(gl, L"-");

	GList_append(gl1, gl);
	GList_free(gl1);

	return gl;
}

static void* frac (void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList* r = GList_init(NULL);
	GList_pushback(r, (GItem*)GItemFraction_init(gl, gl1));

	return r;
}

static void* mult (void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_append_str(gl, L"*");

	GList_append(gl1, gl);
	GList_free(gl1);

	return gl;
}

static void* sign (void* v, const wchar_t c)
{
	GList* gl = (GList*)v;

	GList* r = GList_init(NULL);
	GList_pushback(r, (GItem*)GItemChar_init(c));
	GList_append(gl, r);

	GList_free(gl);

	return r;
}

static void* factorial (void* v)
{
	GList* gl = (GList*)v;

	GList_append_str(gl, L"!");

	return gl;
}

static void* power (void* v, void* v1)
{
	GList* gl = (GList*)v;
	GList* gl1 = (GList*)v1;

	GList_pushback(gl, (GItem*)GItemPower_init(gl1));

	return gl;
}

static void* subscript (void* v, void* v1)
{
	return v;
}

static void* parentheses (void* v)
{
	GList* gl = (GList*)v;

	GList* r = GList_init(NULL);
	GList_append_str(r, L"(");
	GList_append(gl, r);
	GList_append_str(r, L")");

	GList_free(gl);

	return r;
}

static void* common (void* v, void* v1, const wchar_t* s)
{
	GList* gl = (GList*)v;

	GList* r = GList_init(NULL);
	GList_append_str(r, s);
	GList_append_str(r, L"(");
	GList_append(gl, r);
	GList_append_str(r, L")");

	GList_free(gl);

	return r;
}

static void* sqrt (void* v)
{
	GList* gl = (GList*)v;

	GList* r = GList_init(NULL);
	GList_append_str(r, L"Sqrt");
	GList_append_str(r, L"(");
	GList_append(gl, r);
	GList_append_str(r, L")");

	GList_free(gl);

	return r;
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
	GList* gl = GList_init(NULL);
	GList_append_str(gl, s);
	
	return gl;
}

static void* symbol (const wchar_t c)
{
	GList* gl = GList_init(NULL);
	GList_pushback(gl, (GItem*)GItemChar_init(c));

	return gl;
}

static void* literal (const wchar_t* s)
{
	GList* gl = GList_init(NULL);
	GList_append_str(gl, s);

	return gl;
}


void parse_test(GList** ppGl, const wchar_t* s)
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

	GList* i = NULL;
	int rs = MParser_do(mp, &i, s);
	if (rs)
	{
		GList_free(i);
	}
	else
		*ppGl = i;

	MParser_free(mp);
}
