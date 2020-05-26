#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Token *token;

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
Node *add();
Node *mul();
Node *primary();
Node *expr();
Node *unary();
Node *equality();
Node *relational();

bool consume(char *op);
void expect(char *op);
int expect_number();

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val){
	Node *node = calloc(1, sizeof(Node));
	node->kind = ND_NUM;
	node->val = val;
	return node;
}

Node *expr(){
	return equality();
}

Node *equality(){
	Node *node = relational();
	
	for(;;){
		if(consume("==")){
			node = new_node(ND_EQU, node, relational());
		}else if(consume("!=")){
			node = new_node(ND_NEQU, node, relational());
		}else{
			return node;
		}
	}
}

Node *relational(){
	Node *node = add();
	
	for(;;){
		if(consume("<")){
			node = new_node(ND_LES, node, add());
		}else if(consume("<=")){
			node = new_node(ND_LESE, node, add());
		}else if(consume(">")){
			node = new_node(ND_MOR, add(), node);
		}else if(consume(">=")){
			node = new_node(ND_MORE, add(), node);
		}else{
			return node;
		}
	}
}
Node *add(){
	Node *node = mul();

	for(;;){
		if(consume("+")){
			node = new_node(ND_ADD, node, mul());
		}else if(consume("-")){
			node = new_node(ND_SUB, node, mul());
		}else{
			return node;
		}
	}
}

Node *mul(){
	Node *node = unary();

	for(;;){
		if(consume("*")){
			node = new_node(ND_MUL, node, unary());
		}else if(consume("/")){
			node = new_node(ND_DIV, node, unary());
		}else{
			return node;
		}
	}
}

Node *unary(){
	if (consume("+")){
		return primary();
	}
	if (consume("-")){
		return new_node(ND_SUB, new_node_num(0), primary());
	}
	return primary();
}

Node *primary(){
	if (consume("(")){
		Node *node = expr();
		expect(")");
		return node;
	}

	return new_node_num(expect_number());
}

void gen(Node *node){
	if (node->kind == ND_NUM){
		printf("  push %d\n", node->val);
		return ;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->kind){
		case ND_ADD:
			printf("  add rax, rdi\n");
			break;
		case ND_SUB:
			printf("  sub rax, rdi\n");
			break;
		case ND_MUL:
			printf(" imul rax, rdi\n");
			break;
		case ND_DIV:
			printf("  cqo\n");
			printf("  idiv rdi\n");
			break;
		case ND_EQU:
			printf("  cmp rax, rdi\n");
			printf("  sete al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_NEQU:
			printf("  cmp rax, rdi\n");
			printf("  setne al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_LES:
			printf("  cmp rax, rdi\n");
			printf("  setl al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_LESE:
			printf("  cmp rax, rdi\n");
			printf("  setle al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_MOR:
			printf("  cmp rax, rdi\n");
			printf("  sete al\n");
			printf("  movzb rax, al\n");
			break;
		case ND_MORE:
			printf("  cmp rax, rdi\n");
			printf("  sete al\n");
			printf("  movzb rax, al\n");
			break;


	}
	printf("  push rax\n");
}



void error(char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

char *user_input;

void error_at(char *loc, char*fmt, ...){
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, "");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}
	

bool consume(char *op){
	if (token->kind != TK_RESERVED || token->len != strlen(op) || strncmp(token->str, op, token->len)){
		return false;
	}

	token = token->next;
	return true;
}

void expect (char *op){
	if (token-> kind != TK_RESERVED || token->len != strlen(op) || strncmp(token->str, op, token->len)){
		error_at(token->str, "'%c'ではありません", op);
	}
	token = token->next;
}

int expect_number(){
	if (token->kind != TK_NUM){
		error_at(token->str, "数ではありません");
	}
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof(){
	return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char* str){
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	cur->next = tok;
	return tok;
}

Token *tokenize(char *p){
	Token head;
	head.next = NULL;
	Token *cur = &head;

	while(*p){
		if (isspace(*p)){
			p++;
			continue;
		}

		if (strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0 ||  strncmp(p, ">=", 2) == 0 || strncmp(p, "<=", 2) == 0){
			cur = new_token(TK_RESERVED, cur, p);
			p += 2;
			cur->len =2;
			continue;
		}	

		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'|| *p == '<'|| *p == '>'){
			cur = new_token(TK_RESERVED, cur, p++);
			cur->len = 1;
			continue;
		}

		if (isdigit(*p)){
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue;
		}

		error_at(p,"トークナイズできません");
	}

	new_token(TK_EOF, cur, p);
	return head.next;
}


int main(int argc, char **argv){
	 if(argc != 2){ fprintf(stderr, "引数の個数が正しくありません\n");
		 return 1;
	}

	user_input = argv[1];
	token = tokenize(argv[1]);
	Node *node = expr();

	printf(".intel_syntax noprefix\n");
       	printf(".global main\n");
       	printf("main:\n");

	gen(node);

	printf("  pop rax\n");
       	printf(" ret\n"); 
	return 0;
}


