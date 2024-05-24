#ifndef _GAME_H_
#define _GAME_H_

#include <SDL2/SDL.h>
#include <cstdint>

#include "engine/linalg.hpp"
#include "engine/noise.hpp"
#include "engine/3d.hpp"

void controls(Camera* cam) {
  SDL_Event event;

  while(SDL_PollEvent(&event))
  {
    switch(event.type) {
      case SDL_KEYDOWN:
        switch(event.key.keysym.sym) {

#define controls_KEYDOWN_case_vec1 {1,0,0}
#define controls_KEYDOWN_case_vec2 {0,1,0}
#define controls_KEYDOWN_case_vec3 {0,0,1}

#define controls_KEYDOWN_case(key, vec, angle) case key:\
  Vec3 axis = transform_normal(cam->transform, vec);\
  double rot[3][4];\
  axis_angle_to_transform(rot, axis, angle);\
  transform_mul(cam->transform, rot, cam->transform);\
  break;

          controls_KEYDOWN_case(SDLK_z, controls_KEYDOWN_case_vec1, 0.02f)
          controls_KEYDOWN_case(SDLK_s, controls_KEYDOWN_case_vec1, -0.02f)
          controls_KEYDOWN_case(SDLK_d, controls_KEYDOWN_case_vec2, -0.02f)
          controls_KEYDOWN_case(SDLK_q, controls_KEYDOWN_case_vec2, 0.02f)
          controls_KEYDOWN_case(SDLK_e, controls_KEYDOWN_case_vec3, -0.02f)
          controls_KEYDOWN_case(SDLK_a, controls_KEYDOWN_case_vec3, 0.02f)
          default:
            break;
        }
      default:
        break;
    }
  }
}

SDL_Color color(Vec3 pos) {
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

void color_mesh(Mesh* mesh) {
  for(size_t i = 0; i < mesh->triangles.size(); i++) {
    for(size_t j = 0; j < 3; j++) {  
      const Vec3 p = mesh->triangles[i].verts[j].pos;
      mesh->triangles[i].verts[j].color = color(p);
    }
  }
}

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  int window_width;
  int window_height;

  Camera* cam;
  Object* planet;
}
MainLoopArgs;

void game_setup(MainLoopArgs* a) {
  static Mesh ico = generate_icosphere(500, 4);
  compute_normals(&ico);
  color_mesh(&ico);

  static Object planet = { IDENT_TRANSFORM, &ico };
  static Camera cam = { IDENT_TRANSFORM, 0.87f };
  cam.transform[2][3] = -2000;

  a->cam = &cam;
  a->planet = &planet;
}

void game_update(MainLoopArgs* a) {
  controls(a->cam);
  draw_object(a->renderer, a->cam, a->planet, a->window_width, a->window_height);
}

#endif
