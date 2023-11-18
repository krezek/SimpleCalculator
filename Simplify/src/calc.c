#include "platform.h"

#include <calc.h>
#include <rational.h>

#define _USE_MATH_DEFINES
#include <math.h>

static int calc_code;

extern void parse_items(Item** pItems, const wchar_t* s);
int calculate(Item** nodes, double* result);
int calculate_1024(Item** nodes, Rational* pResult);

wchar_t* do_calc_1024(const wchar_t* expr)
{
	Item* item = NULL;
	parse_items(&item, expr);

	if (item)
	{
		int rc;
		Rational result;

		rc = calculate_1024(&item, &result);
		if (rc == 0)
		{

		}

		ItemTree_free(&item);
	}

	return NULL;
}

Rational calcWolker_1024(Item* item)
{
	Rational r = { 0 };

	if (item->_objectType == OBJ_Number)
	{
		ItemNumber* i = (ItemNumber*)item;
		Rational r = Rational_init(i->_str->_str);
		return r;
	}
	else
	{
		calc_code = -1;
	}

	return r;
}

int calculate_1024(Item** nodes, Rational* pResult)
{
	calc_code = 0;

	*pResult = calcWolker_1024(*nodes);

	return calc_code;
}

wchar_t* do_calc(const wchar_t* expr)
{
	Item* item = NULL;
	parse_items(&item, expr);

	if (item)
	{
		int rc;
		double result;
		wchar_t wst[255];

		rc = calculate(&item, &result);
		if (rc == 0)
		{
			long long lrs = (long long)result;

			if (result == (double)lrs)
			{
				swprintf_s(wst, 255, L"%lld", lrs);
			}
			else
			{
				swprintf_s(wst, 255, L"%e", result);
			}
		}
		else
		{
			swprintf_s(wst, 255, L"Invalid value");
		}

		ItemTree_free(&item);

		wchar_t* rt = (wchar_t*)malloc((wcslen(wst) + 1) * sizeof(wchar_t));
		assert(rt != NULL);
		wcscpy(rt, wst);

		return rt;
	}


	return NULL;
}

double calcWolker(Item* item)
{
	if (item->_objectType == OBJ_Add)
	{
		return calcWolker(item->_left) + calcWolker(item->_right);
	}
	else if (item->_objectType == OBJ_Sub)
	{
		return calcWolker(item->_left) - calcWolker(item->_right);
	}
	else if (item->_objectType == OBJ_Sign)
	{
		ItemSign* i = (ItemSign*)item;
		if (i->_sgn == L'-')
			return -1 * calcWolker(item->_left);
		else
			return calcWolker(item->_left);
	}
	else if (item->_objectType == OBJ_Mult)
	{
		return calcWolker(item->_left) * calcWolker(item->_right);
	}
	else if (item->_objectType == OBJ_Frac)
	{
		return calcWolker(item->_left) / calcWolker(item->_right);
	}
	else if (item->_objectType == OBJ_Pow)
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
	else if (item->_objectType == OBJ_Number)
	{
		ItemNumber* i = (ItemNumber*)item;
		double r = wcstod(i->_str->_str, NULL);
		return r;
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
			calc_code = -1;
		}
	}
	else
	{
		calc_code = -1;
	}

	return 0;
}

int calculate(Item** nodes, double* result)
{
	calc_code = 0;

	*result = calcWolker(*nodes);

	return calc_code;
}
