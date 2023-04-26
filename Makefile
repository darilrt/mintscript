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
EXECUTABLE = $(BINDIR)/zen.exe

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(INCLUDES)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

run: $(EXECUTABLE)
	clear
	$(EXECUTABLE)

clean:
	rm -rf $(OBJDIR)/*.o $(EXECUTABLE)

# Tests
TESTDIR = tests

TEST_SOURCES = $(wildcard $(TESTDIR)/*.cpp) $(filter-out $(SRCDIR)/main.cpp, $(SOURCES))
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/%.o)
TEST_EXECUTABLE = $(BINDIR)/test.exe

test: $(TEST_EXECUTABLE)
	clear
	$(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) $(TEST_OBJECTS) -o $@ $(INCLUDES)

$(OBJDIR)/%.o: $(TESTDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

clean-test:
	rm -rf $(OBJDIR)/*.o $(TEST_EXECUTABLE)