
# Makefile
#
# Copyright (C) 2009, 2013 Francesco Abbate
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or (at
# your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

include makedefs

C_SRC_FILES = str.c
CXX_SRC_FILES = agg_font_freetype.cpp render_config.cpp fatal.cpp \
	utils.cpp rendering_buffer_utils.cpp canvas_svg.cpp markers.cpp text.cpp \
	window_part.cpp draw_svg.cpp plot.cpp units.cpp window_surface.cpp \
	printf_check.cpp colors.cpp main-test.cpp

TARGETS = main-test

INCLUDES = -I/usr/include/freetype2 -I/usr/include/agg2 -I/usr/include/X11
LIBS += -lfreetype -lagg -lm -lsupc++

COMPILE = $(CC) $(CFLAGS) $(DEFS) $(INCLUDES)

CXXCOMPILE = $(CXX) $(CXXFLAGS) $(DEFS) $(INCLUDES)

PROJECT_OBJ_FILES = $(C_SRC_FILES:%.c=%.o)
PROJECT_OBJ_FILES += $(CXX_SRC_FILES:%.cpp=%.o)

DEP_FILES := $(C_SRC_FILES:%.c=.deps/%.P) $(CXX_SRC_FILES:%.cpp=.deps/%.P)

DEPS_MAGIC := $(shell mkdir .deps > /dev/null 2>&1 || :)
LIBS_MAGIC := $(shell mkdir .libs > /dev/null 2>&1 || :)

all: $(TARGETS)

main-test: $(PROJECT_OBJ_FILES)
	@echo Linking $@
	$(LINK_EXE) -o $@ $(PROJECT_OBJ_FILES) $(LIBS)

.PHONY: clean all

include makerules

clean:
	$(HOST_RM) *.o $(TARGETS)
	$(HOST_RM) -r ./.libs/

-include $(DEP_FILES)
