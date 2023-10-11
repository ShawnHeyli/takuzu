.PHONY: all clean help

CC := clang
CFLAGS := -std=c17 -Wall -pedantic
CPPFLAGS := -Iinclude -lm
LDFLAGS := 

all: bin/takuzu bin/takuzu_debug

clean:
	rm -f bin/*
	rm -f obj/*.o

help:
	@echo -ne "make:\n"
	@echo -ne "	Classic build of project\n\n"
	@echo -ne "make clean:\n"
	@echo -ne "	Clean all the object files and all executable file\n\n"


bin/takuzu: src/takuzu.c
	$(CC) $< $(CPPFLAGS) $(CFLAGS) -o $@


bin/takuzu_debug: src/takuzu.c
	$(CC) $< $(CPPFLAGS) $(CFLAGS) -g -o $@
