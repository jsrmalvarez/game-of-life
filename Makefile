game_of_life: game_of_life.cpp business_logic.cpp business_logic.hpp
	$(CXX) -std=c++17 *.cpp $$(pkg-config --cflags --libs sdl2) -o game_of_life
