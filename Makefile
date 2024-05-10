# --- Variables --- #

CC 					= gcc
CFLAGS 				= -Wall -g
LIBS 				= -lcmdline

SRC_DIR 			= src
EXEC_DIR 			= execs
OBJ_DIR 			= $(EXEC_DIR)/obj

DOC_DIR 			= docs
DOC_BUILD_DIR		= $(DOC_DIR)/builds

EXECS 				= $(EXEC_DIR)/fish $(EXEC_DIR)/cmdline_test
SOURCES 			= $(SRC_DIR)/cmdline.c $(SRC_DIR)/fish.c $(SRC_DIR)/cmdline_test.c $(SRC_DIR)/utils.c
OBJECTS 			= $(OBJ_DIR)/cmdline.o $(OBJ_DIR)/fish.o $(OBJ_DIR)/cmdline_test.o $(OBJ_DIR)/utils.o

all: dirs libs $(EXECS) full-docs

install: dirs libs $(EXECS)

# --- Compilation Rules --- #

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/cmdline.o: $(SRC_DIR)/cmdline.c $(SRC_DIR)/cmdline.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(EXEC_DIR)/fish: $(OBJ_DIR)/fish.o  $(OBJ_DIR)/utils.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -L$(EXEC_DIR)

$(EXEC_DIR)/cmdline_test: $(OBJ_DIR)/cmdline_test.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -L$(EXEC_DIR)

libs: $(OBJ_DIR)/cmdline.o
	$(CC) $(CFLAGS) -shared $(OBJ_DIR)/cmdline.o -o $(EXEC_DIR)/libcmdline.so

# --- Clean Up / Prepare --- #

clean:
	rm -rf $(EXEC_DIR) $(DOC_BUILD_DIR)

dirs:
	mkdir -p $(OBJ_DIR) $(EXEC_DIR) $(DOC_BUILD_DIR)


# --- Documentation --- #
# Use doxygen (cf. https://www.doxygen.nl/) to generate documentation
full-docs: docs docs-pdf

docs:
	@doxygen $(DOC_DIR)/Doxyfile > /dev/null # Generate HTML / LaTeX and Man pages documentations.

open-docs: docs
	@open $(DOC_BUILD_DIR)/html/index.html

docs-pdf: docs
	@make -C $(DOC_BUILD_DIR)/latex/ > /dev/null

open-pdf: docs-pdf
	@open $(DOC_BUILD_DIR)/latex/refman.pdf

# --- Special Targets --- #

run: install
	$(EXEC_DIR)/fish

.PHONY: all clean libs dirs docs docs-pdf open-docs open-pdf
# .PHONY => tells make that these targets do not produce files
