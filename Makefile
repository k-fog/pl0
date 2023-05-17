CC     = clang
CFLAGS = -std=c11 -Wall -g -fno-common
SRCS   = main.c tokenize.c parse.c hashmap.c eval.c codegen.c

pl0: $(SRCS) Makefile
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDFLAGS)

$(SRCS): pl0.h

test: pl0
	./test.sh

clean:
	rm -f *.o *~ tmp*

.PHONY: test clean
