#include "pl0.h"

long eval(Node *node) {
	while (node->next != NULL) node = node->next;
	switch (node->type) {
		case ND_NUM:
			return node->val;
		case ND_ADD:
			return eval(node->lhs) + eval(node->rhs);
		case ND_SUB:
			return eval(node->lhs) - eval(node->rhs);
		case ND_MUL:
			return eval(node->lhs) * eval(node->rhs);
		case ND_DIV:
			return eval(node->lhs) / eval(node->rhs);
		case ND_EQ:
			return eval(node->lhs) == eval(node->rhs);
		case ND_NE:
			return eval(node->lhs) != eval(node->rhs);
		case ND_LT:
			return eval(node->lhs) < eval(node->rhs);
		case ND_LE:
			return eval(node->lhs) <= eval(node->rhs);
		case ND_ODD:
			return eval(node->rhs) % 2 == 1;
	}
	exit(1);
}
