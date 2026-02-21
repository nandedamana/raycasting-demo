CFLAGS=-O2 -Wall -Wextra -Wno-unused-parameter $$(pkg-config --cflags gtk+-3.0)
LDFLAGS=-lm $$(pkg-config --libs gtk+-3.0)

a.out: *.[ch]
	$(CC) $(CFLAGS) *.c $(LDFLAGS)

clean:
	rm -f a.out
