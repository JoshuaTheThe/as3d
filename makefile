
override CC := clang
override LD := clang
override OUTPUT := as3d
override BIN := bin
override OBJ := bin
override SRC := src

override CFILES := $(shell cd $(SRC) && find -L * -type f -name '*.c' | LC_ALL=C sort)
override OBJFILES := $(addprefix $(OBJ)/,$(CFILES:.c=.c.o))

override LDFLAGS := -lSDL2 -lm
override HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d))

.PHONY: all
all:	bin/$(OUTPUT)

$(BIN)/$(OUTPUT): $(OBJFILES)
	mkdir -p "$$(dirname $@)"
	$(LD) -o $@ $(OBJFILES) $(LDFLAGS)

-include $(HEADER_DEPS)
$(OBJ)/%.c.o: src/%.c
	mkdir -p "$$(dirname $@)"
	$(CC) -c $< -o $@

.PHONY: clean
clean:
	rm -rf bin
