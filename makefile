all:
	make main
	g++ build/*.o -lsfml-graphics -lsfml-window -lsfml-system -lGL
imgui:
	g++ -c imgui/*.cpp -lsfml-graphics -lsfml-window -lsfml-system -lGL
	cp *.o build/
	rm -rf *.o
main:
	g++ -c main.cpp -lsfml-graphics -lsfml-window -lsfml-system
	cp *.o build/
	rm -rf *.o
