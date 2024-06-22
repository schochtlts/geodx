
#include "game.h"

Network network;
Game game;

int main(int argc, char** argv){
  network.select_role();
  network.establish_connection(); 
  game.setup();

  return 0;
}
