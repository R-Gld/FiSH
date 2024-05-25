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

PERMANENT_FISH_EXEC = /usr/local/bin/fish
PERMANENT_LIB_CMDLINE = /usr/local/lib/libcmdline.so

all: clean install

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

docs: check-doxygen
	@doxygen $(DOC_DIR)/Doxyfile > /dev/null # Generate HTML / LaTeX and Man pages documentations.

open-docs: docs
	@open $(DOC_BUILD_DIR)/html/index.html

docs-pdf: docs
	@make -C $(DOC_BUILD_DIR)/latex/ > /dev/null

open-pdf: docs-pdf
	@open $(DOC_BUILD_DIR)/latex/refman.pdf

# --- Special Targets --- #

check-doxygen:
	@dpkg -s doxygen > /dev/null 2>&1 || (echo "Doxygen is not installed. Please install it using 'sudo apt-get install doxygen'." && exit 1)

permanent-install: install
	sudo rm $(PERMANENT_FISH_EXEC) $(PERMANENT_LIB_CMDLINE) 2> /dev/null || true
	sudo ln -s $$PWD/$(EXEC_DIR)/fish $(PERMANENT_FISH_EXEC)
	sudo ln -s $$PWD/$(EXEC_DIR)/libcmdline.so $(PERMANENT_LIB_CMDLINE)
	sudo ldconfig
	@echo "\n\033[1;34mFish installed successfully!\nNow, you can run it by typing 'fish' in your terminal.\033[0m"

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "\tall:\t\t\tClean, compile and install the project"
	@echo "\tinstall:\t\tCompile and install the project"
	@echo "\tpermanent-install:\tInstall the fish shell permanently"
	@echo "\tclean:\t\t\tRemove all generated files"
	@echo "\t-----------------------------------------------------------------------------------"
	@echo "\tcheck-doxygen:\t\tCheck if doxygen is installed (The documentation generator)"
	@echo "\tdocs:\t\t\tGenerate the HTML / Manpage documentation"
	@echo "\tfull-docs:\t\tGenerate the HTML / Manpage / PDF documentation"
	@echo "\topen-docs:\t\tOpen the HTML documentation"
	@echo "\topen-pdf:\t\tOpen the PDF documentation"
	@echo "\tdocs-pdf:\t\tGenerate the PDF documentation"

.PHONY: all clean libs dirs docs docs-pdf open-docs open-pdf check-doxygen install permanent-install help
# .PHONY => tells make that these targets do not produce files
