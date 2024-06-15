
# $make build

build: src/main.cpp src/game.hpp src/engine/3d.hpp src/engine/linalg.hpp
	emcc src/main.cpp -o ../main.js -O3 -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=[ccall] -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["jpg","png"]' --preload-file textures/amoco_cadiz2.jpg 
