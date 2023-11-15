#include <wchar.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "int1024.h"

extern void asm_wchar_to_bcd(const wchar_t* str, size_t strLen, unsigned __int8* pBCD);
extern void asm_double_dabble_rev(unsigned __int8* pBCD, size_t bufLen, void* value, size_t size);
extern void asm_complete2(void* value, size_t size);
extern void asm_double_dabble(unsigned __int8* pBCD, size_t bufLen, void* value, size_t size);
extern void asm_bcd_to_wchar(unsigned __int8* pBCD, size_t bufLen, wchar_t* n);
extern void asm_ignore_leading_zeros(wchar_t* n);


Int1024 Int1024_init(const wchar_t* str)
{
	Int1024 i;
	memset(&i, 0, sizeof(Int1024));

	switch (str[0])
	{
	case L'+':
		++str;
		break;
	case L'-':
		i._sign = 1;
		++str;
		break;
	}
	
	size_t strLen = wcslen(str);
	if (strLen)
	{
		size_t bufLen = (strLen & 0x1) + (strLen >> 1);
		unsigned __int8* pBCD = (unsigned __int8*)malloc(bufLen);
		assert(pBCD != NULL);

		asm_wchar_to_bcd(str, strLen, pBCD);
		asm_double_dabble_rev(pBCD, bufLen, i._value, INT1024_SIZE);

		free(pBCD);
	}

	return i;
}

wchar_t* Int1024_toString(Int1024* i)
{
	size_t bufLen = INT1024_SIZE * 2;
	unsigned __int8* pBCD = (unsigned __int8*)malloc(bufLen);
	assert(pBCD != NULL);

	// Zero memory
	memset(pBCD, 0, bufLen);

	asm_double_dabble(pBCD, bufLen, i->_value, INT1024_SIZE);

	size_t strLen = bufLen * 2 + 2 ; // reserve 2 Byte for sign and 2 Byte for null-terminator
	wchar_t* str = (wchar_t*)malloc(strLen * sizeof(wchar_t));
	assert(str != NULL);

	asm_bcd_to_wchar(pBCD, bufLen, str);
	asm_ignore_leading_zeros(str);

	free(pBCD);

	if (i->_sign)
	{
		for (size_t ix = strLen - 1; ix > 0; --ix)
		{
			str[ix] = str[ix - 1];
		}

		str[0] = L'-';
	}

	return str;
}
