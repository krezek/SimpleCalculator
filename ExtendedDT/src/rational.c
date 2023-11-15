#include <float_lexer.h>

#include "rational.h"

Rational Rational_init(const wchar_t* str)
{
	Rational r;

	r._numerator = Int1024_init(L"10");
	r._denominator = Int1024_init(L"2");

	TokensQueue* q = tokensQueue_init();
	if (float_lexer(str, q))
	{
		tokensQueue_free(q);
		return r;
	}

	tokensQueue_print(q);

	tokensQueue_free(q);

	return r;
}

wchar_t* Rational_toString(Rational* r)
{
	return NULL;
}