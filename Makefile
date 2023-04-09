CFLAGS = -std=c11 -Wall -g -fno-common
SRCS   = main.c

pl0: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: pl0
	./test.sh

clean:
	rm -f *.o *~

.PHONY: test clean
