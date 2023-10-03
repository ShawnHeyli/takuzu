CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c17 -O2
CPPFLAGS = -Wall -Wextra -Werror -pedantic -std=c17 -O2
LDFLAGS = 

.PHONY: clean help all

all: takuzu

takuzu: takuzu.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:

help:
