TARGET = hello
SRC_DIR = source
INCLUDE_DIR = include
ASSET_FOLDER = assets
SCRIPT_FOLDER = scripts

SRC = $(shell find $(SRC_DIR) -type f -name '*.c')
EXE = $(TARGET)

ifeq ($(OS),Windows_NT)
	CC := x86_64-w64-mingw32-gcc
	EXE := $(TARGET).exe
	PLATFORM_FLAGS = -DPLATFORM_WINDOWS
else
	CC := clang
	PLATFORM_FLAGS =
endif

LUA_CFLAGS  = $(shell pkg-config --cflags lua)
LUA_LDFLAGS = $(shell pkg-config --libs lua)


CFLAGS = -Wall -Wextra -std=c11 \
	-I$(INCLUDE_DIR) \
	$(PLATFORM_FLAGS) \
	$(LUA_CFLAGS)

ifeq ($(OS),Windows_NT)
	LDFLAGS = -lraylib -lSDL2main -lSDL2 -llua -mwindows
else
	LDFLAGS = -lraylib -lraygui \
		$(shell sdl2-config --cflags --libs) \
		-lm \
		$(LUA_LDFLAGS)
endif


all: $(EXE)

$(EXE): $(SRC)
	@echo "Packing assets..."
	python3 $(SCRIPT_FOLDER)/tcf.py pack $(ASSET_FOLDER) data.tcf
	@echo "Compiling..."
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
