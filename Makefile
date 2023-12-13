.PHONY: all clean help report

SRC_TAKUZU_DIR := src
REPORT_TAKUZU_DIR := report

# executed by default
all:
	make -f $(SRC_TAKUZU_DIR)/Makefile all

test:
	make -f $(SRC_TAKUZU_DIR)/Makefile test

test_debug:
	make -f $(SRC_TAKUZU_DIR)/Makefile test_debug

report: 
	cd $(REPORT_TAKUZU_DIR) && make report

# to clean object and executable files
clean:
	make -f $(SRC_TAKUZU_DIR)/Makefile clean

# printing helping information of how software works
help:
	@make --quiet -f $(SRC_TAKUZU_DIR)/Makefile help