# ---- project ----
NAME    := hacklib
SRC     := Hacklib.c
HDR     := Hacklib.h
OBJ     := $(SRC:.c=.o)

CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra -std=c11 -fPIC
LDFLAGS ?=
PREFIX  ?= /mingw64          # MSYS2 MinGW shell default; override if needed

# ---- OS detect ----
UNAME_S := $(shell uname -s)
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
  OS := windows
endif
ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
  OS := windows
endif
ifeq ($(UNAME_S),Darwin)
  OS := mac
endif
OS ?= unix

# ---- per-OS shared lib ----
ifeq ($(OS),windows)
  SHARED     := lib$(NAME).dll
  IMPORTLIB  := lib$(NAME).dll.a
  LDFLAGS   += -shared -Wl,--out-implib,$(IMPORTLIB)
  INCDIR     := $(PREFIX)/include
  LIBDIR     := $(PREFIX)/lib
  BINDIR     := $(PREFIX)/bin
else ifeq ($(OS),mac)
  SHARED     := lib$(NAME).dylib
  LDFLAGS   += -dynamiclib -Wl,-install_name,@rpath/$(SHARED)
  INCDIR     := $(PREFIX)/include
  LIBDIR     := $(PREFIX)/lib
  BINDIR     := $(PREFIX)/bin
else
  SHARED     := lib$(NAME).so
  LDFLAGS   += -shared
  INCDIR     := $(PREFIX)/include
  LIBDIR     := $(PREFIX)/lib
  BINDIR     := $(PREFIX)/bin
endif

.PHONY: all clean install uninstall

all: $(SHARED)

$(OBJ): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -I. -c $(SRC) -o $(OBJ)

$(SHARED): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(SHARED)

install: all
	install -d "$(INCDIR)" "$(LIBDIR)" "$(BINDIR)"
	install -m 644 $(HDR) "$(INCDIR)/"
ifeq ($(OS),windows)
	install -m 755 $(SHARED) "$(BINDIR)/"
	@if [ -f "$(IMPORTLIB)" ]; then install -m 644 "$(IMPORTLIB)" "$(LIBDIR)/"; fi
else
	install -m 755 $(SHARED) "$(LIBDIR)/"
endif
	@echo "Installed: $(HDR)->$(INCDIR), $(SHARED)->$(if $(filter windows,$(OS)),$(BINDIR),$(LIBDIR))"

uninstall:
	@rm -f "$(INCDIR)/$(HDR)"
ifeq ($(OS),windows)
	@rm -f "$(BINDIR)/$(SHARED)" "$(LIBDIR)/lib$(NAME).dll.a"
else
	@rm -f "$(LIBDIR)/$(SHARED)"
endif

clean:
	@rm -f $(OBJ) $(SHARED) $(IMPORTLIB)
