#include "platform.h"

#include <panel.h>

Panel* Panel_init()
{
	Panel* p = (Panel*)malloc(sizeof(Panel));
	assert(p != NULL);

	return p;
}

void Panel_free(Panel* p)
{

}

PanelNode* PanelNode_init(Panel* p, PanelNode* nxt, PanelNode* prv)
{
	PanelNode* pn = (PanelNode*)malloc(sizeof(PanelNode));
	assert(pn);

	pn->_panel = p;
	pn->_next = nxt;
	pn->_prev = prv;

	return pn;
}

void PanelNode_free(PanelNode* pn)
{
	if (pn->_panel)
	{
		Panel_free(pn->_panel);
	}

	free(pn);
}

PanelList* PanelList_init()
{
	PanelList* pl = (PanelList*)malloc(sizeof(PanelList));
	assert(pl != NULL);

	pl->_front = NULL;
	pl->_rear = NULL;

	return pl;
}

void PanelList_free(PanelList* pl)
{
	if (pl)
	{
		if (pl->_front)
		{
			while (pl->_front)
			{
				PanelNode* pn = pl->_front;
				pl->_front = pl->_front->_next;

				PanelNode_free(pn);
			}

			pl->_front = NULL;
			pl->_rear = NULL;
		}

		free(pl);
	}
}

void PanelList_pushpack(PanelList* pl, Panel* p)
{
	if (pl->_rear == NULL)
	{
		assert(pl->_front == NULL);
		pl->_front = pl->_rear = PanelNode_init(p, NULL, NULL);
	}
	else
	{
		PanelNode* pn = PanelNode_init(p, NULL, pl->_rear);
		pl->_rear->_next = pn;
		pl->_rear = pn;
	}
}

