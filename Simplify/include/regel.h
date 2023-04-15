#ifndef _REGEL_H_
#define _REGEL_H_

#define REGEL_COUNT 2

typedef void (*rglFunc) (Item** item);

extern struct Regels
{
	const wchar_t* _rgl_str;
	rglFunc _rglFunc;
} g_regles[REGEL_COUNT];

#endif /* _REGEL_H_ */