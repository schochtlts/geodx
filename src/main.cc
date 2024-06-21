
#include "game.h"

Network network;
Game game;

int main(int argc, char** argv){
  network.select_role();
  network.establish_connection();
  network.send("cc");
  game.setup();

  return 0;
}
