#include "platform.h"

#include <items_ext.h>

void (*freefunc) (void*, size_t);

void ItemInteger_destroy(Item* _this)
{
	ItemInteger* i = (ItemInteger*)_this;
	mpz_clear(i->_value);
}

void ItemInteger_toString(Item* _this, String* s)
{
	ItemInteger* i = (ItemInteger*)_this;

	char* str = mpz_get_str(NULL, 10, i->_value);

	wchar_t* wstr = (wchar_t*)malloc((strlen(str) + 1) * sizeof(wchar_t));
	assert(wstr != NULL);
	wsprintf(wstr, L"%S", str);

	String_cpy(s, wstr);

	mp_get_memory_functions(NULL, NULL, &freefunc);
	freefunc(str, strlen(str) + 1);

	free(wstr);
}

void ItemInteger_writeRegel(Item* _this, String* ins, int level)
{
	String_cpy(ins, L"I");
}

ItemInteger* ItemInteger_init(const mpz_t v)
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

	mpz_init(i->_value);
	mpz_set(i->_value, v);

	return i;
}

ItemInteger* ItemInteger_init_str(const wchar_t* wstr)
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

	char* str = (char*)malloc(wcslen(wstr) + 1);
	assert(str != NULL);

	sprintf(str, "%S", wstr);
	mpz_init_set_str(i->_value, str, 10);

	free(str);

	return i;
}