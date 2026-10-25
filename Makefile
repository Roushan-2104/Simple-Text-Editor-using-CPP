# Build with: make        (requires FLTK's fltk-config on PATH)
# Run with:   ./TextEditor

CXX      ?= g++
TARGET   := TextEditor
SRC      := $(wildcard src/*.cpp)
OBJ      := $(patsubst src/%.cpp,build/%.o,$(SRC))

CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -MMD -MP -Iinclude $(shell fltk-config --cxxflags)
LDFLAGS  := $(shell fltk-config --ldstaticflags) -static -mwindows

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET) $(TARGET).exe

.PHONY: all clean

-include $(OBJ:.o=.d)
