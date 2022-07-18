
CXX = g++
CXXFLAGS = -g -Wall 
LDFLAGS = -lallegro -lallegro_main \
    -lallegro_color -lallegro_font -lallegro_primitives -lallegro_image

all: centipede

principal.o: principal.cpp geom.hpp
tela.o: tela.cpp tela.hpp geom.hpp

centipede: principal.o tela.o 
	$(CXX) $(CXXFLAGS) -o $@  $^ $(LDFLAGS)

clean:
	rm -f centipede *.o
