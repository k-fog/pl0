#include "pl0.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("error\n");
        exit(1);
    }

    Token *tok = tokenize(argv[1]);
    view_tokens(tok);

	Node *node = parse(tok);
	view_ast(node);

	printf("%ld\n", eval(node));

    return 0;
}
