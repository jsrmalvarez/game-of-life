all: game_of_life

gollib.o: business_logic.cpp  business_logic.hpp
	$(CXX) -std=c++17 -c business_logic.cpp

libgollib.a: business_logic.o
	ar rcs libgollib.a business_logic.o

game_of_life: game_of_life.cpp libgollib.a
	$(CXX) -std=c++17 *.cpp $$(pkg-config --cflags --libs sdl2) -I. -L. -lgollib -o game_of_life

clean:
	rm -f game_of_life *.o *.a *.gch
