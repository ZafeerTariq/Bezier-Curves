all:
	mkdir -p build
	make build/imgui*.o
	make main
	g++ build/*.o -lsfml-graphics -lsfml-window -lsfml-system -lGL

main:
	mkdir -p build
	g++ -c main.cpp -lsfml-graphics -lsfml-window -lsfml-system
	cp *.o build/
	rm -rf *.o

build/imgui*.o: imgui/*.cpp
	mkdir -p build
	g++ -c imgui/*.cpp -lsfml-graphics -lsfml-window -lsfml-system -lGL
	cp *.o build/
	rm -rf *.o