debug:
	g++ -std=c++11 -Wall -Wextra -g -Iinclude main.cpp src/*.h src/*.cpp -o Kartoffeln

release:
	g++ -std=c++11 -Wall -fexceptions -O3 -Iinclude main.cpp src/*.h src/*.cpp -o Kartoffeln

clean:
	rm -rf ./bin
