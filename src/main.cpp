#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdint>

#include "game.hpp"

void loop(void* args) {
  MainLoopArgs* a = (MainLoopArgs*)args;

  a->window_width = EM_ASM_INT(return window.innerWidth);
  a->window_height = EM_ASM_INT(return window.innerHeight);
  SDL_SetWindowSize(a->window, a->window_width, a->window_height);

  SDL_RenderClear(a->renderer);
  game_update(a);
  SDL_RenderPresent(a->renderer);
}

int32_t main() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

  SDL_Window* window;
  SDL_Renderer* renderer;

  int32_t window_width = EM_ASM_INT(return window.innerWidth);
  int32_t window_height = EM_ASM_INT(return window.innerHeight);
  
  SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window, &renderer);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  MainLoopArgs args;

  args.window = window;
  args.renderer = renderer;
  args.window_width = window_width;
  args.window_height = window_height;

  game_setup(&args);

  emscripten_set_main_loop_arg(loop, (void*)&args, 0, 1); 
  
  return 0;
}
