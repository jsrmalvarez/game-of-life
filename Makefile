all: game_of_life rust_game_of_life

gollib.o: business_logic.cpp  business_logic.hpp
	$(CXX) -std=c++17 -c business_logic.cpp

libgollib.a: business_logic.o
	ar rcs libgollib.a business_logic.o

game_of_life: game_of_life.cpp libgollib.a
	$(CXX) -std=c++17 *.cpp $$(pkg-config --cflags --libs sdl2 fmt) -I. -L. -lgollib -o game_of_life

rust_business_logic/target/release/librust_gol_business_logic.a: rust_business_logic/src/lib.rs
	cd rust_business_logic && cargo build --release

rust_game_of_life: game_of_life.cpp rust_business_logic/target/release/librust_gol_business_logic.a
	$(CXX) -std=c++17 game_of_life.cpp $$(pkg-config --cflags --libs sdl2 fmt) -I. -L./rust_business_logic/target/release -lrust_gol_business_logic -ldl -pthread -o rust_game_of_life
clean:
	rm -f game_of_life rust_game_of_life *.o *.a *.gch rust_business_logic/target/release/librust_gol_business_logic.a
