#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdint>

#include "game.hpp"

Game game;

int main(int argc, char** argv) {

  game.setup();
  
  return 0;
}
