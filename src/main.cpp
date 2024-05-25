#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdint>

#include "game.hpp"

int main(int argc, char* argv[]) {

  Game game;
  game.setup();
  
  return 0;
}
