#include "pl0.h"

/*
 * program   ::= block "."
 * block     ::= [ "const" ident "=" number {"," ident "=" number} ";"]
 *               [ "var" ident {"," ident} ";"]
 *               { "procedure" ident ";" block ";" } statement
 * statement ::= [ ident ":=" expression | "call" ident |
 *               "begin" statement {";" statement } "end" |
 *               "if" condition "then" statement |
 *               "while" condition "do" statement ].
 * Condition ::= "odd" Expr | Expr ("="|"#"|"<"|"<="|">"|">=") Expr
 * Expr      ::= ["+"|"-"] Term { ("+"|"-") Term} 
 * Term      ::= Factor { (* | /) Factor }
 * Factor    ::= Ident | Number | "(" Expr ")"
 */

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

static bool eq_str(Token *t, char *s) {
	return t->len == strlen(s) && strncmp(t->str, s, t->len) == 0;
}

static bool eq_type(Token *tok, TokenType tt) {
	return tok->type == tt;
}

static Node *stmt() {
	Node *node;
	Token *tok = peek();
	if (eq_str(tok, "if")) {
		read();
		node = new_node(ND_IF);
		node->condition = condition();
		if (!eq_str(read(), "then")) exit(1);
		node->body = stmt();
	} else if (eq_str(tok, "while")) {
		read();
		node = new_node(ND_WHILE);
		node->condition = condition();
		if (!eq_str(read(), "do")) exit(1);
		node->body = stmt();
	} else {
		node = expr(); // temporarily
	}
	return node;
}

static Node *condition() {
	Node *node;
	if (eq_str(peek(), "odd")) {
		read();
		node = new_unary(ND_ODD, expr());
	} else {
		node = expr();
		Token *tok = read();
		if (eq_str(tok, "<=")) node = new_binary(ND_LE, node, expr());
		else if (eq_str(tok, "<>")) node = new_binary(ND_NE, node, expr());
		else if (eq_str(tok, "<")) node = new_binary(ND_LT, node, expr());
		else if (eq_str(tok, ">=")) node = new_binary(ND_LE, expr(), node);
		else if (eq_str(tok, ">")) node = new_binary(ND_LT, expr(), node);
		else if (eq_str(tok, "=")) node = new_binary(ND_EQ, node, expr());
	}
	return node;
}

static Node *expr() {
	Node *node;
	if (eq_str(peek(), "+")) {
		read();
		node = term();
	} else if (eq_str(peek(), "-")) {
		read();
		node = new_binary(ND_SUB, new_num(0), term());
	} else
		node = term();

	while (eq_str(peek(), "+") || eq_str(peek(), "-")) {
		if (eq_str(read(), "+"))
			node = new_binary(ND_ADD, node, term());
		else // "-"
			node = new_binary(ND_SUB, node, term());
	}
	return node;
}

static Node *term() {
	Node *node = factor();
	while (eq_str(peek(), "*") || eq_str(peek(), "/")) {
		if (eq_str(read(), "*"))
			node = new_binary(ND_MUL, node, factor());
		else // "/"
			node = new_binary(ND_DIV, node, factor());
	}
	return node;
}

static Node *factor() {
	Token *tok = read();
	if (eq_type(tok, TK_IDENT)) {
		exit(1);
	} else if (eq_str(tok, "-")) {
		return new_binary(ND_SUB, new_num(0), factor());
	} else if (eq_type(tok, TK_NUM)) {
		return new_num(tok->val);
	} else if (eq_str(tok, "(")) {
		Node *node = expr();
		if (!eq_str(read(), ")")) exit(1);
		return node;
	}
	exit(1);
}

Node *parse(Token *tok) {
	current_token = tok;
	// Node *node = condition();
	Node *node = stmt();
	if (!eq_type(current_token, TK_EOF)) exit(1);
	return node;
}

static void print_node(Node *node) {
	switch (node->type) {
		case ND_NUM:
			printf("number: {val: %ld}, ", node->val);
			break;
		case ND_ADD:
		case ND_SUB:
		case ND_MUL:
		case ND_DIV:
		case ND_EQ:
		case ND_NE:
		case ND_LT:
		case ND_LE:
			char *op;
			if (node->type == ND_ADD) op = "+";
			else if (node->type == ND_SUB) op = "-";
			else if (node->type == ND_MUL) op = "*";
			else if (node->type == ND_DIV) op = "/";
			else if (node->type == ND_EQ) op = "==";
			else if (node->type == ND_NE) op = "<>";
			else if (node->type == ND_LT) op = "<";
			else if (node->type == ND_LE) op = "<=";

			printf("%s: {", op);
			printf("lhs: {");
			print_node(node->lhs);
			printf("}, ");
			printf("rhs: {");
			print_node(node->rhs);
			printf("}, ");
			printf("}, ");
			break;
		case ND_ODD:
			printf("odd: {lhs: null, rhs: {");
			print_node(node->rhs);
			printf("}, ");
			break;
		case ND_IF:
		case ND_WHILE:
			char *kw;
			if (node->type == ND_IF) kw = "if";
			else if (node->type == ND_WHILE) kw = "while";
			printf("%s: {condition: {", kw);
			print_node(node->condition);
			printf("}, ");
			printf("body: {");
			print_node(node->body);
			printf("}, ");
			printf("}, ");
			break;
	}
	return;
}

void view_ast(Node *node) {
	printf("===== PARSED =====\n");
	print_node(node);
	printf("\n");
	return;
}
