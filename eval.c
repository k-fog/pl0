#include "pl0.h"

long eval(Node *node) {
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
	}
	exit(1);
}
