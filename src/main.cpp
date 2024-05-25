#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdint>

#include "game.hpp"

Game game;

void geodx_main_loop(void* args) {
  MainLoopArgs* a = (MainLoopArgs*)args;

  a->window_width = EM_ASM_INT(return window.innerWidth);
  a->window_height = EM_ASM_INT(return window.innerHeight);
  SDL_SetWindowSize(a->window, a->window_width, a->window_height);

  SDL_RenderClear(a->renderer);
  game.update(a);
  SDL_RenderPresent(a->renderer);
}

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS);

  SDL_Window* window=NULL;
  SDL_Renderer* renderer=NULL;

  int window_width=EM_ASM_INT(return window.innerWidth);
  int window_height=EM_ASM_INT(return window.innerHeight);
  
  SDL_CreateWindowAndRenderer(window_width, window_height, SDL_RENDERER_PRESENTVSYNC, &window, &renderer);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);


  MainLoopArgs args;

  args.window = window;
  args.renderer = renderer;
  args.window_width = window_width;
  args.window_height = window_height;

  game_setup(&args);

  emscripten_set_main_loop_arg(geodx_main_loop, (void*)&args, 0, 1); 
  
  return 0;
}
