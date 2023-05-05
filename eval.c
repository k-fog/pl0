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

static void decl_var(Env *env, char *key) {
	Pair *p = new_pair(key, pInt(0));
	add2map(env->var, p);
	return;
}

static Pair *get_pair(Env *env, char *key) {
	Pair *ret = get_from_map(env->var, key);
	if (ret) return ret;
	else if(!env->outer) return NULL;
	else return get_from_map(env->outer->var, key);
}

static pVal *get(Env *env, char *key) {
	return get_pair(env, key)->val;
}

static void put(Env *env, char *key, pVal *val) {
	Pair *np = new_pair(key, val);
	Pair *op = get_pair(env, key);
	if (op) op->val = np->val;
	else add2map(env->var, np);
	return;
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
				while (eval(node->condition, env)->val.intnum) ret = eval(node->body, env);
				return ret;
			}
		case ND_RET:
			// TODO:fix
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
				return pv;
			}
		case ND_FNCALL:
			{
				Env *fn_env = new_env(NULL);
				Node *fn = get(env, node->name->str)->val.func;
				Node *params = fn->params->body->next;
				Node *args = node->args->body->next;
				while (params != NULL) {
					put(fn_env, params->str, eval(args, env));
					params = params->next;
					args = args->next;
				}
				return eval(fn->body, fn_env);
			}
	}
	exit(1);
}
