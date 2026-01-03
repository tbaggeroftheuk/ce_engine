# === Configuration ===
TARGET = hello
SRC_DIR = source
INCLUDE_DIR = include
ASSET_FOLDER = assets
SCRIPT_FOLDER = scripts

# Recursively find all .c files
SRC = $(shell find $(SRC_DIR) -type f -name '*.c')
EXE = $(TARGET)

# === Compiler Selection ===
ifeq ($(OS),Windows_NT)
	CC := x86_64-w64-mingw32-gcc
	EXE := $(TARGET).exe
	CFLAGS += -DPLATFORM_WINDOWS
	LDFLAGS = -lraylib -lSDL2main -lSDL2
else
	CC := clang
	SUBSYSTEM =
	LDFLAGS = -lraylib $(shell sdl2-config --cflags --libs)
endif



# === Flags ===
CFLAGS = -Wall -Wextra -std=c11 -I$(INCLUDE_DIR)
# On Windows, you may need to adjust LDFLAGS if raylib/sdl2 are installed differently
ifeq ($(OS),Windows_NT)
	LDFLAGS = -lraylib -lSDL2main -lSDL2 -mwindows
else
	LDFLAGS = -lraylib -lraygui $(shell sdl2-config --cflags --libs) -lm
endif

# === Build Rules ===
all: $(EXE)

$(EXE): $(SRC)
	@echo "Packing assets..."
	python3 $(SCRIPT_FOLDER)/tcf.py pack $(ASSET_FOLDER) data.tcf
	@echo "Compiling..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


run: $(EXE)
ifeq ($(OS),Windows_NT)
	@echo "Run the executable manually in Windows"
else
	./$(EXE)
endif

clean:
	rm -f $(EXE) data.tcf

debug: CFLAGS += -g
debug: clean all

# === Convenience Targets ===
gcc:
	$(MAKE) CC=gcc

clang:
	$(MAKE) CC=clang

.PHONY: all run clean debug gcc clang
