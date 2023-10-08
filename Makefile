.PHONY: all dev arg_parser_t arg_parser_tc clean help

SRC_TAKUZU_DIR := src
TEST_DIR := tests


# executed by default
all:
	make -f $(SRC_TAKUZU_DIR)/Makefile

# to build executable with more debug infos
dev:
	make -f $(SRC_TAKUZU_DIR)/Makefile DEV_MOD=1

# TESTING -> arguments parser
arg_parser_t:
	make dev
	make -f $(TEST_DIR)/Makefile ARG_PARSER=1

# TESTING + COVERAGE -> arguments parser 
arg_parser_tc:
	make -f $(SRC_TAKUZU_DIR)/Makefile clean
	make COVERAGE=1
	make -f $(TEST_DIR)/Makefile arg_parser_cover ARG_PARSER=1

# to clean object and executable files
clean:
	make -f $(SRC_TAKUZU_DIR)/Makefile clean
	make -f $(TEST_DIR)/Makefile clean

clean_emacs:
	make -f $(SRC_TAKUZU_DIR)/Makefile clean_emacs
	make -f $(TEST_DIR)/Makefile clean_emacs
	rm -f *~

clean_coverage:
	make -f $(SRC_TAKUZU_DIR)/Makefile clean_coverage
	make -f $(TEST_DIR)/Makefile clean_coverage

clean_full:
	make clean
	make clean_emacs
	make clean_coverage

# printing helping information of how software works
help:
	@make --quiet -f $(SRC_TAKUZU_DIR)/Makefile help
	@echo -ne "make arg_parser_t:\n"
	@echo -ne "	Build and run tests for check the arguments parser of takuzu module\n\n"
	@echo -ne "make arg_parser_tc:\n"
	@echo -ne "	Like make arg_parser_t PLUS COVERAGE\n\n"

