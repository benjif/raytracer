SRC := $(wildcard src/*.cpp)

$(shell mkdir -p build)
newrt: $(SRC)
	g++ -o build/raytracer $(SRC) `libpng-config --cflags` -lpng -Wall -g -Iinclude

clean:
	rm build/raytracer

.PHONY: all clean
