#ifndef _PARSER_H_
#define _PARSER_H_

#include <wchar.h>

typedef struct _Parser Parser;

typedef void* (*PListFunc) (void* v, void* v1);
typedef void* (*PEquFunc) (void* v, void* v1, const wchar_t c);
typedef void* (*PAddFunc) (void* v, void* v1);
typedef void* (*PSubFunc) (void* v, void* v1);
typedef void* (*PFracFunc) (void* v, void* v1);
typedef void* (*PMultFunc) (void* v, void* v1); 
typedef void* (*PSignFunc) (void* v, const wchar_t c);
typedef void* (*PFactorialFunc) (void* v);
typedef void* (*PPowerFunc) (void* v, void* v1);
typedef void* (*PSubscriptFunc) (void* v, void* v1);
typedef void* (*PParenthesesFunc) (void* v);
typedef void* (*PCommonFunc) (void* v, void* v1, const wchar_t* s);
typedef void* (*PSqrtFunc) (void* v);
typedef void* (*PLimitFunc) (void* v, void* v1, void* v2);
typedef void* (*PDerivativeFunc) (void* v, void* v1);
typedef void* (*PIntegrateFunc) (void* v, void* v1, void* v2, void* v3);
typedef void* (*PSigmaFunc) (void* v, void* v1, void* v2);
typedef void* (*PNumberFunc) (const wchar_t* s);
typedef void* (*PSymbolFunc) (const wchar_t c);
typedef void* (*PLiteralFunc) (const wchar_t* s);

typedef struct _Parser
{
	PListFunc _listFunc;
	PEquFunc _equFunc;
	PAddFunc _addFunc;
	PSubFunc _subFunc;
	PFracFunc _fracFunc;
	PMultFunc _multFunc;
	PSignFunc _signFunc;
	PFactorialFunc _factorialFunc;
	PPowerFunc _powerFunc;
	PSubscriptFunc _subscriptFunc;
	PParenthesesFunc _parenthesesFunc;
	PCommonFunc _commonFunc;
	PSqrtFunc _sqrtFunc;
	PLimitFunc _limitFunc;
	PDerivativeFunc _derivativeFunc;
	PIntegrateFunc _integrateFunc;
	PSigmaFunc _sigmaFunc;
	PNumberFunc _numberFunc;
	PSymbolFunc _symbolFunc;
	PLiteralFunc _literalFunc;
} Parser;

int Parser_do(Parser* pp, void** pgItems, const wchar_t* s);

#endif /* _PARSER_H_ */

