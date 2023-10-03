.PHONY: clean help all

SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin

CC := gcc
CFLAGS := -Wall -Wextra -Werror -pedantic
CPPFLAGS := -I$(INC_DIR)
LDFLAGS := 


SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

EXEC := $(BIN_DIR)/takuzu

all: $(EXEC)

clean:
	rm -f $(BIN_DIR)/*
	rm -f $(OBJ_DIR)/*.o

help:
	@echo -ne "make:\n"
	@echo -ne "	Classic build of project\n\n"
	@echo -ne "make clean:\n"
	@echo -ne "	Clean all the object files and all executable file\n"

#dependency

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) -o $@

$(EXEC): $(OBJS)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@

