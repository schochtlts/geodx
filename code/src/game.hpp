#pragma once

#include <emscripten.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "engine/linalg.hpp"
#include "engine/noise.hpp"
#include "engine/3d.hpp"

typedef enum { IDLE, XY_ROT } InputState; 

class Game {
private:
  void controls();
  SDL_Color color(Vec3);
  void color_mesh(Mesh*);
  SDL_Texture* load_texture(std::string);
  SDL_Texture* generate_texture(Mesh*, int, int);

  void input_state_transition(SDL_Event);
  void input_state_action();

  SDL_Window* _window;
  SDL_Renderer* _renderer;
  int _window_height;
  int _window_width;

  Object _planet;
  Camera _cam;
  //SDL_Texture* _texture;

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

  //return (SDL_Color){ (Uint8)(noise * 255), (Uint8)(noise * 255), (Uint8)(noise * 255) };

  if(noise > 0.6f) {
    return (SDL_Color){ 0, 128, 0, 255 };
  }
  if(noise > 0.55f) {
    return (SDL_Color){ 255, 225, 75, 255 };
  }
  return (SDL_Color){ 0, 0, 128, 255 };
}

void Game::color_mesh(Mesh* mesh) {
  for(size_t i = 0; i < mesh->triangles.size(); i++) {
    for(size_t j = 0; j < 3; j++) {  
      const Vec3 p = mesh->triangles[i].verts[j].pos;
      //mesh->triangles[i].verts[j].color = color(p);
    }
  }
}

SDL_Texture* Game::load_texture(std::string path) {
  SDL_Surface* surface = IMG_Load(path.c_str());
  if( surface == NULL ) puts("B");
    
  SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
  if( texture == NULL ) puts("C");

  SDL_FreeSurface(surface);

  return texture;
}

SDL_Texture* Game::generate_texture(Mesh* mesh, int width, int height) {
  SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
  SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 255, 0, 255, 255));
  SDL_LockSurface(surface);
  Uint32* pixels = (Uint32*)(surface->pixels);

  for(size_t i = 0; i < mesh->triangles.size(); i++) {
    const Triangle t = mesh->triangles[i];
    Vertex p1, p2, p3;

    if( std::abs(t.verts[0].uv.y - t.verts[1].uv.y) < 0.0001f ) {
      p1 = t.verts[0]; p2 = t.verts[1]; p3 = t.verts[2];
    }
    else if( std::abs(t.verts[1].uv.y - t.verts[2].uv.y) < 0.0001f ) {
      p1 = t.verts[1]; p2 = t.verts[2]; p3 = t.verts[0];
    }
    else {
      p1 = t.verts[2]; p2 = t.verts[0]; p3 = t.verts[1];
    }

    int y_diff = (int)(p3.uv.y * height) - (int)(p1.uv.y * height);
    int dy = y_diff > 0 ? 1 : -1;
    int start_y = p1.uv.y * height;

    double dx1 = (p3.uv.x - p1.uv.x) * width / std::abs( (p3.uv.y - p1.uv.y) * height );
    double dx2 = (p3.uv.x - p2.uv.x) * width / std::abs( (p3.uv.y - p1.uv.y) * height );

    Vec3 dv1 = p3.pos - p1.pos;
    dv1 = (1.0f / std::abs(y_diff + dy)) * dv1;

    Vec3 dv2 = p3.pos - p2.pos;
    dv2 = (1.0f / std::abs(y_diff + dy)) * dv2;

    Vec3 v1 = p1.pos, v2 = p2.pos;
    double x1 = p1.uv.x * width, x2 = p2.uv.x * width;

    for(int y = start_y; y != start_y + y_diff + dy; y += dy) {
      int x_diff = (int)x2 - (int)x1;
      int dx = x_diff > 0 ? 1 : -1;
      
      Vec3 dv = v2 - v1;
      dv = (1.0f / std::abs(x_diff + 2*dx)) * dv;

      Vec3 v = v1;

      for(int x = (int)x1 - dx; x != (int)x2 + 2*dx; x += dx) {
        SDL_Color c = color(v);

        if( (x >= 0 && x < width) && (y >= 0 && y < height) ) {
          pixels[y * width + x] = SDL_MapRGBA(surface->format, c.r, c.g, c.b, 255);
        }

        v = v + dv;
      }

      x1 += dx1; x2 += dx2;
      v1 = v1 + dv1; v2 = v2 + dv2;
    }
  }

  SDL_UnlockSurface(surface);
  SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
  SDL_FreeSurface(surface);

  return texture;
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
  if( !IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) ) puts("A");
 
  // disable right click menu
  EM_ASM(document.addEventListener('contextmenu',e=>e.preventDefault()));

  // game related
  static Mesh ico = generate_icosphere(500, 3);
  compute_normals(&ico);
  //color_mesh(&ico);
  _planet = { IDENT_MAT3x4, &ico, generate_texture(&ico, 256*5+128, 256*3) };
  _cam = { IDENT_MAT3x4, 0.87f };
  _cam.transform(3,2) = -2000;
  //_texture = load_texture("textures/amoco_cadiz.jpg");

  // emscripten related
  emscripten_set_main_loop(__update__, 0, 1);
}

void Game::update() {
  _window_width = EM_ASM_INT(return window.innerWidth);
  _window_height = EM_ASM_INT(return window.innerHeight);
  SDL_SetWindowSize(_window, _window_width, _window_height);

  SDL_RenderClear(_renderer);
  //SDL_Rect rect = { 0, 0, 200, 200 };
  //SDL_RenderCopy(_renderer, _texture, &rect, &rect);
  //SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
  draw_object(_renderer, &_cam, &_planet, _window_width, _window_height);
  controls();
  SDL_RenderPresent(_renderer);
}
