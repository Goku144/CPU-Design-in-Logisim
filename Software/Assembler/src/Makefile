# --- config ---
BIN_DIR := bin
SRC     := HackAssembler.c
PRG     := hasm
EXE     := $(BIN_DIR)/$(PRG).exe

CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra
LDLIBS  ?= -lHacklib

.PHONY: all clean
all: $(EXE)

$(BIN_DIR):
	@mkdir -p "$(BIN_DIR)"

$(EXE): $(SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o "$@" "$<" $(LDLIBS)

clean:
	@rm -f "$(EXE)"
