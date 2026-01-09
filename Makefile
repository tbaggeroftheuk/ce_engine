# =========================
# Project configuration
# =========================

TARGET = hello
SRC_DIR = source
INCLUDE_DIR = include
ASSET_FOLDER = assets
SCRIPT_FOLDER = scripts

SRC = $(shell find $(SRC_DIR) -type f -name '*.c')
EXE = $(TARGET)

# =========================
# Host OS detection (BUILD MACHINE)
# =========================

ifeq ($(OS),Windows_NT)
	HOST_OS := $(shell cmd.exe /C ver | tr -d '\r\n')
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		HOST_OS := $(shell . /etc/os-release 2>/dev/null && echo "$$NAME $$VERSION")
	endif
	ifeq ($(UNAME_S),Darwin)
		HOST_OS := $(shell sw_vers -productName)" "$(shell sw_vers -productVersion)
	endif
endif

ifeq ($(strip $(HOST_OS)),)
	HOST_OS := Unknown OS
endif

HOST_OS_ESCAPED := "\"$(HOST_OS)\""

# =========================
# Compiler selection
# =========================

ifeq ($(OS),Windows_NT)
	CC := x86_64-w64-mingw32-gcc
	EXE := $(TARGET).exe
	PLATFORM_FLAGS = -DPLATFORM_WINDOWS
else
	CC := clang
	PLATFORM_FLAGS =
endif

# =========================
# External libraries
# =========================

LUA_CFLAGS  = $(shell pkg-config --cflags lua)
LUA_LDFLAGS = $(shell pkg-config --libs lua)

GLIB_CFLAGS  = $(shell pkg-config --cflags glib-2.0 gmodule-2.0)
GLIB_LDFLAGS = $(shell pkg-config --libs glib-2.0 gmodule-2.0)

SDL2_FLAGS = $(shell sdl2-config --cflags --libs)

# =========================
# Build flags
# =========================

CFLAGS = -Wall -Wextra -std=c11 \
	-I$(INCLUDE_DIR) \
	$(PLATFORM_FLAGS) \
	$(LUA_CFLAGS) \
	$(GLIB_CFLAGS) \
	-DENGINE_BUILT_ON_OS=$(HOST_OS_ESCAPED)

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lraylib -lSDL2main -lSDL2 -llua -mwindows
else
	LDFLAGS = -lraylib -lraygui \
		$(SDL2_FLAGS) \
		-lm \
		$(LUA_LDFLAGS) \
		$(GLIB_LDFLAGS) \
		-lvlc
endif

# =========================
# Targets
# =========================

all: $(EXE)

$(EXE): $(SRC)
	@echo "Packing assets..."
	python3 $(SCRIPT_FOLDER)/tcf.py pack $(ASSET_FOLDER) data.tcf
	@echo "Compiling on: $(HOST_OS)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(EXE)
ifeq ($(OS),Windows_NT)
	@echo "Run the executable manually on Windows"
else
	./$(EXE)
endif

debug: CFLAGS += -g
debug: clean all

clean:
	rm -f $(EXE) data.tcf

gcc:
	$(MAKE) CC=gcc

clang:
	$(MAKE) CC=clang

.PHONY: all run clean debug gcc clang
