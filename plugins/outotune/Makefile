#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = outotune

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	OutotunePlugin.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS += lv2_dsp
TARGETS += vst
TARGETS += jack

BUILD_CXX_FLAGS += -laubio

all: $(TARGETS)

# --------------------------------------------------------------
