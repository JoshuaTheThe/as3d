
override CPC := clang++
override LD := clang
override OUTPUT := as3d
override BIN := bin
override OBJ := bin
override SRC := src

override CXXFILES := $(shell cd $(SRC) && find -L * -type f -name '*.cpp' | LC_ALL=C sort)
override OBJFILES := $(addprefix $(OBJ)/,$(CXXFILES:.cpp=.cpp.o))

override LDFLAGS := -lSDL2 -lm
override CXXFLAGS := -I src/ -I src/imgui
override HEADER_DEPS := $(addprefix obj/,$(CXXFILES:.cpp=.cpp.d))

.PHONY: all
all:	bin/$(OUTPUT)

$(BIN)/$(OUTPUT): $(OBJFILES)
	mkdir -p "$$(dirname $@)"
	$(LD) -o $@ $(OBJFILES) $(LDFLAGS)

-include $(HEADER_DEPS)
$(OBJ)/%.cpp.o: src/%.cpp
	mkdir -p "$$(dirname $@)"
	$(CPC) -c $< -o $@ $(CXXFLAGS)

.PHONY: clean
clean:
	rm -rf bin
