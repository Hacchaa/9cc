#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "9cc.h"

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

Token *token;

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


