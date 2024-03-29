CC     = clang
CFLAGS = -std=c11 -Wall -g -fno-common
SRCS   = main.c tokenize.c parse.c hashmap.c eval.c

pl0: $(SRCS) Makefile
	$(CC) $(CFLAGS) -o $@ $(SRCS) $(LDFLAGS)

$(SRCS): pl0.h

test: pl0
	./test.sh

clean:
	rm -f *.o *~

.PHONY: test clean
