CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++98
TARGET = volcano_demo
SOURCES = src/demo_main.cpp src/VolcanoScene.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@ -lglut -lGLU -lGL

clean:
	rm -f $(TARGET)
