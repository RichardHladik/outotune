#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX, adapted by Richard Hladík
#

.DEFAULT_GOAL := all
SKIP_STRIPPING := true

run: all
	../../bin/outotune

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
	UIUtils.cpp \
	WidgetGraph.cpp \
	WidgetButton.cpp \

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_OPENGL),true)
LV2 = lv2_sep
else
LV2 = lv2_dsp
endif

../../bin/outotune.lv2/manifest.ttl: $(LV2) ../../dpf/utils/lv2_ttl_generator
	cd ../../bin/outotune.lv2 && ../../dpf/utils/lv2_ttl_generator ./outotune_dsp.so

TARGETS += $(LV2)
TARGETS += vst
ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif

BUILD_CXX_FLAGS += -std=c++17 -L../../World/build -lworld -I ../../World/src -g -lfftw3

EXTERNAL_DEPS = ../../World/build/libworld.a

../../bin/outotune: $(EXTERNAL_DEPS)
../../bin/outotune.lv2/outotune_dsp.so: $(EXTERNAL_DEPS)
../../bin/outotune.lv2/outotune_ui.so: $(EXTERNAL_DEPS)
../../bin/outotune-vst.so: $(EXTERNAL_DEPS)

all: $(TARGETS) ../../bin/outotune.lv2/manifest.ttl

../../World/build/libworld.a:
	make -C ../../World build/libworld.a

../../dpf/build/libdgl-opengl.a:
	make -C ../../dpf dgl

../../dpf/utils/lv2_ttl_generator:
	make -C ../../dpf utils/lv2_ttl_generator

benchmark: ../../World/build/libworld.a ../../bin/benchmark

../../bin/benchmark: $(OBJS_DSP) Benchmark.cpp
	-@mkdir -p $(shell dirname $@)
	@echo "Creating Benchmark standalone for $(NAME)"
	$(CXX) $^ $(BUILD_CXX_FLAGS) $(LINK_FLAGS) -o $@



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
