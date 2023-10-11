.PHONY: all clean help

SRC_TAKUZU_DIR := src

# executed by default
all:
	make -f $(SRC_TAKUZU_DIR)/Makefile all

# to clean object and executable files
clean:
	make -f $(SRC_TAKUZU_DIR)/Makefile clean

# printing helping information of how software works
help:
	@make --quiet -f $(SRC_TAKUZU_DIR)/Makefile help