# --- Toolchain & defaults (portable) --- #
UNAME_S := $(shell uname -s)

CC       ?= clang
CFLAGS   ?= -Wall -g
LDFLAGS  :=
LIBS     := -lcmdline

SRC_DIR  := src
EXEC_DIR := execs
OBJ_DIR  := $(EXEC_DIR)/obj

DOC_DIR        := docs
DOC_BUILD_DIR  := $(DOC_DIR)/builds

EXECS    := $(EXEC_DIR)/fish $(EXEC_DIR)/cmdline_test
SOURCES  := $(SRC_DIR)/cmdline.c $(SRC_DIR)/fish.c $(SRC_DIR)/cmdline_test.c $(SRC_DIR)/utils.c
OBJECTS  := $(OBJ_DIR)/cmdline.o $(OBJ_DIR)/fish.o $(OBJ_DIR)/cmdline_test.o $(OBJ_DIR)/utils.o

# --- Platform specifics --- #
ifeq ($(UNAME_S),Darwin) # macOS
	SO_EXT       := dylib
	SHARED_FLAG  := -dynamiclib
	# RPATH pour que les binaires trouvent la lib dans le même dossier
	RPATH_FLAG   := -Wl,-rpath,@executable_path
	# id de la lib : @rpath/libcmdline.dylib (bon compromis si un jour tu installes ailleurs)
	LIB_ID_FLAG  := -Wl,-install_name,@rpath/libcmdline.$(SO_EXT)
	# Sur mac, clang est par défaut ; si tu préfères gcc via Homebrew, exporte CC=gcc-14, etc.
else # Linux et autres Unix ELF
	SO_EXT       := so
	SHARED_FLAG  := -shared
	RPATH_FLAG   := -Wl,-rpath,'$$ORIGIN'
	LIB_ID_FLAG  :=
	# Sur Linux, tu utilises peut-être gcc
	CC          ?= gcc
endif

PERMANENT_FISH_EXEC     := /usr/local/bin/fish
PERMANENT_LIB_CMDLINE   := /usr/local/lib/libcmdline.$(SO_EXT)

all: install

install: dirs libs $(EXECS)

# --- Compilation Rules --- #

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/cmdline.o: $(SRC_DIR)/cmdline.c $(SRC_DIR)/cmdline.h
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

$(EXEC_DIR)/fish: $(OBJ_DIR)/fish.o $(OBJ_DIR)/utils.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -L$(EXEC_DIR) $(RPATH_FLAG)

$(EXEC_DIR)/cmdline_test: $(OBJ_DIR)/cmdline_test.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -L$(EXEC_DIR) $(RPATH_FLAG)

libs: $(OBJ_DIR)/cmdline.o
	$(CC) $(CFLAGS) $(SHARED_FLAG) $(OBJ_DIR)/cmdline.o -o $(EXEC_DIR)/libcmdline.$(SO_EXT) $(LIB_ID_FLAG)

# --- Clean Up / Prepare --- #

clean:
	rm -rf $(EXEC_DIR) $(DOC_BUILD_DIR)

dirs:
	mkdir -p $(OBJ_DIR) $(EXEC_DIR) $(DOC_BUILD_DIR)


# --- Documentation --- #
# Use doxygen (cf. https://www.doxygen.nl/) to generate documentation
full-docs: docs docs-pdf

docs: check-doxygen
	@doxygen $(DOC_DIR)/Doxyfile > /dev/null

open-docs: docs
	@open $(DOC_BUILD_DIR)/html/index.html

docs-pdf: docs
	@$(MAKE) -C $(DOC_BUILD_DIR)/latex/ > /dev/null

open-pdf: docs-pdf
	@open $(DOC_BUILD_DIR)/latex/refman.pdf

# --- Helpers --- #

check-doxygen:
	@command -v doxygen >/dev/null 2>&1 || (echo "Doxygen not found. Install it (e.g. 'brew install doxygen' on macOS)." && exit 1)

# Installation globale (optionnelle)
permanent-install: install
	@echo "Installing fish to $(PERMANENT_FISH_EXEC) and lib to $(PERMANENT_LIB_CMDLINE)"
	sudo rm -f $(PERMANENT_FISH_EXEC) $(PERMANENT_LIB_CMDLINE)
	sudo cp $(EXEC_DIR)/fish $(PERMANENT_FISH_EXEC)
	sudo cp $(EXEC_DIR)/libcmdline.$(SO_EXT) $(PERMANENT_LIB_CMDLINE)
ifeq ($(UNAME_S),Darwin)
	# Ajuster l'install_name pour pointer sur /usr/local/lib si tu installes globalement
	sudo install_name_tool -id $(PERMANENT_LIB_CMDLINE) $(PERMANENT_LIB_CMDLINE)
	# Et faire pointer fish vers cette lib
	sudo install_name_tool -change @rpath/libcmdline.$(SO_EXT) $(PERMANENT_LIB_CMDLINE) $(PERMANENT_FISH_EXEC)
else
	# Linux : mettre à jour le cache des libs si besoin
	@sudo ldconfig || true
endif
	@echo "\n\033[1;34mFish installed successfully! Try 'fish'.\033[0m"

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all / install     Build libs & execs"
	@echo "  permanent-install Copy fish and lib to /usr/local"
	@echo "  clean             Remove builds"
	@echo "  docs / full-docs  Generate docs (needs doxygen)"
	@echo "  open-docs         Open HTML docs"
	@echo "  open-pdf          Open PDF docs"

.PHONY: all clean libs dirs docs docs-pdf open-docs open-pdf check-doxygen install permanent-install help