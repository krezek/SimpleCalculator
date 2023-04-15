#ifndef _ITEMS_EXT_H_
#define _ITEMS_EXT_H_

#include <items.h>

typedef struct
{
	Item _item;

	__int64 _value;
} ItemInteger;

ItemInteger* ItemInteger_init(const __int64 v);

#endif /* _ITEMS_EXT_H_ */
