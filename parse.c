#include "pl0.h"

/*
 * Program   ::= module Id ; Block Id .
 * Block     ::= DeclList begin StmtList end
 * DeclList  ::= { Decl ; }
 * Decl      ::= ConstDecl | ProcDecl | VarDecl
 * ConstDecl ::= const ConstDeclItem { , ConstDeclItem }
 * ConstDeclItem ::= Id : Type = ConstExpr
 * ConstExpr ::= Id | Integer
 * VarDecl   ::= var VarDeclItem { , VarDeclItem }
 * VarDeclItem ::= Id : Type
 * ProcDecl  ::= procedure Id ([ FormalDecl {, FormalDecl} ] ) ; Block Id
 * FormalDecl::= Id : Type
 * Type      ::= int
 * StmtList  ::= { Stmt ; }
 * Stmt      ::= CallStmt | AssignStmt | OutStmt | IfStmt | WhileStmt
 * CallStmt  ::= Id ( [ Exprs ] )
 * AssignStmt::= Lvalue := Expr
 * Lvalue    ::= Id
 * OutStmt   ::= output := Expr
 * IfStmt    ::= if Test then StmtList end
 * WhileStmt ::= while Test do StmtList end
 * Test      ::= odd Sum | Sum Relop Sum
 * Relop     ::= <= | <> | < | >= | > | =
 * Exprs     ::= Expr {, Expr }
 * Expr      ::= Sum
 * Sum       ::= Term { (+ | -) Term }
 * Term      ::= Factor { (* | /) Factor }
 * Factor    ::= - Factor | LValue | Integer | input | ( Expr )
 */

static Node *exprs();
static Node *expr();
static Node *sum();
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

static Node *exprs() {
	Node *head = expr();
	Node *cur = head;
	while (eq_str(peek(), ",")) {
		read();
		cur->next = expr();
		cur = cur->next;
	}
	return head;
}

static Node *expr() {
	return sum();
}

static Node *sum() {
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
		Node *node = sum();
		if (!eq_str(read(), ")")) exit(1);
		return node;
	}
	exit(1);
}

Node *parse(Token *tok) {
	current_token = tok;
	Node *node = exprs();
	if (!eq_type(current_token, TK_EOF)) exit(1);
	return node;
}

static void print_node(Node *node) {
	for (Node *n = node; n != NULL; n = n->next) {
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
				print_node(node->lhs);
				printf("}, ");
				printf("rhs: {");
				print_node(node->rhs);
				printf("}, ");
				printf("}, ");
				break;
		}
	}
	return;
}

void view_ast(Node *node) {
	printf("===== PARSED =====\n");
	print_node(node);
	printf("\n");
	return;
}
