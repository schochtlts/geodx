
# $make build

build: main.cc game.h engine/3d.h engine/linalg.h
	emcc main.cc -o ../main.js -O3 -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=[ccall] -s USE_SDL=2
