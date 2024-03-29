#include "pl0.h"

Token *current_token;

Token *new_token(TokenType type, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->type = type;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

static bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

Token *tokenize(char *src) {
    Token head;
    Token *cur = &head;

    while (*src) {
        // skip space
        if (isspace(*src)) {
            src++;
            continue;
        }

        // number
        if (isdigit(*src)) {
            char *tmp = src;
            cur = new_token(TK_NUM, cur, src, 0);
            cur->val = strtol(src, &src, 10);
            cur->len = src - tmp;
            continue;
        }

        // operator
        if (startswith(src, "<>") || startswith(src, "<=") ||
                startswith(src, ">=") || startswith(src, ":=")) {
            cur = new_token(TK_OP, cur, src, 2);
            src += 2;
            continue;
        }
        if (strchr("+-*/=<>%", *src)) {
            cur = new_token(TK_OP, cur, src++, 1);
            continue;
        }

        // punctuator
        if (strchr("(),;.", *src)) {
            cur = new_token(TK_PUNCT, cur, src++, 1);
            continue;
        }

        // identifier
        if (isalpha(*src)) {
            char *start = src++;
            while (isalpha(*src) || isdigit(*src) || *src == '_') src++;
            cur = new_token(TK_IDENT, cur, start, src - start);
            continue;
        }

        exit(1);
    }

    new_token(TK_EOF, cur, src, 0);

    return head.next;
}

char *to_string(char *s, int len) {
    char *t = malloc(sizeof(char) * (len + 1));
    strncpy(t, s, len);
    return t;
}

Token *peek() {
    return current_token;
}

Token *read() {
    Token *t = current_token;
    current_token = current_token->next;
    return t;
}

