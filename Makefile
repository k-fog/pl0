CFLAGS = -std=c11 -Wall -g -fno-common
SRCS   = main.c tokenize.c

pl0: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SRCS): pl0.h

test: pl0
	./test.sh

clean:
	rm -f *.o *~

.PHONY: test clean
