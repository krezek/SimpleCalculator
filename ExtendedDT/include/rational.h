#ifndef _RATIONAL_H_
#define _RATIONAL_H_

#include "int1024.h"

typedef struct
{
	Int1024 _numerator;
	Int1024 _denominator;
} Rational;

Rational Rational_init(const wchar_t* str);

wchar_t* Rational_toString(Rational* r);

#endif /* _RATIONAL_H_ */
