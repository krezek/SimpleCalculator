#include "platform.h"

#include <math_parser.h>
#include <lexer.h>

MParser* MParser_init()
{
	MParser* mp = (MParser*)malloc(sizeof(MParser));
	assert(mp != NULL);

	return mp;
}

void MParser_free(MParser* mp)
{
	free(mp);
}

///////////////////////// Stack //////////////////////////

typedef struct _StackNode
{
	void* _data;
	struct _StackNode* _next;
} StackNode;

static StackNode* stackNode_init(void* d)
{
	StackNode* stackNode = (StackNode*)malloc(sizeof(StackNode));
	stackNode->_data = d;
	stackNode->_next = NULL;
	return stackNode;
}

static int stack_isEmpty(StackNode* root)
{
	return !root;
}

static void stack_push(StackNode** root, void* data)
{
	StackNode* stackNode = stackNode_init(data);
	stackNode->_next = *root;
	*root = stackNode;
}

static void* stack_pop(StackNode** root)
{
	if (stack_isEmpty(*root))
		return NULL;
	StackNode* temp = *root;
	*root = (*root)->_next;
	void* popped = temp->_data;
	free(temp);

	return popped;
}

///////////////////////// Stack end //////////////////////

//    list : equ {(",") equ} .
//    equ : expr {("=") expr} .
//    expr : term {("+"|"-") term} .
//    term : factor {("*"|"/"|"%") factor} .
//    factor: "-" factorial | "+" factorial | factorial .
//    factorial : power "!" | power
//    power : subscript { "^" subscript } .
//    subscript : primary { "_" primary } .
//    primary : func "(" expr ")" | "(" expr ")" | number | literal | symbol .
//    func : "Sigma" | "Integrate" | "Derivative" | CommFunc

static int list(MParser* pp, void** pItems, TokensQueue* tokens);
static int equ(MParser* pp, void** pItems, TokensQueue* tokens);
static int expr(MParser* pp, void** pItems, TokensQueue* tokens);
static int term(MParser* pp, void** pItems, TokensQueue* tokens);
static int factor(MParser* pp, void** pItems, TokensQueue* tokens);
static int factorial(MParser* pp, void** pItems, TokensQueue* tokens);
static int power(MParser* pp, void** pItems, TokensQueue* tokens);
static int subscript(MParser* pp, void** pItems, TokensQueue* tokens);
static int primary(MParser* pp, void** pItems, TokensQueue* tokens);
static int func(MParser* pp, void** pItems, TokensQueue* tokens);
static int number(MParser* pp, void** pItems, TokensQueue* tokens);
static int literal(MParser* pp, void** pItems, TokensQueue* tokens);
static int symbol(MParser* pp, void** pItems, TokensQueue* tokens);

int MParser_do(MParser* pp, void** pItems, const wchar_t* s)
{
	void* nodes = NULL;
	int rs = 0;
	TokensQueue* q = tokensQueue_init();

	if (lexer(s, q))
	{
		tokensQueue_free(q);
		return -1;
	}
	
	//tokensQueue_print(q);

	rs = list(pp, &nodes, q);
	if (rs)
	{
		if (nodes)
		{
			//ItemTree_free(&nodes);
		}
	}
	else
	{
		*pItems = nodes;
	}

	tokensQueue_free(q);

	return rs;
}

//    list : equ {(",") equ} .
static int list(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs;
	void* node = NULL;

	rs = equ(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, L","))
			{
				tok = tokensQueue_dequeue(tokens);
				token_free(tok);

				void* r_node = NULL;
				rs = equ(pp, &r_node, tokens);
				node = pp->_listFunc(node, r_node);
				if (rs)
					break;
				
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
		}
	}

	*pItems = node;

	return rs;
}

//    equ : expr {("=") expr} .
static int equ(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs;
	void* node = NULL;

	rs = expr(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			if (accept_tok(tok, OPERATOR, L"=") |
				accept_tok(tok, OPERATOR, L"<") | 
				accept_tok(tok, OPERATOR, L">") | 
				accept_tok(tok, OPERATOR, L"\u2260") | 
				accept_tok(tok, OPERATOR, L"\u2264") | 
				accept_tok(tok, OPERATOR, L"\u2265"))
			{
				tok = tokensQueue_dequeue(tokens);
				void* r_node = NULL;
				rs = expr(pp, &r_node, tokens);
				node = pp->_equFunc(node, r_node, tok->_str[0]);
				token_free(tok);
			}
		}
	}

	*pItems = node;

	return rs;
}

//    expr : term {("+"|"-") term} .
static int expr(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs;
	void* node = NULL;

	rs = term(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, L"+") ||
				accept_tok(tok, OPERATOR, L"-"))
			{
				tok = tokensQueue_dequeue(tokens);
				if (_wcsicmp(L"+", tok->_str) == 0)
				{
					void* l_node = NULL;
					rs = term(pp, &l_node, tokens);
					node = pp->_addFunc(node, l_node);
					if (rs)
					{
						token_free(tok);
						break;
					}
				}
				else if (_wcsicmp(L"-", tok->_str) == 0)
				{
					void* l_node = NULL;
					rs = term(pp, &l_node, tokens);
					node = pp->_subFunc(node, l_node);
					if (rs)
					{
						token_free(tok);
						break;
					}
				}

				token_free(tok);
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
		}
	}

	*pItems = node;

	return rs;
}

//    term : factor {("*"|"/"|"%") factor} .
static int term(MParser* pp, void** pItems, TokensQueue* tokens)
{
	int rs;
	void* node = NULL;

	rs = factor(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, L"*") ||
				accept_tok(tok, OPERATOR, L"/"))
			{
				tok = tokensQueue_dequeue(tokens);
				if (_wcsicmp(L"*", tok->_str) == 0)
				{
					void* l_node = NULL;
					rs = factor(pp, &l_node, tokens);
					node = pp->_multFunc(node, l_node);
					if (rs)
					{
						token_free(tok);
						break;
					}
				}
				else if (_wcsicmp(L"/", tok->_str) == 0)
				{
					void* l_node = NULL;
					rs = factor(pp, &l_node, tokens);
					node = pp->_fracFunc(node, l_node);
					if (rs)
					{
						token_free(tok);
						break;
					}
				}

				token_free(tok);
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
		}
	}

	*pItems = node;

	return rs;
}

//    factor: "-" factorial | "+" factorial | factorial .
static int factor(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	Token* signToken = NULL;
	Token* tok = NULL;
	int rs;

	if (!tokensQueue_empty(tokens))
	{
		tok = tokensQueue_front(tokens);
		if (accept_tok(tok, OPERATOR, L"+") ||
			accept_tok(tok, OPERATOR, L"-"))
		{
			signToken = tokensQueue_dequeue(tokens);
		}
	}

	rs = factorial(pp, &node, tokens);

	if (signToken)
	{
		node = pp->_signFunc(node, signToken->_str[0]);
		token_free(signToken);
	}
	
	*pItems = node;

	return rs;
}

//    factorial : power "!" | power
static int factorial(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs;

	rs = power(pp, &node, tokens);

	if (!rs)
	{
		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			if (accept_tok(tok, OPERATOR, L"!"))
			{
				tok = tokensQueue_dequeue(tokens);
				node = pp->_factorialFunc(node);
				token_free(tok);
			}
		}
	}

	*pItems = node;

	return rs;
}

//    power : subscript { "^" subscript } .
static int power(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs;
	StackNode* root = NULL;

	rs = subscript(pp, &node, tokens);

	if (!rs)
	{
		stack_push(&root, node);

		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, L"^"))
			{
				tok = tokensQueue_dequeue(tokens);

				void* l_node = NULL;
				rs = subscript(pp, &l_node, tokens);
				stack_push(&root, l_node);
				if (rs)
				{
					token_free(tok);
					break;
				}

				token_free(tok);
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
		}

		node = stack_pop(&root);

		while (!stack_isEmpty(root))
		{
			void* tmp = stack_pop(&root);
			node = pp->_powerFunc(tmp, node);
		}
	}

	*pItems = node;

	return rs;
}

//    subscript : primary { "_" primary } .
static int subscript(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs;
	StackNode* root = NULL;

	rs = primary(pp, &node, tokens);

	if (!rs)
	{
		stack_push(&root, node);

		if (!tokensQueue_empty(tokens))
		{
			Token* tok = tokensQueue_front(tokens);
			while (accept_tok(tok, OPERATOR, L"_"))
			{
				tok = tokensQueue_dequeue(tokens);

				void* l_node = NULL;
				rs = primary(pp, &l_node, tokens);
				stack_push(&root, l_node);
				if (rs)
				{
					token_free(tok);
					break;
				}

				token_free(tok);
				if (tokensQueue_empty(tokens)) break;
				tok = tokensQueue_front(tokens);
			}
		}

		node = stack_pop(&root);

		while (!stack_isEmpty(root))
		{
			void* tmp = stack_pop(&root);
			node = pp->_subscriptFunc(tmp, node);
		}
	}
	
	*pItems = node;

	return rs;
}

//    primary : func "(" expr ")" | "(" expr ")" | number | literal | symbol.
static int primary(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = -1;

	if (!tokensQueue_empty(tokens))
	{
		Token* tok = tokensQueue_front(tokens);
		Token* ntok = tokensQueue_next(tokens);

		if (tok->_typ == LITERAL && ntok && 
			(accept_tok(ntok, PARENTHESE, L"(") || accept_tok(ntok, PARENTHESE, L"[") || accept_tok(ntok, PARENTHESE, L"{")))
		{
			rs = func(pp, &node, tokens);
			*pItems = node;
			return rs;
		}
		else if (accept_tok(tok, PARENTHESE, L"(")) // (expr)
		{
			tok = tokensQueue_dequeue(tokens);
			token_free(tok);

			rs = expr(pp, &node, tokens);
			*pItems = node;

			if (tokensQueue_empty(tokens)) // expect ")"
			{
				wprintf(L"Expected: \")\"\n");
				return -1;
			}

			tok = tokensQueue_front(tokens);
			if (!accept_tok(tok, PARENTHESE, L")"))
			{
				wprintf(L"Expected: \")\"\n");
				return -1;
			}

			*pItems = pp->_parenthesesFunc(*pItems);

			tok = tokensQueue_dequeue(tokens);
			token_free(tok);

			return rs;
		}
		else if (tok->_typ == NUMBER) // Number
		{
			rs = number(pp, &node, tokens);
			*pItems = node;
			return rs;
		}
		else if (tok->_typ == LITERAL) // Literal
		{
			rs = literal(pp, &node, tokens);
			*pItems = node;
			return rs;
		}
		else if (tok->_typ == SYMBOL) // SYMBOL
		{
			rs = symbol(pp, &node, tokens);
			*pItems = node;
			return rs;
		}
	}
	else
		rs = -1;

	*pItems = node;

	return rs;
}

//    func : "Sigma" | "Integrate" | "Derivative" | CommFunc
static int func(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = -1;

	Token* tok_func_name = tokensQueue_dequeue(tokens); // Function name

	Token* tok_par = tokensQueue_dequeue(tokens); // Parenthes
	token_free(tok_par);

	if (accept_tok(tok_func_name, LITERAL, L"Sigma"))
	{
		rs = expr(pp, &node, tokens);
		if (!rs)
		{
			void* bNode = NULL;
			void* tNode = NULL;

			Token* bTok = tokensQueue_front(tokens);
			if (accept_tok(bTok, OPERATOR, L";"))
			{
				bTok = tokensQueue_dequeue(tokens);
				token_free(bTok);

				rs = list(pp, &bNode, tokens);

				if (!rs)
				{
					Token* tTok = tokensQueue_front(tokens);
					if (accept_tok(tTok, OPERATOR, L";"))
					{
						tTok = tokensQueue_dequeue(tokens);
						token_free(tTok);

						rs = list(pp, &tNode, tokens);
					}
				}
			}

			node = pp->_sigmaFunc(node, bNode, tNode);
		}

		*pItems = node;
	}
	else if (accept_tok(tok_func_name, LITERAL, L"Integrate"))
	{
		rs = expr(pp, &node, tokens);
		if (!rs)
		{
			void* rNode = NULL;
			void* bNode = NULL;
			void* tNode = NULL;

			Token* rTok = tokensQueue_front(tokens);

			if (accept_tok(rTok, OPERATOR, L";"))
			{
				rTok = tokensQueue_dequeue(tokens);
				token_free(rTok);

				rs = literal(pp, &rNode, tokens);

				if (!rs)
				{
					Token* bTok = tokensQueue_front(tokens);

					if (accept_tok(bTok, OPERATOR, L";"))
					{
						bTok = tokensQueue_dequeue(tokens);
						token_free(bTok);

						rs = list(pp, &bNode, tokens);

						if (!rs)
						{
							Token* tTok = tokensQueue_front(tokens);
							if (accept_tok(tTok, OPERATOR, L";"))
							{
								tTok = tokensQueue_dequeue(tokens);
								token_free(tTok);

								rs = list(pp, &tNode, tokens);
							}
						}
					}
				}
			}

			node = pp->_integrateFunc(node, rNode, bNode, tNode);
		}

		*pItems = node;
	}
	else if (accept_tok(tok_func_name, LITERAL, L"Derivative"))
	{
		rs = expr(pp, &node, tokens);
		if (!rs)
		{
			void* rNode = NULL;
			//Item* tNode = NULL;

			Token* rTok = tokensQueue_front(tokens);
			if (accept_tok(rTok, OPERATOR, L";"))
			{
				rTok = tokensQueue_dequeue(tokens);
				token_free(rTok);

				rs = literal(pp, &rNode, tokens);
				//rNode = (Item*)ItemFrac_init(
				//	(Item*)ItemLiteral_init(L"d"),
				//	rNode
				//);

				/*if (!rs)
				{
					Token* tTok = tokensQueue_front(tokens);
					if (accept_tok(tTok, OPERATOR, L";"))
					{
						tTok = tokensQueue_dequeue(tokens);
						token_free(tTok);

						rs = list(&tNode, tokens);
					}
				}*/
			}

			node = pp->_derivativeFunc(node, rNode);
		}

		*pItems = node;
	}
	else if (accept_tok(tok_func_name, LITERAL, L"Limit"))
	{
		rs = expr(pp, &node, tokens);
		if (!rs)
		{
			void* t1Node = NULL;
			void* t2Node = NULL;

			Token* bTok = tokensQueue_front(tokens);
			if (accept_tok(bTok, OPERATOR, L";"))
			{
				bTok = tokensQueue_dequeue(tokens);
				token_free(bTok);

				rs = list(pp, &t1Node, tokens);

				if (!rs)
				{
					Token* tTok = tokensQueue_front(tokens);
					if (accept_tok(tTok, OPERATOR, L";"))
					{
						tTok = tokensQueue_dequeue(tokens);
						token_free(tTok);

						rs = expr(pp, &t2Node, tokens);
					}
				}
			}

			node = pp->_limitFunc(node, t1Node, t2Node);
		}

		*pItems = node;
	}
	else if (accept_tok(tok_func_name, LITERAL, L"Sqrt"))
	{
		rs = expr(pp, &node, tokens);
		node = pp->_sqrtFunc(node);
		*pItems = node;
	}
	else
	{
		void* rNode = NULL;

		rs = expr(pp, &node, tokens);
		if (!rs)
		{
			Token* rTok = tokensQueue_front(tokens);
			if (accept_tok(rTok, OPERATOR, L";"))
			{
				rTok = tokensQueue_dequeue(tokens);
				token_free(rTok);

				rs = expr(pp, &rNode, tokens);
			}
		}

		node = pp->_commonFunc(node, rNode, tok_func_name->_str);
		*pItems = node;
	}

	if (tokensQueue_empty(tokens)) // expect "Parenthes"
	{
		wprintf(L"Expected: \"Parenthes\"\n");
		token_free(tok_func_name);
		return -1;
	}

	tok_par = tokensQueue_front(tokens);
	if (!accept_tok(tok_par, PARENTHESE, L")") &&
		!accept_tok(tok_par, PARENTHESE, L"]") &&
		!accept_tok(tok_par, PARENTHESE, L"}"))
	{
		wprintf(L"Expected: \"Parenthes\"\n");
		token_free(tok_func_name);
		return -1;
	}

	tok_par = tokensQueue_dequeue(tokens); // Parenthes
	token_free(tok_par);

	token_free(tok_func_name);

	return rs;
}

//    number .
static int number(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = -1;

	Token* tok = tokensQueue_dequeue(tokens);
	node = pp->_numberFunc(tok->_str);
	token_free(tok);

	*pItems = node;
	return (rs = 0);
}

//    symbol .
static int symbol(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = -1;

	Token* tok = tokensQueue_dequeue(tokens);
	node = pp->_symbolFunc(tok->_str[0]);
	token_free(tok);

	*pItems = node;
	return (rs = 0);
}

//    literal .
static int literal(MParser* pp, void** pItems, TokensQueue* tokens)
{
	void* node = NULL;
	int rs = -1;

	Token* tok = tokensQueue_dequeue(tokens);
	node = pp->_literalFunc(tok->_str);
	token_free(tok);

	*pItems = node;
	return (rs = 0);
}
