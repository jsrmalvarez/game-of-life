game_of_life: game_of_life.cpp
	$(CXX) -std=c++17 game_of_life.cpp $$(pkg-config --cflags --libs sdl2) -o game_of_life
