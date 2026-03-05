# =========================
# Project settings
# =========================
TARGET        := ce_engine
ASSET_FOLDER  := assets
SCRIPT_FOLDER := scripts
SRC_DIR       := source
INCLUDE_DIR   := include

SRC := $(shell find $(SRC_DIR) -type f \( -name '*.cpp' -o -name '*.c' \) )
EXE := $(TARGET)

# =========================
# Detect OS
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
# Compiler & flags
# =========================
ifeq ($(OS),Windows_NT)
	CXX := x86_64-w64-mingw32-g++
	EXE := $(TARGET).exe
	PLATFORM_FLAGS := -DPLATFORM_WINDOWS
else
	CXX := clang++
	PLATFORM_FLAGS :=
endif

CXXFLAGS := -Wall -Wextra -std=c++20 \
    -I$(INCLUDE_DIR) \
    -I$(INCLUDE_DIR)/third_party \
    $(PLATFORM_FLAGS) \
    -DENGINE_BUILT_ON_OS=$(HOST_OS_ESCAPED)

# =========================
# Linker flags
# =========================
ifeq ($(OS),Windows_NT)
	LDFLAGS := -lraylib -lole32 -luuid -lcomdlg32 -limm32 -loleaut32 -limgui
else
	LDFLAGS := -lraylib -lm -lGL -lX11 -lpthread -ldl -lrt -lXi -limgui

endif

# =========================
# Windows subsystem: hide console for non-debug builds
# =========================
ifeq ($(OS),Windows_NT)
	ifeq ($(filter debug,$(MAKECMDGOALS)),)
		# Normal build → GUI subsystem (no console)
		SUBSYSTEM_FLAG := -Wl,-subsystem,windows
	else
		# Debug build → console
		SUBSYSTEM_FLAG :=
	endif
endif


TCF_CMD := python $(SCRIPT_FOLDER)/tcf.py pack $(ASSET_FOLDER) data.tcf



# =========================
# Build rules
# =========================
all: $(EXE)

$(EXE): $(SRC)
	@echo "Packing assets"
	@$(TCF_CMD)
	@echo "Compiling on: $(HOST_OS)"
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(SUBSYSTEM_FLAG)

# Run target
run: $(EXE)
ifeq ($(OS),Windows_NT)
	@echo "Run the executable manually on Windows: $(EXE)"
else
	./$(EXE)
endif

# Debug target
debug: CXXFLAGS += -g
debug: clean all

# Clean target
clean:
	rm -f $(EXE)

# Force use of GCC
gcc:
	$(MAKE) CXX=g++

# Force use of Clang
clang:
	$(MAKE) CXX=clang++

.PHONY: all run debug clean gcc clang
