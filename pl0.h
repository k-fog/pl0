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
char *to_string(char *s, int len);
void view_tokens(Token *tok);
Token *peek();
Token *read();


// parse
typedef enum {
	ND_IDENT, // identifier
	ND_NUM,   // number
	ND_ADD,   // +
	ND_SUB,   // -
	ND_MUL,   // *
	ND_DIV,   // /
	ND_EQ,	  // =
	ND_LT,    // <
	ND_LE,    // <=
	ND_NE,    // <>
	ND_ODD,   // odd
	ND_IF,    // if statement
	ND_WHILE, // while statement
	ND_RET,   // return
	ND_ASSG,  // assignment
	ND_BEGIN, // begin ... end
} NodeType;

typedef struct Node Node;
struct Node {
	NodeType type;
	Node *lhs;
	Node *rhs;

	Node *condition;
	Node *body;
	
	Node *next;

	long val;
	char *str;
};

Node *parse(Token *tok);
void view_ast(Node *node);


// hashmap
typedef struct {
	char *key;
	long val;
} Pair;

typedef struct {
	int size;
	Pair **data;
} Hashmap;

Hashmap *new_hashmap(int size);
void add2map(Hashmap *map, Pair *pair);
long get_from_map(Hashmap *map, char *key);
bool haskey(Hashmap *map, char *key);


// eval
#define HASHMAX 89
typedef struct Environment Environment;
struct Environment {
	Environment *outer;
	Hashmap *var;
};

Environment *new_environment(Environment *outer);
long eval(Node *node, Environment *env);
