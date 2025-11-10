MAIN_SRCFILES = $(shell find src -name "*.c")
MAIN_OBJFILES = $(patsubst src/%.c, build/obj/main/%.o, $(MAIN_SRCFILES))
MAIN_SRCFILES_SHADERS := $(shell find src -name "*.vert")
MAIN_SRCFILES_SHADERS += $(shell find src -name "*.frag")
MAIN_OBJFILES_SHADERS := $(patsubst src/%.vert, build/obj/%_vert.spv, $(MAIN_SRCFILES_SHADERS))
MAIN_OBJFILES_SHADERS += $(patsubst src/%.frag, build/obj/%_frag.spv, $(MAIN_OBJFILES_SHADERS))
MAIN_TARGET = build/main1
MAIN_INCLUDE = -Isrc
MAIN_LINK = -lglfw -lvulkan
MAIN_CFLAGS = -Wall -std=gnu99 -m64 -ggdb

C = gcc
GLSLC = glslc

all: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_OBJFILES_SHADERS) $(MAIN_OBJFILES)
	@mkdir -p $(dir $@)
	$(C) $(MAIN_CFLAGS) -o $@ $(MAIN_OBJFILES) $(MAIN_LINK)

build/obj/main/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(C) $(MAIN_CFLAGS) -c -o $@ $< $(MAIN_INCLUDE)

build/obj/%_vert.spv: src/%.vert
	@mkdir -p $(dir $@)
	$(GLSLC) -o $@ $<

build/obj/%_frag.spv: src/%.frag
	@mkdir -p $(dir $@)
	$(GLSLC) -o $@ $<

clean:
	@rm -r build
