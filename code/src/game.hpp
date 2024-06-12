#pragma once

#include <emscripten.h>
#include <SDL2/SDL.h>
#include <cstdint>
#include <cmath>
#include <algorithm>

#include "engine/linalg.hpp"
#include "engine/noise.hpp"
#include "engine/3d.hpp"

typedef enum { IDLE, XY_ROT } InputState; 

class Game {
private:
  void controls();
  SDL_Color color(Vec3 pos);
  void color_mesh(Mesh* mesh);
  
  void input_state_transition(SDL_Event);
  void input_state_action();

  SDL_Window* _window;
  SDL_Renderer* _renderer;
  int _window_height;
  int _window_width;

  Object _planet;
  Camera _cam;

  InputState _state = IDLE;
  Vec3 _mouse_anchor;
  Mat3x4 _planet_anchor;

public:
  void setup();
  void update();
};

#define MOUSE_SENSIBILITY 0.005f
#define MOUSEWHEEL_SENSIBILITY 0.01f

void Game::input_state_transition(SDL_Event event) {
  if(event.type == SDL_MOUSEWHEEL) {
    // c moche mais hey
    //
    // ca garantie qu'il ait une relation lineaire
    // entre la quantite de scroll et la taille
    // apparente ajoutee a la planete
    const double dr = -MOUSEWHEEL_SENSIBILITY*event.wheel.preciseY;
    const double z = -_cam.transform(3,2);
    const double D = 0.5f / std::tan(_cam.fov / 2);
    const double dz = ( dr*(z + D)*(z + D) ) / ( 500*D + dr*(z + D) );
    
    _cam.transform(3,2) -= dz;
    if( _cam.transform(3,2) > -500 ) _cam.transform(3,2) = -500;
    if( _cam.transform(3,2) < -10000 ) _cam.transform(3,2) = -10000;
  }
  switch(_state) {
    case IDLE: {
      if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        _mouse_anchor = Vec3( event.button.x, event.button.y );
        _planet_anchor = _planet.transform;
        _state = XY_ROT;
        break;
      }
    } break;
    case XY_ROT: {
      if(event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        _state = IDLE;
        break;
      }
    } break;
    default: break;
  }
}

void Game::input_state_action() {
  switch(_state) {
    case XY_ROT: {
      int mouse_x, mouse_y;
      SDL_GetMouseState(&mouse_x, &mouse_y);

      const Vec3 v1 = Vec3( _window_width / 2 - _mouse_anchor.x, _mouse_anchor.y - _window_height / 2 );
      const Vec3 v2 = Vec3( -v1.y, v1.x ).normalized();
      const Vec3 v3 = Vec3( mouse_x - _mouse_anchor.x, _mouse_anchor.y - mouse_y );

      const double sign = dot(v1, v3) > 0 ? -1 : 1;
      const double angle1 = sign*std::acos( dot(v2, v3.normalized()) );

      const Vec3 axis = transform_normal(axis_angle_to_mat(v1.normalized(), angle1), Vec3( 0, 0, -1 ));
      const double angle2 = MOUSE_SENSIBILITY*v3.norm();

      SDL_SetRenderDrawColor(_renderer, 255, 0, 255, 255);
      SDL_RenderDrawLine(_renderer, _mouse_anchor.x, _mouse_anchor.y, mouse_x, mouse_y);
      SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);

      _planet.transform = axis_angle_to_mat(axis, angle2)*_planet_anchor;
    } break;
    default: break;
  }
}

void Game::controls() {
  SDL_Event event;

  while(SDL_PollEvent(&event))
    input_state_transition(event);

  input_state_action();
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

  _window_width = EM_ASM_INT(return window.innerWidth);
  _window_height = EM_ASM_INT(return window.innerHeight);
  SDL_CreateWindowAndRenderer(_window_width, _window_height, SDL_RENDERER_PRESENTVSYNC, &_window, &_renderer);
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
 
  // disable right click menu
  EM_ASM(document.addEventListener('contextmenu',e=>e.preventDefault()));

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
  draw_object(_renderer, &_cam, &_planet, _window_width, _window_height);
  controls();
  SDL_RenderPresent(_renderer);
}
