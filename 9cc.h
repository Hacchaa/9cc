typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
	TokenKind kind;
	Token *next;
	int val;
	char *str;
	int len;
};

typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_NUM,
	ND_EQU,
	ND_NEQU,
	ND_LES,
	ND_LESE,
	ND_MOR,
	ND_MORE,
} NodeKind;

typedef struct Node Node;

struct Node{
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};

void gen(Node *node);
Node* expr();
Token* tokenize(char *p);
void error_at(char *loc, char*fmt, ...);
extern Token *token;
extern char *user_input;

