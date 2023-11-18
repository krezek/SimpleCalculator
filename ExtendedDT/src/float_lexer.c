#include <stdlib.h>
#include <assert.h>

#include "float_lexer.h"

static Token* token_init(int i, TokenTyp t, const wchar_t* s, size_t len)
{
	Token* tok = (Token*)malloc(sizeof(Token));
	assert(tok != 0);

	tok->_index = i;
	tok->_typ = t;

	tok->_str = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	assert(tok->_str != 0);

	memset(tok->_str, 0, (len + 1) * sizeof(wchar_t));
	wcsncpy_s(tok->_str, len + 1, s, len);

	return tok;
}


static void token_free(Token* tok)
{
	free(tok->_str);
	free(tok);
}

static void token_print(const Token* tok)
{
	wprintf(L"(%d,%d,\"%s\")", tok->_index, tok->_typ, tok->_str);
}

static TNode* tNode_init(Token* v)
{
	TNode* n = (TNode*)malloc(sizeof(TNode));
	assert(n != 0);

	n->_val = v;
	n->_next = NULL;

	return n;
}

static void tNode_free(TNode* n)
{
	free(n);
}

static TokensQueue* tokensQueue_init()
{
	TokensQueue* q = (TokensQueue*)malloc(sizeof(TokensQueue));
	assert(q != 0);

	q->_front = q->_rear = NULL;
	return q;
}

static void tokensQueue_enqueue(TokensQueue* q, Token* v)
{
	TNode* tmp = tNode_init(v);

	if (q->_rear == NULL) {
		q->_front = q->_rear = tmp;
		return;
	}

	q->_rear->_next = tmp;
	q->_rear = tmp;
}

static Token* tokensQueue_dequeue(TokensQueue* q)
{
	if (q->_front == NULL)
		return NULL;

	TNode* tmp = q->_front;

	q->_front = q->_front->_next;

	if (q->_front == NULL)
		q->_rear = NULL;

	Token* v = tmp->_val;

	tNode_free(tmp);

	return v;
}

static Token* tokensQueue_front(TokensQueue* q)
{
	return q->_front->_val;
}

static Token* tokensQueue_next(TokensQueue* q)
{
	if (q->_front && q->_front->_next)
	{
		return q->_front->_next->_val;
	}

	return NULL;
}

// Boolean return value
static int tokensQueue_empty(TokensQueue* q)
{
	return (q->_front) ? 0 : 1;
}

static void tokensQueue_print(TokensQueue* q)
{
	TNode* t = q->_front;
	while (t)
	{
		token_print(t->_val);
		t = t->_next;
	}
}

static void tokensQueue_free(TokensQueue* q)
{
	while (!tokensQueue_empty(q))
	{
		Token* tok = tokensQueue_dequeue(q);
		token_free(tok);
	}

	free(q);
}

int float_lexer(const wchar_t* expr, TokensQueue* queue) {
	const static int lexT[7][5] = {
		//                     op     num   e/E     .      blank
		//                     0      1      2      3      4
		/*0  new state*/    {  1,     2,     4,     5,     0 },
		/*1  op final*/     {  0,     0,     0,     0,     1 },
		/*2  number*/       {  3,     2,     3,     3,     3 },
		/*3  number final*/ {  0,     0,     0,     0,     0 },
		/*4  e/E final*/    {  0,     0,     0,     0,     4 },
		/*5  . final*/      {  0,     0,     0,     0,     5 },
	};

	static const wchar_t op[] = L"+-";
	
	int ep = 0, cp = 0;
	size_t len;
	int col, lex_state = 0;
	wchar_t c;
	Token* tok;

	len = wcslen(expr);

	while (cp <= len) {
		c = expr[cp];

		if (c && wcschr(op, c))
			col = 0; 
		else if (iswdigit(c))
			col = 1;
		else if (c == L'e' || c == L'E')
			col = 2;
		else if (c == L'.')
			col = 3;
		else if (!c || iswblank(c))
			col = 4;
		else
		{
			wprintf(L"Unknown character '%c' '%d'\n", c, c);
			return -1;
		}

		lex_state = lexT[lex_state][col];

		switch (lex_state) {
		case 1: // operator
			tok = token_init(ep, OPERATOR, expr + ep, 1);
			tokensQueue_enqueue(queue, tok);

			lex_state = 0;
			break;

		case 3: // Number
			tok = token_init(ep, NUMBER, expr + ep, cp - ep);
			tokensQueue_enqueue(queue, tok);

			--cp;
			lex_state = 0;
			break;

		case 4: // e/E
			tok = token_init(ep, E_LITERAL, expr + ep, 1);
			tokensQueue_enqueue(queue, tok);

			lex_state = 0;
			break;

		case 5: // . (dot)
			tok = token_init(ep, DOT, expr + ep, 1);
			tokensQueue_enqueue(queue, tok);

			lex_state = 0;
			break;
		}

		++cp;
		if (lex_state == 0)
			ep = cp;
	}

	return 0;
}

// Boolean return value
static int accept_tok(Token* tok, TokenTyp typ, const wchar_t* str)
{
	if ((tok->_typ == typ) && (_wcsicmp(tok->_str, str) == 0))
		return 1;

	return 0;
}
