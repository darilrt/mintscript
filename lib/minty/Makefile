CXX=g++
INCLUDES=-I../../include -I/C:/vclib/SDL2/include
LIBS=-L../../build -L/C:/vclib/SDL2/lib/x64 -lmint -lSDL2main -lSDL2

all: libminty.dll

libminty.dll: minty.cpp

minty.cpp: 
	$(CXX) -shared -o ../../build/libminty.dll src/minty.cpp $(INCLUDES) $(LIBS)