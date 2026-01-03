# =========================
# CE Engine Makefile
# Cross-platform (Linux / Windows)
# =========================

# --- Configuration ---
TARGET       = hello
SRC_DIR      = source
INCLUDE_DIR  = include
ASSET_FOLDER = assets
SCRIPT_FOLDER = scripts

# Find all .c source files recursively
SRC          = $(shell find $(SRC_DIR) -type f -name '*.c')
EXE          = $(TARGET)

# --- Compiler Selection ---
CC           := clang        # default compiler
WIN_CC       := x86_64-w64-mingw32-gcc

# --- Flags ---
CFLAGS       = -Wall -Wextra -std=c11 -I$(INCLUDE_DIR)

# Linux linker flags
LDFLAGS_LINUX   = -lraylib -lraygui -lGL -lm -ldl -lpthread -lX11

# Windows linker flags
LDFLAGS_WINDOWS = -lraylib -lraygui -lopengl32 -lgdi32 -lwinmm -lshell32 -limm32 -static -static-libgcc

# Default: Linux build
LDFLAGS = $(LDFLAGS_LINUX)

# Build type flags
DEBUG_CFLAGS = -g
RELEASE_CFLAGS = -O2

# =========================
# Build Rules
# =========================

# Default: build
all: $(EXE)

# Build executable
$(EXE): $(SRC)
	@echo "Packing assets..."
	python3 $(SCRIPT_FOLDER)/tcf.py pack $(ASSET_FOLDER) data.tcf
	@echo "Compiling..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Run the executable
run: $(EXE)
	./$(EXE)

# Clean build artifacts
clean:
	rm -f $(EXE) data.tcf

# Debug build
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: clean all

# Release build
release: CFLAGS += $(RELEASE_CFLAGS)
release: clean all

# =========================
# Windows targets
# =========================
windows: CC=$(WIN_CC)
windows: EXE=$(TARGET).exe
windows: LDFLAGS=$(LDFLAGS_WINDOWS)
windows: clean all

windows_debug: CC=$(WIN_CC)
windows_debug: EXE=$(TARGET).exe
windows_debug: LDFLAGS=$(LDFLAGS_WINDOWS)
windows_debug: CFLAGS += $(DEBUG_CFLAGS)
windows_debug: clean all

# =========================
# Convenience compiler targets
# =========================
gcc:
	$(MAKE) CC=gcc

clang:
	$(MAKE) CC=clang

# =========================
# Phony targets
# =========================
.PHONY: all run clean debug release gcc clang windows windows_debug
