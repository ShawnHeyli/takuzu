.PHONY: all clean help

CC := gcc
CFLAGS := -Wall -Werror -pedantic
CPPFLAGS := -Iinclude
LDFLAGS := 

all: bin/takuzu bin/takuzu_debug

test: bin/takuzu tests/test.sh
	@tests/test.sh

test_debug: bin/takuzu_debug tests/test.sh
	@tests/test.sh debug

clean:
	rm -f bin/*
	rm -f obj/*.o

help:
	@echo "This is a Makefile for takuzu game."
	@echo "To compile the software, type 'make' or 'make all'."
	@echo "To clean object and executable files, type 'make clean'."

bin/takuzu: src/takuzu.c src/grid.c
	$(CC) $^ $(CPPFLAGS) $(CFLAGS) -o $@

bin/takuzu_debug: src/takuzu.c src/grid.c
	$(CC) $^ $(CPPFLAGS) $(CFLAGS) -ggdb3 -o $@
