#include "pl0.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("error\n");
        exit(1);
    }
    Token *tok = tokenize(argv[1]);
    view_tokens(tok);
    return 0;
}
