#include "pl0.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("error\n");
        exit(1);
    }

    Token *tok = tokenize(argv[1]);
    Node *node = parse(tok);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    codegen(node);
    printf("  pop rax\n");
    printf("  ret\n");

    return 0;
}
