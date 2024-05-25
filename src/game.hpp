
#pragma once

#include <emscripten.h>
#include <SDL2/SDL.h>
#include <cstdint>

#include "engine/linalg.hpp"
#include "engine/noise.hpp"
#include "engine/3d.hpp"

//typedef void (*em_arg_callback_func)(void*);

class Game{
private:
  void controls();
  SDL_Color color(Vec3 pos);
  void color_mesh(Mesh* mesh);

  SDL_Window* _window;
  SDL_Renderer* _renderer;
  int _window_height;
  int _window_width;

  static Object _planet;
  static Camera _cam;

public:
  Game();
  ~Game();
  void setup();
  em_callback_func update();
};

Game::Game(){
}

Game::~Game(){
}

void Game::controls() {
  SDL_Event event;

  while(SDL_PollEvent(&event)){
    switch(event.type){
      case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {

#define controls_KEYDOWN_case_vec1 {1,0,0}
#define controls_KEYDOWN_case_vec2 {0,1,0}
#define controls_KEYDOWN_case_vec3 {0,0,1}

#define controls_KEYDOWN_case(key, vec, angle) case key:{Vec3 axis##key = transform_normal(_cam.transform, vec);double rot##key [3][4];axis_angle_to_transform(rot##key , axis##key , angle);transform_mul(_cam.transform, rot##key, _cam.transform);break;}

          controls_KEYDOWN_case(SDLK_z, controls_KEYDOWN_case_vec1, 0.02f)
          controls_KEYDOWN_case(SDLK_s, controls_KEYDOWN_case_vec1, -0.02f)
          controls_KEYDOWN_case(SDLK_d, controls_KEYDOWN_case_vec2, -0.02f)
          controls_KEYDOWN_case(SDLK_q, controls_KEYDOWN_case_vec2, 0.02f)
          controls_KEYDOWN_case(SDLK_e, controls_KEYDOWN_case_vec3, -0.02f)
          controls_KEYDOWN_case(SDLK_a, controls_KEYDOWN_case_vec3, 0.02f)
          //default:
          //  break;
        }
        break;
      //default:
      //  break;
    }
  }
}

SDL_Color Game::color(Vec3 pos) {
  double noise =  simplex_noise(mul(0.01f, pos), 0);
  noise +=   0.5f*simplex_noise(mul(0.02f, pos), 0);
  noise +=  0.25f*simplex_noise(mul(0.04f, pos), 0);
  noise += 0.125f*simplex_noise(mul(0.08f, pos), 0);
  noise /= 1.0f + 0.5f + 0.25f + 0.125f;

  SDL_Color color;

  if(noise > 0.5f) {
    return (SDL_Color){ 0, 128, 0, 255 };
  }
  if(noise > 0.3f) {
    return (SDL_Color){ 255, 225, 75, 255 };
  }
  return (SDL_Color){ 0, 0, 128, 255 };
}

void Game::color_mesh(Mesh* mesh) {
  for(size_t i = 0; i < mesh->triangles.size(); i++) {
    for(size_t j = 0; j < 3; j++) {  
      const Vec3 p = mesh->triangles[i].verts[j].pos;
      mesh->triangles[i].verts[j].color = color(p);
    }
  }
}

void Game::setup(){

// SDL related initialisations
  SDL_Init(SDL_INIT_EVERYTHING);
  _window=NULL;
  _renderer=NULL;
  _window_width=EM_ASM_INT(return window.innerWidth);
  _window_height=EM_ASM_INT(return window.innerHeight);
  SDL_CreateWindowAndRenderer(_window_width, _window_height, SDL_RENDERER_PRESENTVSYNC, &_window, &_renderer);
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);

// game related
  static Mesh ico = generate_icosphere(500, 4);
  compute_normals(&ico);
  color_mesh(&ico);
  _planet = { IDENT_TRANSFORM, &ico };
  _cam = { IDENT_TRANSFORM, 0.87f };
  _cam.transform[2][3] = -2000;

// emscripten related
  emscripten_set_main_loop(update(), 0, 1);
}

em_callback_func Game::update() {
  _window_width = EM_ASM_INT(return window.innerWidth);
  _window_height = EM_ASM_INT(return window.innerHeight);
  SDL_SetWindowSize(_window, _window_width, _window_height);

  SDL_RenderClear(_renderer);
  controls();
  draw_object(_renderer, &_cam, &_planet, _window_width, _window_height);
  SDL_RenderPresent(_renderer);
}
