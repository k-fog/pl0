#include "pl0.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("error\n");
        exit(1);
    }

    Token *tok = tokenize(argv[1]);
    Node *node = parse(tok);
    global_env = new_env(NULL);
    printf("%ld\n", eval(node, global_env)->val.intnum);

    return 0;
}
