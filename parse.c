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
	return NULL;
}
static Node *const_decl() {
	return NULL;
}

static Node *func_decl() {
	if (!eq(read(), "function")) exit(1);
	Node *name = new_ident(read());
	Node *args = new_node(ND_ARGS);	
	Node *node = args->next;
	if (!eq(read(), "(")) exit(1);
	for (;;) {
		if (eq(peek(), ")")) break;
		node->next = new_ident(read());
		node = node->next;
		if (eq(peek(), ",")) continue;
		else break;
	}
	if (!eq(read(), ")")) exit(1);
	Node *body = block();
	if (!eq(read(), ";")) exit(1);

	node = new_node(ND_FNDEF);
	node->name = name;
	node->args = args;
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
	} else if (eq(tok, "while")) {
		node = new_node(ND_WHILE);
		node->condition = condition();
		if (!eq(read(), "do")) exit(1);
		node->body = stmt();
	} else if (eq(tok, "return")) {
		node = new_unary(ND_RET, expr());
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
	while (eq(peek(), "*") || eq(peek(), "/")) {
		if (eq(read(), "*"))
			node = new_binary(ND_MUL, node, factor());
		else // "/"
			node = new_binary(ND_DIV, node, factor());
	}
	return node;
}

static Node *factor() {
	Token *tok = read();
	if (eq_type(tok, TK_IDENT)) {
		if (eq(peek(), "(")) {
			read();
			if (!eq(read(), ")")) exit(1);
			Node *node = new_node(ND_FNCALL);
			node->name = new_ident(tok);
			return node;
		}
		return new_ident(tok);
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
	// Node *node = condition();
	Node *node = program();
	if (!eq_type(current_token, TK_EOF)) exit(1);
	return node;
}

/*
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
*/

/*
void view_ast(Node *node) {
	printf("===== PARSED =====\n");
	print_node(node);
	printf("\n");
	return;
}
*/
