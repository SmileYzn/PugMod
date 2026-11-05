# Makefile settings
MAKEFLAGS += --warn-undefined-variables --silent -j$(shell nproc)
#
# Build directory
BUILD_DIR=Release
#
# System based commands
PRE_CMD=mkdir -p $(BUILD_DIR)
POS_CMD=sh PostBuild.sh
CLEAN_CMD=rm -rf $(BUILD_DIR) precompiled.h.gch
#
# Common build definitions
BUILD_NAME=pugmod_mm
BUILD_SOURCES=$(wildcard *.cpp)
BUILD_CFLAGS=-std=c++14 -Wall -Wno-unknown-pragmas -fcommon -m32 -mtune=generic -march=i686 -msse2 -shared -s -pipe -g0 -O2 -fPIC -fvisibility=hidden -DNDEBUG
BUILD_TARGET=linux32
BUILD_EXTENSION=so
BUILD_OBJECTS=$(BUILD_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
BUILD_PCH=precompiled.h.gch
BUILD_COMPILER=c++
BUILD_LINKER=-static-libgcc -static-libstdc++ -Wl,--as-needed -Wl,--exclude-libs,ALL -Wl,--gc-sections -lcurl -ldl -lm
BUILD_CCFLAGS=$(BUILD_CFLAGS) -Dlinux -D__BUILD__ -DHTTP_ONLY
#
# Include directory
SRCDIR=.
METAMOD=include/metamod
SDK=include/cssdk
INCLUDE=-I$(SRCDIR) -isystem $(METAMOD) -isystem $(SDK)/common -isystem $(SDK)/dlls -isystem $(SDK)/engine -isystem $(SDK)/game_shared -isystem $(SDK)/pm_shared -isystem $(SDK)/public
#
# Defult: prebuild, linux32, posbuild
default: prebuild $(BUILD_TARGET) posbuild
#
# Linux: prebuild, linux32, posbuild
linux: $(BUILD_TARGET)
#
# Pre-build: Execute command
prebuild:
	$(PRE_CMD)
#
# Pos-build: Execute command
posbuild: $(BUILD_TARGET)
	$(POS_CMD)
#
# Linux target build
$(BUILD_TARGET): $(BUILD_OBJECTS)
	$(BUILD_COMPILER) $(INCLUDE) $(BUILD_CCFLAGS) $(BUILD_OBJECTS) $(BUILD_LINKER) -o$(BUILD_DIR)/$(BUILD_NAME).$(BUILD_EXTENSION)
$(BUILD_DIR)/%.o: %.cpp $(BUILD_PCH) | prebuild
	$(BUILD_COMPILER) $(INCLUDE) $(BUILD_CCFLAGS) -Winvalid-pch -o $@ -c $<
#
# Precompiled header (built once, used by every source file)
$(BUILD_PCH): precompiled.h
	$(BUILD_COMPILER) $(INCLUDE) $(BUILD_CCFLAGS) -x c++-header -o $@ -c $<
#
# Clean
clean:
	$(CLEAN_CMD)