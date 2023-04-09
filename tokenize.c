#include "pl0.h"

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
        if (startswith(src, "==") || startswith(src, "!=") ||
            startswith(src, "<=") || startswith(src, ">=")) {
            cur = new_token(TK_OP, cur, src, 2);
            src += 2;
            continue;
        }
        if (strchr("+-*/=()<>", *src)) {
            cur = new_token(TK_OP, cur, src++, 1);
            continue;
        }

        // identifier
        if (isalpha(*src)) {
            char *start = src++;
            while (isalpha(*src) || isdigit(*src) || *src == '_') src++;
            cur = new_token(TK_IDENT, cur, start, src - start);
            continue;
        }
    }

    new_token(TK_EOF, cur, src, 0);

    return head.next;
}

static char *to_string(char *s, int len) {
    char *t = malloc(sizeof(char) * (len + 1));
    strncpy(t, s, len);
    return t;
}

void view_tokens(Token *tok) {
    for (;;) {
        switch (tok->type) {
            case TK_IDENT:
                printf("identifier: {str: \"%s\", len: %d}\n", to_string(tok->str, tok->len), tok->len);
                break;
            case TK_OP:
                printf("operator: {str: \"%s\", len: %d}\n", to_string(tok->str, tok->len), tok->len);
                break;
            case TK_NUM:
                printf("num: {val: %ld, len: %d}\n", tok->val, tok->len);
                break;
            case TK_EOF:
                printf("end of file\n");
                return;
        }
        tok = tok->next;
    }
}

Token *token; // current token

Token *peek() {
    return token;
}

Token *read() {
    Token *t = token;
    token = token->next;
    return t;
}

