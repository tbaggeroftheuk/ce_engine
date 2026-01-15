# =========================
# Project configuration
# =========================

TARGET        := ce_engine
SRC_DIR       := source
INCLUDE_DIR   := include

SRC := $(shell find $(SRC_DIR) -type f -name '*.cpp')
EXE := $(TARGET)

# =========================
# Host OS detection
# =========================

ifeq ($(OS),Windows_NT)
	HOST_OS := Windows
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		HOST_OS := Linux
	endif
	ifeq ($(UNAME_S),Darwin)
		HOST_OS := macOS
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
	CXX := x86_64-w64-mingw32-g++
	EXE := $(TARGET).exe
	PLATFORM_FLAGS := -DPLATFORM_WINDOWS
else
	CXX := g++
	PLATFORM_FLAGS :=
endif

# =========================
# Build flags
# =========================

CXXFLAGS := -Wall -Wextra -std=c++20 \
	-I$(INCLUDE_DIR) \
	$(PLATFORM_FLAGS) \
	-DENGINE_BUILT_ON_OS=$(HOST_OS_ESCAPED)

ifeq ($(OS),Windows_NT)
	LDFLAGS := -lraylib -lopengl32 -lgdi32 -lwinmm -luser32
else
	LDFLAGS := -lraylib -lm
endif

# =========================
# Targets
# =========================

all: $(EXE)

$(EXE): $(SRC)
	@echo "Compiling on: $(HOST_OS)"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

run: $(EXE)
ifeq ($(OS),Windows_NT)
	@echo "Run the executable manually on Windows: $(EXE)"
else
	./$(EXE)
endif

debug: CXXFLAGS += -g
debug: clean all

clean:
	rm -f $(EXE)

gcc:
	$(MAKE) CXX=g++

clang:
	$(MAKE) CXX=clang++

.PHONY: all run clean debug gcc clang
