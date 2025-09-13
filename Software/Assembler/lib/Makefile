# ===== MSYS2 / MinGW Makefile: shared library from mylib.c =====
# Build:      make
# Install:    make install
# Uninstall:  make uninstall
# Debug dirs: make print-dirs

# ---- Source & names (safe defaults) ----
SRC   ?= Hacklib.c
SNAME := $(basename $(notdir $(SRC)))
HDR   ?= $(SNAME).h

# Sanity checks (helpful error messages)
ifeq ($(strip $(wildcard $(SRC))),)
  $(error $(SRC) not found next to this Makefile)
endif
ifeq ($(strip $(wildcard $(HDR))),)
  $(error $(HDR) not found next to this Makefile)
endif

LIBBASE := lib$(SNAME)
DLL     := $(LIBBASE).dll
IMPLIB  := $(LIBBASE).dll.a
OBJ     := $(SNAME).o

# ---- Toolchain & flags ----
CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra -DWIN32_LEAN_AND_MEAN -DBUILDING_$(SNAME)_DLL
# If your header uses __declspec(dllexport) on BUILDING_$(SNAME)_DLL, you may drop --export-all-symbols.
LDFLAGS ?= -shared -Wl,--out-implib,$(IMPLIB) -Wl,--export-all-symbols

# ---- Where to install (detect MSYS2 prefix, fall back if in PowerShell) ----
PREFIX ?= $(MINGW_PREFIX)
ifeq ($(PREFIX),)
  ifeq ($(MSYSTEM),MINGW64)
    PREFIX := /mingw64
  else ifeq ($(MSYSTEM),UCRT64)
    PREFIX := /ucrt64
  else ifeq ($(MSYSTEM),CLANG64)
    PREFIX := /clang64
  else ifeq ($(MSYSTEM),MINGW32)
    PREFIX := /mingw32
  else
    # Fallback for cmd/PowerShell users:
    ifneq ($(wildcard C:/msys64/mingw64),)
      PREFIX := C:/msys64/mingw64
    else
      PREFIX := /mingw64
    endif
  endif
endif

INCDIR := $(PREFIX)/include
LIBDIR := $(PREFIX)/lib
BINDIR := $(PREFIX)/bin

RM        := rm -f
INSTALL   := install
MKDIR_P   := install -d

.PHONY: all clean install uninstall print-dirs
all: $(DLL)

$(OBJ): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(DLL): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $@

clean:
	-$(RM) $(OBJ) $(DLL) $(IMPLIB)

install: all
	$(MKDIR_P) "$(INCDIR)" "$(LIBDIR)" "$(BINDIR)"
	$(INSTALL) -m644 "$(HDR)"    "$(INCDIR)/$(notdir $(HDR))"
	$(INSTALL) -m644 "$(IMPLIB)" "$(LIBDIR)/$(notdir $(IMPLIB))"
	$(INSTALL) -m755 "$(DLL)"    "$(BINDIR)/$(notdir $(DLL))"
	@echo "Installed:"
	@echo "  Header     -> $(INCDIR)/$(notdir $(HDR))"
	@echo "  Import lib -> $(LIBDIR)/$(notdir $(IMPLIB))"
	@echo "  DLL        -> $(BINDIR)/$(notdir $(DLL))"

uninstall:
	-$(RM) "$(INCDIR)/$(notdir $(HDR))" \
	       "$(LIBDIR)/$(notdir $(IMPLIB))" \
	       "$(BINDIR)/$(notdir $(DLL))"
	@echo "Uninstalled files for $(SNAME) from $(PREFIX)."

print-dirs:
	@echo MSYSTEM=$(MSYSTEM)
	@echo MINGW_PREFIX=$(MINGW_PREFIX)
	@echo PREFIX=$(PREFIX)
	@echo INCDIR=$(INCDIR)
	@echo LIBDIR=$(LIBDIR)
	@echo BINDIR=$(BINDIR)
