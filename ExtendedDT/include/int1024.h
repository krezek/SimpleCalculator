#ifndef _INT1024_H_
#define _INT1024_H_

#include <wchar.h>

#define INT1024_SIZE 128

typedef struct _Int1024
{
	unsigned __int8 _sign;
	unsigned __int8 _value[INT1024_SIZE];
} Int1024;

Int1024 Int1024_init(const wchar_t* str);

wchar_t* Int1024_toString(Int1024* i);

#endif /* _INT1024_H_ */
