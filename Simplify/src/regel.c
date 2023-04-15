#include "platform.h"

#include <items.h>
#include <regel.h>

void rgl_0(Item** item);

struct Regels g_regles[REGEL_COUNT] =
{
	{ L"N", rgl_0 }
};

// Rgl: N
void rgl_0(Item** item)
{
	printf("apply: rgl_0\n");
}
