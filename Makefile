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
	@echo -ne "make arg_parser_t:\n"
	@echo -ne "	Build and run tests for check the arguments parser of takuzu module\n\n"
	@echo -ne "make arg_parser_tc:\n"
	@echo -ne "	Like make arg_parser_t PLUS COVERAGE\n\n"

