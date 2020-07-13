#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

.DEFAULT_GOAL := all

run: all
	../../bin/outotune

lv2-ttl: lv2_dsp lv2_sep ../../dpf/utils/lv2_ttl_generator
	cd ../../bin/outotune.lv2 && ../../dpf/utils/lv2_ttl_generator ./outotune_dsp.so

clean-all: clean
	make -C ../../dpf clean
	make -C ../../World clean


# --------------------------------------------------------------
# Project name, used for binaries

NAME = outotune

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	OutotunePlugin.cpp \
	World.cpp \


FILES_UI = \
	OutotuneUI.cpp \
	WidgetUtils.cpp \
	WidgetGraph.cpp \
	WidgetButton.cpp \

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_OPENGL),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif

TARGETS += vst
ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif

BUILD_CXX_FLAGS += -std=c++17 -L../../World/build -lworld -I ../../World/src -g -lfftw3

EXTERNAL_DEPS = ../../World/build/libworld.a

jack: $(EXTERNAL_DEPS)
lv2_dsp: $(EXTERNAL_DEPS)
lv2_sep: $(EXTERNAL_DEPS)
vst: $(EXTERNAL_DEPS)

all: $(TARGETS) lv2-ttl

../../World/build/libworld.a:
	make -C ../../World build/libworld.a

../../dpf/build/libdgl-opengl.a:
	make -C ../../dpf dgl

../../dpf/utils/lv2_ttl_generator:
	make -C ../../dpf utils/lv2_ttl_generator

.PHONY: ../../World/build/libworld.a

# --------------------------------------------------------------

install: all
	mkdir -p ~/.lv2
	mkdir -p ~/.vst
	cp -r ../../bin/outotune.lv2 ~/.lv2
	cp ../../bin/outotune-vst.so ~/.vst

uninstall:
	rm -rf ~/.lv2/outotune.lv2
	rm -f ~/.vst/outotune-vst.so
