#include "pl0.h"

Env *global_env;

Env *new_env(Env *outer) {
    Env *env = calloc(1, sizeof(Env));
    env->outer = outer;
    env->var = new_hashmap(HASHMAX);
    return env;
}

pVal *pInt(int x) {
    pVal *pv = calloc(1, sizeof(pVal));
    pv->type = P_INT;
    pv->val.intnum = x;
    pv->is_const = false;
    return pv;
}

pVal *pFunc(Node *node) {
    pVal *pv = calloc(1, sizeof(pVal));
    pv->type = P_FUNC;
    pv->val.func = node;
    pv->is_const = true;
    return pv;
}

static Pair *get_pair(Env *env, char *key) {
    Pair *ret = get_from_map(env->var, key);
    if (ret) return ret;
    else if (env->outer == NULL) return NULL;
    else return get_pair(env->outer, key);
}

static pVal *get(Env *env, char *key) {
    return get_pair(env, key)->val;
}

static void put(Env *env, char *key, pVal *val) {
    Pair *new = new_pair(key, val);
    Pair *old = get_pair(env, key);
    if (old) old->val = new->val;
    else add2map(env->var, new);
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
        case ND_MOD:
            return pInt(eval(node->lhs, env)->val.intnum % eval(node->rhs, env)->val.intnum);
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
            {
                pVal *ret = eval(node->rhs, env);
                ret->type = P_RETINT;
                return ret;
            }
        case ND_WRITE:
            printf("%d", eval(node->rhs, env)->val.intnum);
            return pInt(0);
        case ND_WRITELN:
            printf("\n");
            return pInt(0);
        case ND_ASSG:
            {
                pVal *pv = eval(node->rhs, env);
                if (get(env, node->lhs->str)->is_const == true) exit(1);
                put(env, node->lhs->str, pv);
                return pv;
            }
        case ND_BEGIN:
        case ND_BLOCK:
            {
                Env *inner = new_env(env);
                node = node->body->next;
                pVal *pv;
                while (node->next != NULL) {
                    pv = eval(node, inner);
                    if (pv->type == P_RETINT) return pv;
                    node = node->next;
                }
                return eval(node, inner);
            }
        case ND_FNDEF:
            {
                pVal *pv = pFunc(node);
                put(global_env, node->name->str, pv);
                return pv;
            }
        case ND_VARS:
            {
                node = node->body->next;
                while (node != NULL) {
                    put(env, node->str, pInt(0));
                    node = node->next;
                }
                return pInt(0);
            }
        case ND_CONST:
            node = node->body->next;
            while (node != NULL) {
                eval(node, env);
                node = node->next;
            }
            return pInt(0);
        case ND_CONSTDEF:
            {
                pVal *pv = pInt(node->rhs->val);
                pv->is_const = true;
                put(env, node->lhs->str, pv);
                return pInt(0);
            }
        case ND_FNCALL:
            {
                Env *fn_env = new_env(global_env);
                Node *fn = get(global_env, node->name->str)->val.func;
                Node *params = fn->params->body->next;
                Node *args = node->args->body->next;
                while (params != NULL) {
                    put(fn_env, params->str, eval(args, env));
                    params = params->next;
                    args = args->next;
                }
                return eval(fn->body, fn_env);
            }
        default:
            exit(1);
    }
}
