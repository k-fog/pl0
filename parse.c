#include "pl0.h"

/*
 * <program> ::= <block> '.'
 * <block> ::=  ( <declaration> )* <statement>
 * <declaration> ::= <const_decl> | <var_decl> | <func_decl>
 * <const_decl> ::= 'const' <const_def> ( ',' <const_def> )* ';'
 * <const_def> ::= ident '=' number
 * <var_decl> ::= 'var' ident ( ',' ident )* ';'
 * <func_decl> ::= 'function' ident '(' [ ident ( ',' ident )* ]  ')' <block> ';'
 * <statement> ::= [ ident ':=' <expression>
 *              | 'begin' <statement> ( ';' <statement> )* 'end'
 *              | 'if' <condition> 'then' <statement>
 *              | 'while' <condition> 'do' <statement>
 *              | 'return' <expression>
 *              | 'write' <expression>
 *              | 'writeln' ]
 * <condition> ::= 'odd' <expression>
 *              | <expression> ( '=' | '<>' | '<" | '<=' | '>' | '>=' ) <expression>
 * <expression> ::= [ ( '-' | '+' ) ] <term> ( ( '-' | '+' ) <term> )*
 * <term> ::= <factor> ( ( '*' | '/' ) <factor> )*
 * <factor> ::= ident '(' [ <expression> ( ',' <expression> )* ] ')'
 *              | number
 *              | ident
 *              | '(' <expression> ')'
 */

LVar *locals;

static Node *program();
static Node *block();
static Node *var_decl();
static Node *const_decl();
static Node *func_decl();
static Node *stmt();
static Node *condition();
static Node *expr();
static Node *term();
static Node *factor();

static Node *new_node(NodeType type) {
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    return node;
}

static Node *new_num(long v) {
    Node *node = new_node(ND_NUM);
    node->val = v;
    return node;
}

static Node *new_ident(Token *tok) {
    Node *node = new_node(ND_IDENT);
    node->str = to_string(tok->str, tok->len);
    return node;
}

static Node *new_unary(NodeType type, Node *p) {
    Node *node = new_node(type);
    node->rhs = p;
    return node;
}

static Node *new_binary(NodeType type, Node *lhs, Node *rhs) {
    Node *node = new_node(type);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static bool eq(Token *t, char *s) {
    return t->len == strlen(s) && strncmp(t->str, s, t->len) == 0;
}

static bool eq_type(Token *tok, TokenType tt) {
    return tok->type == tt;
}

static Node *program() {
    Node *node = block();
    if (!eq(read(), ".")) exit(0);
    return node;
}

static Node *block() {
    Node *head = new_node(ND_BLOCK);
    Node *node = head->body = new_node(ND_NULL);
    for (;;) {
        if (eq(peek(), "var")) node->next = var_decl();
        else if (eq(peek(), "const")) node->next = const_decl();
        else if (eq(peek(), "function")) node->next = func_decl();
        else break;
        node = node->next;
    }

    node->next = stmt();
    return head;
}

static Node *var_decl() {
    if (!eq(read(), "var")) exit(1);
    Node *head = new_node(ND_VARS);
    head->body = new_node(ND_NULL);
    Node *node = head->body->next = new_ident(read());
    while (eq(peek(), ",")) {
        read();
        node = node->next = new_ident(read());
    }
    if (!eq(read(), ";")) exit(1);
    return head;
}
static Node *const_decl() {
    if (!eq(read(), "const")) exit(1);
    Node *head = new_node(ND_CONST);
    Node *node = head->body = new_node(ND_NULL);
    Token *tok;
    do {
        tok = read();
        if (!eq(read(), "=")) exit(1);
        node->next = new_binary(ND_CONSTDEF, new_ident(tok), new_num(read()->val));
        node = node->next;
    } while (eq(tok = read(), ","));
    if (!eq(tok, ";")) exit(1);
    return head;
}

static Node *func_decl() {
    if (!eq(read(), "function")) exit(1);
    Node *name = new_ident(read());
    Node *params = new_node(ND_PARAMS);	
    params->body = new_node(ND_NULL);
    Node *node = params->body;
    if (!eq(read(), "(")) exit(1);
    for (;;) {
        if (eq(peek(), ")")) break;
        node->next = new_ident(read());
        node = node->next;
        if (eq(peek(), ",")) {
            read();
            continue;
        }
        else break;
    }
    if (!eq(read(), ")")) exit(1);
    Node *body = block();
    if (!eq(read(), ";")) exit(1);
    node = new_node(ND_FNDEF);
    node->name = name;
    node->params = params;
    node->body = body;
    return node;
}

static Node *stmt() {
    Node *node;
    Token *tok = read();
    if (eq(tok, "begin")) {
        Node *head = new_node(ND_BEGIN);
        head->body = new_node(ND_NULL);
        node = head->body->next = stmt();
        if (!eq(read(), ";")) exit(1);
        while (!eq(peek(), "end")) {
            node->next = stmt();
            if (!eq(read(), ";")) exit(1);
            node = node->next;
        }
        read();
        return head;
    } else if (eq(tok, "if")) {
        node = new_node(ND_IF);
        node->condition = condition();
        if (!eq(read(), "then")) exit(1);
        node->body = stmt();
        if (eq(peek(), "else")) {
            read();
            node->els = stmt();
        }
    } else if (eq(tok, "while")) {
        node = new_node(ND_WHILE);
        node->condition = condition();
        if (!eq(read(), "do")) exit(1);
        node->body = stmt();
    } else if (eq(tok, "return")) {
        node = new_unary(ND_RET, expr());
    } else if (eq(tok, "write")) {
        node = new_unary(ND_WRITE, expr());
    } else if (eq(tok, "writeln")) {
        node = new_node(ND_WRITELN);
    } else {
        node = new_ident(tok);
        if (!eq(read(), ":=")) exit(1);
        node = new_binary(ND_ASSG, node, expr());
    }
    return node;
}

static Node *condition() {
    Node *node;
    if (eq(peek(), "odd")) {
        read();
        node = new_unary(ND_ODD, expr());
    } else {
        node = expr();
        Token *tok = read();
        if (eq(tok, "<=")) node = new_binary(ND_LE, node, expr());
        else if (eq(tok, "<>")) node = new_binary(ND_NE, node, expr());
        else if (eq(tok, "<")) node = new_binary(ND_LT, node, expr());
        else if (eq(tok, ">=")) node = new_binary(ND_LE, expr(), node);
        else if (eq(tok, ">")) node = new_binary(ND_LT, expr(), node);
        else if (eq(tok, "=")) node = new_binary(ND_EQ, node, expr());
    }
    return node;
}

static Node *expr() {
    Node *node;
    if (eq(peek(), "+")) {
        read();
        node = term();
    } else if (eq(peek(), "-")) {
        read();
        node = new_binary(ND_SUB, new_num(0), term());
    } else
        node = term();

    while (eq(peek(), "+") || eq(peek(), "-")) {
        if (eq(read(), "+"))
            node = new_binary(ND_ADD, node, term());
        else // "-"
            node = new_binary(ND_SUB, node, term());
    }
    return node;
}

static Node *term() {
    Node *node = factor();
    while (eq(peek(), "*") || eq(peek(), "/") || eq(peek(), "%")) {
        Token *tok = read();
        if (eq(tok, "*"))
            node = new_binary(ND_MUL, node, factor());
        else if(eq(tok, "/"))
            node = new_binary(ND_DIV, node, factor());
        else if(eq(tok, "%"))
            node = new_binary(ND_MOD, node, factor());
    }
    return node;
}

static Node *factor() {
    Token *tok = read();
    if (eq_type(tok, TK_IDENT)) {
        if (eq(peek(), "(")) {
            read();
            Node *args = new_node(ND_ARGS);	
            args->body = new_node(ND_NULL);
            Node *node = args->body;
            for (;;) {
                if (eq(peek(), ")")) break;
                node->next = expr();
                node = node->next;
                if (eq(peek(), ",")) {
                    read();
                    continue;
                }
                else break;
            }
            if (!eq(read(), ")")) exit(1);
            node = new_node(ND_FNCALL);
            node->name = new_ident(tok);
            node->args = args;
            return node;
        } else {
            Node *node = new_node(ND_LVAR);
            LVar *lvar = find_lvar(tok);
            if (lvar) node->offset = lvar->offset;
            else {
                lvar = calloc(1, sizeof(LVar));
                lvar->next = locals;
                lvar->name = tok->str;
                lvar->len = tok->len;
                lvar->offset = locals->offset + 8;
                node->offset = lvar->offset;
                locals = lvar;
            }
            return node;
        }
    } else if (eq(tok, "-")) {
        return new_binary(ND_SUB, new_num(0), factor());
    } else if (eq_type(tok, TK_NUM)) {
        return new_num(tok->val);
    } else if (eq(tok, "(")) {
        Node *node = expr();
        if (!eq(read(), ")")) exit(1);
        return node;
    }
    exit(1);
}

Node *parse(Token *tok) {
    current_token = tok;
    locals = calloc(1, sizeof(LVar));
    locals->offset = -8;
    Node *node = program();
    if (!eq_type(current_token, TK_EOF)) exit(1);
    return node;
}

