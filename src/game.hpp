
#pragma once

#include <emscripten.h>
#include <SDL2/SDL.h>
#include <cstdint>

#include "engine/linalg.hpp"
#include "engine/noise.hpp"
#include "engine/3d.hpp"

class Game {
private:
  void controls();
  SDL_Color color(Vec3 pos);
  void color_mesh(Mesh* mesh);
  void rotate_cam(Camera* cam, Vec3 vec, double angle);

  SDL_Window* _window;
  SDL_Renderer* _renderer;
  int _window_height;
  int _window_width;

  Object _planet;
  Camera _cam;

  double _camera_speed=1.0f;
  clock_t _last_movement_time=0.0f;

public:
  void setup();
  void update();
};

void Game::rotate_cam(Camera* cam, Vec3 vec, double angle) {
  const Vec3 axis = transform_normal(cam->transform, vec);
  const clock_t currentTime=clock();
  std::cout<<currentTime<<std::endl;
  _camera_speed+=0.02f;
  _last_movement_time=currentTime;
  const Mat3x4 rot = axis_angle_to_mat(axis, angle*_camera_speed);
  cam->transform = rot*(cam->transform);
}

void Game::controls() {
  SDL_Event event;

  while(SDL_PollEvent(&event)) {
    switch(event.type) {
      case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {
          case SDLK_s: { rotate_cam(&_cam, Vec3( 1, 0, 0 ), -0.02f); break; }
          case SDLK_z: { rotate_cam(&_cam, Vec3( 1, 0, 0 ), 0.02f);  break; }
          case SDLK_d: { rotate_cam(&_cam, Vec3( 0, 1, 0 ), -0.02f); break; }
          case SDLK_q: { rotate_cam(&_cam, Vec3( 0, 1, 0 ), 0.02f);  break; }
          case SDLK_e: { rotate_cam(&_cam, Vec3( 0, 0, 1 ), -0.02f); break; }
          case SDLK_a: { rotate_cam(&_cam, Vec3( 0, 0, 1 ), 0.02f);  break; }
        }
        break;
    }
  }
}

SDL_Color Game::color(Vec3 pos) {
  double noise =  simplex_noise(0.01f*pos, 0);
  noise +=   0.5f*simplex_noise(0.02f*pos, 0);
  noise +=  0.25f*simplex_noise(0.04f*pos, 0);
  noise += 0.125f*simplex_noise(0.08f*pos, 0);
  noise /= 1.0f + 0.5f + 0.25f + 0.125f;

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

// somehow works
extern Game game;
void __update__() {
  game.update();
}

void Game::setup() {

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
  _planet = { IDENT_MAT3x4, &ico };
  _cam = { IDENT_MAT3x4, 0.87f };
  _cam.transform(3,2) = -2000;

// emscripten related
  emscripten_set_main_loop(__update__, 0, 1);
}

void Game::update() {
  _window_width = EM_ASM_INT(return window.innerWidth);
  _window_height = EM_ASM_INT(return window.innerHeight);
  SDL_SetWindowSize(_window, _window_width, _window_height);

  SDL_RenderClear(_renderer);
  controls();
  draw_object(_renderer, &_cam, &_planet, _window_width, _window_height);
  SDL_RenderPresent(_renderer);
}
