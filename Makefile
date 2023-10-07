.PHONY: all clean help

SRC_DIR := src

all:
	make -f ${SRC_DIR}/Makefile

dev:
	make -f ${SRC_DIR}/Makefile DEV_MOD=1

clean:
	make -f ${SRC_DIR}/Makefile clean

help:
	@make --quiet -f ${SRC_DIR}/Makefile help
