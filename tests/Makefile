.PHONY: all arg_parser_cover clean clean_emacs clean_coverage

TEST_DIR := tests

SRC_TEST_DIR := $(TEST_DIR)/src
INC_TEST_DIR := $(TEST_DIR)/include
OBJ_TEST_DIR := $(TEST_DIR)/obj
BIN_TEST_DIR := $(TEST_DIR)/bin
VU_GIRD_DIR := $(TEST_DIR)/valid_gird/us
COVERAGE_DIR := $(TEST_DIR)/coverage
COVERAGE_LOG_DIR := $(COVERAGE_DIR)/log
COVERAGE_SCRIPTS_DIR := $(COVERAGE_DIR)/scripts
COVERAGE_HTML_DIR := $(COVERAGE_DIR)/html

BIN_TAKUZU_DIR := bin
SRC_TAKUZU_DIR := src
OBJ_TAKUZU_DIR := obj


CC := gcc
CFLAGS := -Wall -Wextra -Werror 
CPPFLAGS := -I$(INC_TEST_DIR)
LDFLAGS := 

SRCS :=
OBJS :=

EXEC := 

ALWAYS_SRCS := $(SRC_TEST_DIR)/all_tests.c $(SRC_TEST_DIR)/cu_test.c

ifdef ALL_TESTS
SRCS := $(wildcard $(SRC_TEST_DIR)/*.c)
EXEC := $(BIN_TEST_DIR)/all_tests
endif

ifdef ARG_PARSER
SRCS := $(ALWAYS_SRCS) $(SRC_TEST_DIR)/arguments_parser.c
CPPFLAGS += -DBIN_PATH="\"./$(BIN_TAKUZU_DIR)/\"" -DVUGIRD_PATH="\"$(VU_GIRD_DIR)/\"" \
-DLOG_PATH="\"$(COVERAGE_LOG_DIR)\"" -DARG_PARSER
EXEC := $(BIN_TEST_DIR)/arg_parser
endif

OBJS := $(SRCS:$(SRC_TEST_DIR)/%.c=$(OBJ_TEST_DIR)/%.o)


all: $(EXEC)

arg_parser_cover: $(EXEC)
	@echo -ne "" > $(COVERAGE_LOG_DIR)/out.log
	@echo -ne "" > $(COVERAGE_LOG_DIR)/err.log
	@./$(EXEC)
	@./$(COVERAGE_SCRIPTS_DIR)/gcovr.sh $(SRC_TAKUZU_DIR)/takuzu.c \
$(COVERAGE_HTML_DIR)/takuzu.html

clean:
	rm -f $(BIN_TEST_DIR)/*
	rm -f $(OBJ_TEST_DIR)/*

clean_emacs:
	rm -f $(TEST_DIR)/*~
	rm -f $(SRC_TEST_DIR)/*~
	rm -f $(INC_TEST_DIR)/*~

clean_coverage:
	rm -rf $(COVERAGE_HTML_DIR)/*


#dependency
$(OBJ_TEST_DIR)/all_tests.o: $(INC_TEST_DIR)/cu_test.h $(INC_TEST_DIR)/arguments_parser.h
$(OBJ_TEST_DIR)/cu_test.o: $(INC_TEST_DIR)/cu_test.h
$(OBJ_TEST_DIR)/arguments_parser.o: $(INC_TEST_DIR)/cu_test.h $(INC_TEST_DIR)/arguments_parser.h


#builder
$(OBJ_TEST_DIR)/%.o: $(SRC_TEST_DIR)/%.c
	$(CC) -c $< $(CPPFLAGS) $(CFLAGS) -o $@

$(EXEC): $(OBJS)
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $@
