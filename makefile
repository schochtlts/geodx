
# $make build

build: src/main.cpp src/game.hpp src/engine/3d.hpp src/engine/linalg.hpp
	emcc src/main.cpp -o build/main.js -O3 -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=[ccall] -s USE_SDL=2
