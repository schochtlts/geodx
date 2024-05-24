
# $make build

build:
	emcc src/main.cpp -o build/main.js -O3 -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=[ccall] -s USE_SDL=2
	read -p "Press any key to execute..."
	emrun build/index.html
