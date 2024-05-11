
#include <SDL2/SDL.h>
#include <emscripten.h>

int main(){
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window;
  SDL_Renderer *renderer;
  int geodx_WindowWidth = EM_ASM_INT(return window.innerWidth);
  int geodx_WindowHeight = EM_ASM_INT(return window.innerHeight);
  SDL_CreateWindowAndRenderer(geodx_WindowWidth, geodx_WindowHeight, 0, &window, &renderer);

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  SDL_Rect rect = {.x = 10, .y = 10, .w = 150, .h = 100};
  SDL_RenderFillRect(renderer, &rect);

  SDL_RenderPresent(renderer);

  return 0;
}
