# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 # -Wall -Wextra -Werror -pedantic

# Directories
SRCDIR = src
INCLUDES = -Iinclude
OBJDIR = obj
BINDIR = build
DESTDIR =

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

ifeq ($(OS),Windows_NT)
	EXECUTABLE = $(BINDIR)/mint.exe
	OUTDLL = $(BINDIR)/mint.dll
	DESTDIR = C:\MintScript
else
	EXECUTABLE = $(BINDIR)/mint
	OUTDLL = $(BINDIR)/libmint.so
	DESTDIR = /usr/local/bin
endif

# Targets
all: $(OUTDLL) $(EXECUTABLE)

$(EXECUTABLE):
	$(CXX) $(CXXFLAGS) src/main.cpp -o $@ $(INCLUDES) -static-libstdc++ -L$(BINDIR) -lmint

$(OUTDLL): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -shared -o $@ $(INCLUDES) -static-libstdc++ -Wl,--out-implib,$(BINDIR)/libmint.a

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -DMINT_EXPORTS -c $< -o $@ $(INCLUDES) -static-libstdc++

run: $(OUTDLL) $(EXECUTABLE)
	clear
	$(EXECUTABLE) ./examples/test.mint

clean:
	rm -rf $(OBJDIR)/*.o $(EXECUTABLE)

install: $(EXECUTABLE)
	mkdir -p $(DESTDIR)
	cp $(EXECUTABLE) $(DESTDIR)

# TODO: Test