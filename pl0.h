#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TK_IDENT,
    TK_OP,
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

// tokenize
Token *new_token(TokenType type, Token *cur, char *str, int len);
Token *tokenize(char *src);
void view_tokens(Token *tok);
Token *peek();
Token *read();
