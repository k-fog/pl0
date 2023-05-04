#include "pl0.h"

Env *new_env(Env *outer) {
	Env *env = calloc(1, sizeof(Env));
	env->outer = outer;
	env->var = new_hashmap(HASHMAX);
	return env;
}

pVal *pInt(long int x) {
	pVal *pv = calloc(1, sizeof(pVal));
	pv->type = P_INT;
	pv->val.intnum = x;
	return pv;
}

pVal *pFunc(Node *node) {
	pVal *pv = calloc(1, sizeof(pVal));
	pv->type = P_FUNC;
	pv->val.func = node;
	return pv;
}

static void put(Env *env, char *key, pVal *val) {
	Pair *p = new_pair(key, val);
	if (haskey(env->var, key)) update_map(env->var, p);
	else add2map(env->var, p);
	return;
}

static pVal *get(Env *env, char *key) {
	if (haskey(env->var, key))
		return get_from_map(env->var, key);
	else
		return get(env->outer, key);
}

pVal *eval(Node *node, Env *env) {
	switch (node->type) {
		case ND_IDENT:
			return get(env, node->str);
		case ND_NUM:
			return pInt(node->val);
		case ND_ADD:
			return pInt(eval(node->lhs, env)->val.intnum + eval(node->rhs, env)->val.intnum);
		case ND_SUB:
			return pInt(eval(node->lhs, env)->val.intnum - eval(node->rhs, env)->val.intnum);
		case ND_MUL:
			return pInt(eval(node->lhs, env)->val.intnum * eval(node->rhs, env)->val.intnum);
		case ND_DIV:
			return pInt(eval(node->lhs, env)->val.intnum / eval(node->rhs, env)->val.intnum);
		case ND_EQ:
			return pInt(eval(node->lhs, env)->val.intnum == eval(node->rhs, env)->val.intnum);
		case ND_NE:
			return pInt(eval(node->lhs, env)->val.intnum != eval(node->rhs, env)->val.intnum);
		case ND_LT:
			return pInt(eval(node->lhs, env)->val.intnum < eval(node->rhs, env)->val.intnum);
		case ND_LE:
			return pInt(eval(node->lhs, env)->val.intnum <= eval(node->rhs, env)->val.intnum);
		case ND_ODD:
			return pInt(eval(node->rhs, env)->val.intnum % 2 == 1);
		case ND_IF:
			if (eval(node->condition, env)->val.intnum) return eval(node->body, env);
			else return pInt(0);
		case ND_WHILE:
			{
				pVal *ret;
				while (eval(node->condition, env)) ret = eval(node->body, env);
				return ret;
			}
		case ND_RET:
			return eval(node->rhs, env);
		case ND_ASSG:
			{
				pVal *pv = eval(node->rhs, env);
				put(env, node->lhs->str, pv);
				return pv;
			}
		case ND_BEGIN:
		case ND_BLOCK:
			{
				Env *inner = new_env(env);
				node = node->body->next;
				while (node->next != NULL) {
					eval(node, inner);
					node = node->next;
				}
				return eval(node, inner);
			}
		case ND_FNDEF:
			{
				pVal *pv = pFunc(node);
				put(env, node->name->str, pv);
				printf("function %s is defined!\n", node->name->str);
				return pv;
			}
		case ND_FNCALL:
			Env *fn_env = new_env(NULL);
			return eval(get(env, node->name->str)->val.func->body, fn_env);
	}
	exit(1);
}
