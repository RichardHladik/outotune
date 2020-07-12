#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

run: all
	../../bin/outotune

run-dbg: all
	valgrind ../../bin/outotune

lv2-ttl: lv2_dsp
	cd ../../bin/outotune.lv2 && ../../dpf/utils/lv2_ttl_generator ./outotune_dsp.so



# --------------------------------------------------------------
# Project name, used for binaries

NAME = outotune

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	OutotunePlugin.cpp \
	World.cpp \
	ScaleGeneric.cpp \
	CorrectionClamp.cpp \


FILES_UI = \
	OutotuneUI.cpp \
	WidgetGraph.cpp \
	WidgetButton.cpp \

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS += lv2_dsp
TARGETS += lv2_sep
TARGETS += vst
TARGETS += jack

BUILD_CXX_FLAGS += -std=c++17 -L../../World/build -lworld -I ../../World/src -g -lfftw3

jack: ../../World/build/libworld.a
all: ../../World/build/libworld.a $(TARGETS) lv2-ttl
../../World/build/libworld.a:
	make -C ../../World

.PHONY: ../../World/build/libworld.a

# --------------------------------------------------------------

install: ${TARGETS} lv2-ttl
	mkdir -p ~/.lv2
	mkdir -p ~/.vst
	cp -r ../../bin/outotune.lv2 ~/.lv2
	cp ../../bin/outotune-vst.so ~/.vst
