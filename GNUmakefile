# Название проекта и версия
CONF_PACKAGE = cook
CONF_VERSION = 0.1

TARGET = posix
ARCH = x86_64

# Include the generated by configure config.mak
-include config.mak

# The directory where tibs makefiles are located
DIR.TIBS ?= build/tibs

# Default build mode
MODE ?= release

# No automatic .dep building (only explicitly by make dep)
AUTODEP ?= 0

# Extra files to include in distribution
DISTEXTRA += include/ libs/ tibs/ config.mak local-config.mak

DIR.INCLUDE.C += libs/str:libs/useful:libs/cooker

# Default toolkit
ifeq ($(ARCH),arm)
TOOLKIT ?= ARM-NONE-EABI-GCC
else
TOOLKIT ?= GCC
endif

# Build tools if they aren't available
ifndef MAKEDEP
MAKEDEP = $(OUT)makedep$(EXE)
MAKEDEP_DEP = $(MAKEDEP)
endif

include $(DIR.TIBS)/rules.mak
