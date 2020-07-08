#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

run: all
	../../bin/outotune

run-dbg: all
	valgrind ../../bin/outotune

# --------------------------------------------------------------
# Project name, used for binaries

NAME = outotune

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	OutotunePlugin.cpp \
	ScaleGeneric.cpp \
	CorrectionClamp.cpp \


FILES_UI = \
	OutotuneUI.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS += lv2_dsp
TARGETS += vst
TARGETS += jack

BUILD_CXX_FLAGS += -std=c++17 -L../../World/build -lworld -I ../../World/src -g -lfftw3

jack: ../../World/build/libworld.a
all: ../../World/build/libworld.a $(TARGETS)
../../World/build/libworld.a:
	make -C ../../World

.PHONY: ../../World/build/libworld.a

# --------------------------------------------------------------
