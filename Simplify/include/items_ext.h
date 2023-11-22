#ifndef _ITEMS_EXT_H_
#define _ITEMS_EXT_H_

#include <items.h>
#include <gmp.h>

typedef struct
{
	Item _item;

	mpz_t _value;
} ItemInteger;

ItemInteger* ItemInteger_init(const mpz_t v);
ItemInteger* ItemInteger_init_str(const wchar_t* wstr);

#endif /* _ITEMS_EXT_H_ */
