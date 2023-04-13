#include "pl0.h"

Environment *new_environment(Environment *outer) {
	Environment *env = calloc(1, sizeof(Environment));
	env->outer = outer;
	env->var = new_hashmap(HASHMAX);
	return env;
}

static void put(Environment *env, char *key, long val) {
	Pair *p = calloc(1, sizeof(Pair));
	p->key = key;
	p->val = val;
	add2map(env->var, p);
	return;
}

static long get(Environment *env, char *key) {
	// temp
	return get_from_map(env->var, key);
}

long eval(Node *node, Environment *env) {
	switch (node->type) {
		case ND_IDENT:
			return get(env, node->str);
		case ND_NUM:
			return node->val;
		case ND_ADD:
			return eval(node->lhs, env) + eval(node->rhs, env);
		case ND_SUB:
			return eval(node->lhs, env) - eval(node->rhs, env);
		case ND_MUL:
			return eval(node->lhs, env) * eval(node->rhs, env);
		case ND_DIV:
			return eval(node->lhs, env) / eval(node->rhs, env);
		case ND_EQ:
			return eval(node->lhs, env) == eval(node->rhs, env);
		case ND_NE:
			return eval(node->lhs, env) != eval(node->rhs, env);
		case ND_LT:
			return eval(node->lhs, env) < eval(node->rhs, env);
		case ND_LE:
			return eval(node->lhs, env) <= eval(node->rhs, env);
		case ND_ODD:
			return eval(node->rhs, env) % 2 == 1;
		case ND_IF:
			if (eval(node->condition, env)) return eval(node->body, env);
			else return 0;
		case ND_WHILE:
			long ret;
			while (eval(node->condition, env)) ret = eval(node->body, env);
			return ret;
		case ND_RET:
			return eval(node->body, env);
		case ND_ASSG:
			put(env, node->lhs->str, eval(node->rhs, env));
			return 0;
		case ND_BEGIN:
			node = node->next;
			while (node->next != NULL) {
				eval(node, env);
				node = node->next;
			}
			return eval(node, env);
	}
	exit(1);
}
