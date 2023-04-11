#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tokenize
typedef enum {
    TK_IDENT,
	TK_OP,
    TK_PUNCT,
    TK_NUM,
    TK_EOF,
} TokenType;

typedef struct Token Token;
struct Token {
    TokenType type;
    char *str;
    int len;
    long val;
    Token *next;
};

extern Token *current_token;

Token *new_token(TokenType type, Token *cur, char *str, int len);
Token *tokenize(char *src);
void view_tokens(Token *tok);
Token *peek();
Token *read();


// parse
typedef enum {
	ND_NUM,  // number
	ND_ADD,  // +
	ND_SUB,  // -
	ND_MUL,  // *
	ND_DIV,  // /
	ND_EQ,	 // =
	ND_LT,   // <
	ND_LE,   // <=
	ND_NE,   // <>
	ND_ODD,  // odd
	ND_IF,   // if stmt
} NodeType;

typedef struct Node Node;
struct Node {
	NodeType type;
	Node *lhs;
	Node *rhs;

	Node *condition;
	Node *body;

	long val;
};

Node *parse(Token *tok);
void view_ast(Node *node);


// eval
long eval(Node *node);
