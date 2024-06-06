
# $make build

build:
	emcc src/main.cpp -o main.js -O3 -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=[ccall] -s USE_SDL=2
