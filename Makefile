MAIN_SRCFILES = $(shell find src -name "*.c")
MAIN_OBJFILES = $(patsubst src/%.c, build/obj/main/%.o, $(MAIN_SRCFILES))
MAIN_TARGET = build/main1
MAIN_INCLUDE = -Isrc
MAIN_LINK = -lglfw -lvulkan
MAIN_CFLAGS = -Wall -std=gnu99 -m64 -ggdb

C = gcc

all: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_OBJFILES)
	@mkdir -p $(dir $@)
	$(C) $(MAIN_CFLAGS) -o $@ $^ $(MAIN_LINK)

build/obj/main/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(C) $(MAIN_CFLAGS) -c -o $@ $< $(MAIN_INCLUDE)

clean:
	@rm -r build
