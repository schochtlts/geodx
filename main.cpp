
#include <SDL2/SDL.h>

int main(){
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer(300, 300, 0, &window, &renderer);

  SDL_SetRenderDrawColor(renderer, /* RGBA: green */ 0x00, 0x80, 0x00, 0xFF);
  SDL_Rect rect = {.x = 10, .y = 10, .w = 150, .h = 100};
  SDL_RenderFillRect(renderer, &rect);

  SDL_RenderPresent(renderer);
}
