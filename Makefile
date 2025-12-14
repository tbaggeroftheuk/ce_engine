# === Configuration ===
TARGET = hello
SRC_DIR = source
INCLUDE_DIR = include
ASSET_FOLDER = assets
SCRIPT_FOLDER = scripts
SRC = $(wildcard $(SRC_DIR)/*.c)
EXE = $(TARGET)

# === Compiler/Linker Settings ===
ifeq ($(OS),Windows_NT)
	CC = x86_64-w64-mingw32-gcc
	SUBSYSTEM = -mconsole
else
	CC = gcc
	SUBSYSTEM =
endif

# === Flags ===
CFLAGS = -Wall -Wextra -std=c11 -I$(INCLUDE_DIR)
SDL_FLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_mixer

# === Build Rules ===
all: $(EXE)

$(EXE): $(SRC)
	python3 $(SCRIPT_FOLDER)/tcf.py pack $(ASSET_FOLDER) data.tcf
	$(CC) $(CFLAGS) -o $@ $^ $(SDL_FLAGS) $(SUBSYSTEM)

run: $(EXE)
	./$(EXE)

clean:
	rm -f $(EXE)

debug: CFLAGS += -g
debug: clean all
