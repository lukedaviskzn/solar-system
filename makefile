CXX=g++
CXXFLAGS= -c `sdl2-config --cflags` -std=c++14 -g
INCLUDES= -Iinclude
LFLAGS= `sdl2-config --libs` -lGLEW -lGL
BINDIR=bin
SRCDIR=src
SRCFILES=$(wildcard $(SRCDIR)/*.cpp)
OBJFILES=$(patsubst $(SRCDIR)/%,$(BINDIR)/%,$(SRCFILES:.cpp=.o))
TARGET=$(BINDIR)/solar_system

build: $(OBJFILES) $(TARGET)

$(TARGET): $(OBJFILES)
	$(CXX) $(OBJFILES) -o $(TARGET) $(LFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(INCLUDES) $(CXXFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(OBJFILES)
