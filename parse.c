#include "pl0.h"

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

static Node *new_binary(NodeType type, Node *lhs, Node *rhs) {
	Node *node = new_node(type);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

static bool eq_str(Token *t, char *s) {
	return t->len != 0 && strncmp(t->str, s, t->len) == 0;
}

static bool eq_type(Token *tok, TokenType tt) {
	return tok->type == tt;
}

static Node *expr() {
	Node *node = term();
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
	if (eq_type(peek(), TK_IDENT)) {
		exit(1);
	} else if (eq_type(peek(), TK_NUM)) {
		return new_num(read()->val);
	} else if (eq_str(peek(), "(")) {
		read();
		Node *node = expr();
		if (!eq_str(read(), ")")) exit(1);
		return node;
	}
	exit(1);
}

Node *parse(Token *tok) {
	current_token = tok;
	Node *node = expr();
	if (!eq_type(current_token, TK_EOF)) exit(1);
	return node;
}

static void indent(int depth) {
	for (int i = 0; i < depth; i++) printf("  ");
	return;
}

static void print_node(Node *node, int depth) {
	switch (node->type) {
		case ND_NUM:
			printf("number: {val: %ld}, ", node->val);
			break;
		case ND_ADD:
		case ND_SUB:
		case ND_MUL:
		case ND_DIV:
			char op;
			if (node->type == ND_ADD) op = '+';
			else if (node->type == ND_SUB) op = '-';
			else if (node->type == ND_MUL) op = '*';
			else if (node->type == ND_DIV) op = '/';

			printf("%c: {", op);
			printf("lhs: {");
			print_node(node->lhs, depth + 1);
			printf("}, ");
			printf("rhs: {");
			print_node(node->rhs, depth + 1);
			printf("}, ");
			printf("}, ");
			break;
	}
	return;
}

void view_ast(Node *node) {
	printf("===== PARSED =====\n");
	print_node(node, 0);
	printf("\n");
	return;
}
