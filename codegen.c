#include "pl0.h"

LVar *locals;

static int label_id = 0;

LVar *find_lvar(Token *tok) {
    for (LVar *v = locals; v; v = v->next)
        if (v->len == tok->len && !memcmp(tok->str, v->name, v->len))
            return v;
    return NULL;
}

static void gen_lval(Node *node) {
    if (node->type != ND_LVAR) exit(1);
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
    return;
}

static void gen(Node *node) {
    switch (node->type) {
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;
        case ND_ASSG:
            gen_lval(node->lhs);
            gen(node->rhs);
            printf("  pop rdi\n");
            printf("  pop rax\n");
            printf("  mov [rax], rdi\n");
            printf("  push rdi\n");
            return;
        case ND_RET:
            gen(node->rhs);
            printf("  pop rax\n");
            printf("  mov rsp, rbp\n");
            printf("  pop rbp\n");
            printf("  ret\n");
            return;
        case ND_IF:
            gen(node->condition);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            if (node->els)
                printf("  je .L.els.%05d\n", label_id);
            else
                printf("  je .L.end.%05d\n", label_id);
            gen(node->body);
            if (node->els) {
                printf(".L.els.%05d:\n", label_id);
                gen(node->els);
            }
            printf(".L.end.%05d:\n", label_id);
            label_id++;
            return;
        case ND_WHILE:
            printf(".L.bgn.%05d:\n", label_id);
            gen(node->condition);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je .L.end.%05d\n", label_id);
            gen(node->body);
            printf("  jmp .L.bgn.%05d\n", label_id);
            printf(".L.end.%05d:\n", label_id);
            label_id++;
            return;
        case ND_BLOCK:
        case ND_BEGIN:
            if (node->body) printf("hey!!\n");
            node = node->body->next;
            while (node) {
                gen(node->body);
                printf("  pop rax\n");
                node = node->next;
            }
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    switch (node->type) {
        case ND_ADD:
            printf("  add rax, rdi\n");
            break;
        case ND_SUB:
            printf("  sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("  imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("  cqo\n");
            printf("  idiv rdi\n");
            break;
        case ND_EQ:
            printf("  cmp rax, rdi\n");
            printf("  sete al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_NE:
            printf("  cmp rax, rdi\n");
            printf("  setne al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LT:
            printf("  cmp rax, rdi\n");
            printf("  setl al\n");
            printf("  movzb rax, al\n");
            break;
        case ND_LE:
            printf("  cmp rax, rdi\n");
            printf("  setle al\n");
            printf("  movzb rax, al\n");
            break;
    }

    printf("  push rax\n");
    return;
}

void codegen(Node *node) {
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    gen(node);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
}
