# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 # -Wall -Wextra -Werror -pedantic

# Directories
SRCDIR = src
INCLUDES = -Iinclude
OBJDIR = obj
BINDIR = build

# Files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
EXECUTABLE = $(BINDIR)/mint.exe

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(INCLUDES) -static-libstdc++

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES) -static-libstdc++

run: $(EXECUTABLE)
	clear
	$(EXECUTABLE) ./examples/test.mint

clean:
	rm -rf $(OBJDIR)/*.o $(EXECUTABLE)

# Install

# if windows then C:\MintScript\mint.exe else /usr/local/bin/mint
DESTDIR =

ifeq ($(OS),Windows_NT)
	EXECUTABLE = $(BINDIR)/mint.exe
	DESTDIR = C:\MintScript
else
	EXECUTABLE = $(BINDIR)/mint
	DESTDIR = /usr/local/bin/mint
endif

install: $(EXECUTABLE)
	mkdir -p $(DESTDIR)
	cp $(EXECUTABLE) $(DESTDIR)

# TODO: Test