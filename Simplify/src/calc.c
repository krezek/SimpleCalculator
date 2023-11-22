#include "platform.h"

#include <items.h>
#include <gmp.h>
#include <calc.h>

extern void parse_items(Item** pItems, const wchar_t* s);
mpf_t* calculate(Item** nodes, int* pCode);

wchar_t* do_calc(const wchar_t* expr)
{
	Item* item = NULL;
	parse_items(&item, expr);

	if (item)
	{
		int rc;
		mpf_t* result = NULL;
		char str[255];

		result = calculate(&item, &rc);
		if (rc == 0)
		{
			gmp_sprintf(str, "%.Ff", *result);
		}
		else
		{
			sprintf_s(str, 255, "Invalid value");
		}

		ItemTree_free(&item);

		if (result)
		{
			mpf_clear(*result);
			free(result);
		}
			
		wchar_t* rt = (wchar_t*)malloc((strlen(str) + 1) * sizeof(wchar_t));
		assert(rt != NULL);
		wsprintf(rt, L"%S", str);

		return rt;
	}


	return NULL;
}

mpf_t* calcWolker(Item* item, int* pCode)
{
	if (item->_objectType == OBJ_Number)
	{
		ItemNumber* i = (ItemNumber*)item;

		char* str = (char*)malloc(wcslen(i->_str->_str) + 1);
		assert(str != NULL);

		sprintf(str, "%S", i->_str->_str);

		mpf_t* r = (mpf_t*)malloc(sizeof(mpf_t));
		assert(r != NULL);

		mpf_init_set_str(*r, str, 10);

		free(str);

		return r;
	}
	else if (item->_objectType == OBJ_Add)
	{
		mpf_t* a1 = calcWolker(item->_left, pCode);
		mpf_t* a2 = calcWolker(item->_right, pCode);

		mpf_t* r = (mpf_t*)malloc(sizeof(mpf_t));
		assert(r != NULL);

		mpf_init(*r);
		mpf_add(*r, *a1, *a2);

		mpf_clear(*a1);
		mpf_clear(*a2);
		free(a1);
		free(a2);

		return r;
	}
	else if (item->_objectType == OBJ_Sub)
	{
		mpf_t* a1 = calcWolker(item->_left, pCode);
		mpf_t* a2 = calcWolker(item->_right, pCode);

		mpf_t* r = (mpf_t*)malloc(sizeof(mpf_t));
		assert(r != NULL);

		mpf_init(*r);
		mpf_sub(*r, *a1, *a2);

		mpf_clear(*a1);
		mpf_clear(*a2);
		free(a1);
		free(a2);

		return r;
	}
	else if (item->_objectType == OBJ_Mult)
	{
		mpf_t* a1 = calcWolker(item->_left, pCode);
		mpf_t* a2 = calcWolker(item->_right, pCode);

		mpf_t* r = (mpf_t*)malloc(sizeof(mpf_t));
		assert(r != NULL);

		mpf_init(*r);
		mpf_mul(*r, *a1, *a2);

		mpf_clear(*a1);
		mpf_clear(*a2);
		free(a1);
		free(a2);

		return r;
	}
	else if (item->_objectType == OBJ_Frac)
	{
		mpf_t* a1 = calcWolker(item->_left, pCode);
		mpf_t* a2 = calcWolker(item->_right, pCode);

		mpf_t* r = (mpf_t*)malloc(sizeof(mpf_t));
		assert(r != NULL);

		mpf_init(*r);
		mpf_div(*r, *a1, *a2);

		mpf_clear(*a1);
		mpf_clear(*a2);
		free(a1);
		free(a2);

		return r;
	}
	else if (item->_objectType == OBJ_Sign)
	{
		ItemSign* i = (ItemSign*)item;
		if (i->_sgn == L'-')
		{
			mpf_t* a = calcWolker(item->_left, pCode);

			mpf_t* r = (mpf_t*)malloc(sizeof(mpf_t));
			assert(r != NULL);

			mpf_init(*r);
			mpf_neg(*r, *a);

			mpf_clear(*a);
			free(a);

			return r;
		}
		else
			return calcWolker(item->_left, pCode);
	}
	/*else if (item->_objectType == OBJ_Pow)
	{
		return powl(calcWolker(item->_left), calcWolker(item->_right));
	}
	else if (item->_objectType == OBJ_Factorial)
	{
		long int n = (long int)calcWolker(item->_left);
		long int v = 1;

		if (n == 0)
			return 1;
		else
		{
			for (long int ix = n; ix > 1; --ix)
				v = v * ix;
		}

		return (double)v;
	}
	else if (item->_objectType == OBJ_Sqrt)
	{
		return sqrtl(calcWolker(item->_left));
	}
	else if (item->_objectType == OBJ_CommFunc)
	{
		ItemCommFunc* i = (ItemCommFunc*)item;
		if (_wcsicmp(L"sin", i->_str) == 0)
		{
			return sin(calcWolker(item->_left));
		}
		else if (_wcsicmp(L"cos", i->_str) == 0)
		{
			return cos(calcWolker(item->_left));
		}
		else if (_wcsicmp(L"tan", i->_str) == 0)
		{
			return tan(calcWolker(item->_left));
		}
		else if (_wcsicmp(L"log", i->_str) == 0)
		{
			return log10(calcWolker(item->_left));
		}
		else if (_wcsicmp(L"exp", i->_str) == 0)
		{
			return exp(calcWolker(item->_left));
		}
		else if (_wcsicmp(L"asin", i->_str) == 0)
		{
			return asin(calcWolker(item->_left));
		}
		else if (_wcsicmp(L"acos", i->_str) == 0)
		{
			return acos(calcWolker(item->_left));
		}
		else if (_wcsicmp(L"atan", i->_str) == 0)
		{
			return atan(calcWolker(item->_left));
		}
		else if (_wcsicmp(L"ln", i->_str) == 0)
		{
			return log(calcWolker(item->_left));
		}
		else if (_wcsicmp(L"root", i->_str) == 0)
		{
			return powl(calcWolker(item->_left), 1 / calcWolker(item->_right));
		}
	}
	else if (item->_objectType == OBJ_Parentheses)
	{
		return calcWolker(item->_left);
	}
	else if (item->_objectType == OBJ_Literal)
	{
		ItemLiteral* i = (ItemLiteral*)item;
		
		if ((_wcsicmp(L"pi", i->_str->_str) == 0)
			|| (_wcsicmp(L"\u03C0", i->_str->_str) == 0))
		{
			return M_PI;
		}
		else if (_wcsicmp(L"e", i->_str->_str) == 0)
		{
			return M_E;
		}
		else
		{
			*pCode = -1;
		}
	}*/
	else
	{
		*pCode = -1;
	}

	return NULL;
}

mpf_t* calculate(Item** nodes, int* pCode)
{
	*pCode = 0;
	return calcWolker(*nodes, pCode);
}
