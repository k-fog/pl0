#include "pl0.h"

void codegen(Node *node) {
    switch (node->type) {
        case ND_NUM:
            printf("  push %d\n", node->val);
            return;
        default:
            break;
    }

    codegen(node->lhs);
    codegen(node->rhs);

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
    }

    printf("  push rax\n");
    return;
}
