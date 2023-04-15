#include "platform.h"

#include <items_ext.h>

void ItemInteger_destroy(Item* _this)
{
}

void ItemInteger_toString(Item* _this, String* s)
{
	ItemInteger* i = (ItemInteger*)_this;

	wchar_t buf[20];
	_i64tow(i->_value, buf, 10);
	String_cpy(s, buf);
}

void ItemInteger_writeRegel(Item* _this, String* ins, int level)
{
	String_cpy(ins, L"I");
}

ItemInteger* ItemInteger_init(const __int64 v)
{
	ItemInteger* i = (ItemInteger*)malloc(sizeof(ItemInteger));
	assert(i != 0);

	i->_item._left = NULL;
	i->_item._right = NULL;

	i->_item._destroyFunc = ItemInteger_destroy;
	i->_item._toStringFunc = ItemInteger_toString;
	i->_item._writeRegelFunc = ItemInteger_writeRegel;

	i->_item._objectType = OBJ_Integer;
	i->_item._procLevel = PROC_L_11;

	i->_value = v;

	return i;
}