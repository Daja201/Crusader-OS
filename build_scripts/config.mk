export CFLAGS = -std=c99 -g
export ASMFLAGS =
CC ?= gcc
CXX ?= g++
LD ?= gcc
ASM ?= nasm
LINKFLAGS ?=
LIBS ?=

TARGET ?= i686-elf
TARGET_ASM ?= nasm
TARGET_ASMFLAGS ?=
TARGET_CFLAGS ?= -std=c99 -g #-O2
TARGET_CC ?= $(TARGET)-gcc
TARGET_CXX ?= $(TARGET)-g++
TARGET_LD ?= $(TARGET)-gcc
TARGET_LINKFLAGS ?=
TARGET_LIBS ?=

# Export toolchain/target variables so they are available to recursive make (-C) calls
export TARGET TARGET_ASM TARGET_ASMFLAGS TARGET_CFLAGS TARGET_CC TARGET_CXX TARGET_LD TARGET_LINKFLAGS TARGET_LIBS

# Allow user to point to an existing cross-toolchain prefix. If empty, build_scripts/toolchain.mk
# will build a local toolchain under toolchain/$(TARGET). Example: make TOOLCHAIN_PREFIX=/usr/local CROSS_PREFIX=/usr/bin
TOOLCHAIN_PREFIX ?= $(abspath toolchain/$(TARGET))
export PATH := $(TOOLCHAIN_PREFIX)/bin:$(PATH)

export SOURCE_DIR = $(abspath .)
export BUILD_DIR = $(abspath build)

BINUTILS_VERSION = 2.37
BINUTILS_URL = https://ftp.gnu.org/gnu/binutils/binutils-$(BINUTILS_VERSION).tar.xz

GCC_VERSION = 11.2.0
GCC_URL = https://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz