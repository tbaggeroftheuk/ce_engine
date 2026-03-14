# =========================
# Project settings
# =========================
TARGET        := ce_engine
BUILD_DIR     := build
SRC_DIR       := source
INCLUDE_DIR   := include
ASSET_FOLDER  := assets
SCRIPT_FOLDER := scripts
DATA_FILE     := data.tcf
EXE           := $(TARGET)
MAKEFLAGS    += -j4

# =========================
# OS Detection
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
HOST_OS ?= Unknown
HOST_OS_ESCAPED := "\"$(HOST_OS)\""

# =========================
# Compiler
# =========================
CC  := clang
CXX := clang++
ifeq ($(OS),Windows_NT)
    EXE := $(TARGET).exe
    PLATFORM_FLAGS := -DPLATFORM_WINDOWS
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        ARCH_FLAGS := -D_WIN64 -D_M_X64
    else
        ARCH_FLAGS := -D_WIN32 -D_M_IX86
    endif
else
    PLATFORM_FLAGS :=
    ARCH_FLAGS :=
endif

# =========================
# Source files
# =========================
CPP_SRC := $(shell find $(SRC_DIR) -name '*.cpp')
C_SRC   := $(shell find $(SRC_DIR) -name '*.c')

CPP_OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SRC))
C_OBJ   := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRC))
OBJ     := $(CPP_OBJ) $(C_OBJ)

# =========================
# Compiler flags
# =========================
CFLAGS := -Wall -Wextra -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/third_party -I$(INCLUDE_DIR)/third_party/lua
CXXFLAGS := -Wall -Wextra -std=c++20 \
            -I$(INCLUDE_DIR) \
            -I$(INCLUDE_DIR)/third_party \
            -I$(INCLUDE_DIR)/third_party/imgui \
            -I$(INCLUDE_DIR)/third_party/lua \
            $(PLATFORM_FLAGS) $(ARCH_FLAGS) \
            -D_CRT_SECURE_NO_WARNINGS \
            -DENGINE_BUILT_ON_OS=$(HOST_OS_ESCAPED) \
            -DCE_DATA_FILE_NAME=\"$(DATA_FILE)\"

# =========================
# Linker flags
# =========================
ifeq ($(OS),Windows_NT)
    LDFLAGS := -lraylib -lole32 -luuid -lcomdlg32 -limm32 -loleaut32
    SUBSYSTEM_FLAG := $(if $(filter debug,$(MAKECMDGOALS)),,-Wl,-subsystem,windows)
else
    LDFLAGS := -lraylib -lm -lGL -lX11 -lpthread -ldl -lrt -lXi
    SUBSYSTEM_FLAG :=
endif

# =========================
# Asset packing
# =========================
TCF_CMD := python $(SCRIPT_FOLDER)/tcf.py pack $(ASSET_FOLDER) $(DATA_FILE)

# =========================
# Rules
# =========================
all: $(EXE)

$(EXE): $(OBJ)
	@echo "Packing assets..."
	@$(TCF_CMD)
	@echo "Linking executable on $(HOST_OS)..."
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) $(SUBSYSTEM_FLAG)

# Compile C++
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile C
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Run
run: $(EXE)
ifeq ($(OS),Windows_NT)
	@echo "Run manually: $(EXE)"
else
	./$(EXE)
endif

# Debug build
debug: CXXFLAGS += -g
debug: CFLAGS += -g
debug: clean all

# Clean
clean:
	rm -rf $(BUILD_DIR) $(EXE) $(DATA_FILE)

# Build only assets
assets:
	@echo "Packing assets only..."
	@$(TCF_CMD)

# Build executable only
exe_only: $(OBJ)
	@echo "Linking executable..."
	$(CXX) -o $(EXE) $(OBJ) $(LDFLAGS) $(SUBSYSTEM_FLAG)

# Switch compilers
gcc:
	$(MAKE) CC=gcc CXX=g++

clang:
	$(MAKE) CC=clang CXX=clang++

# Windows cross-compile (MinGW)
windows:
	$(MAKE) CC=x86_64-w64-mingw32-gcc \
	           CXX=x86_64-w64-mingw32-g++ \
	           EXE=$(TARGET).exe \
	           LDFLAGS="-lraylib -lole32 -luuid -lcomdlg32 -limm32 -loleaut32" \
	           PLATFORM_FLAGS=-DPLATFORM_WINDOWS \
	           ARCH_FLAGS=-D_WIN64 -D_M_X64 \
	           CXXFLAGS="$(CXXFLAGS) -D_CRT_SECURE_NO_WARNINGS" \
	           all

.PHONY: all run debug clean assets exe_only gcc clang windows