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
          case SDLK_z: {
            Vec3 axis = transform_normal(cam->transform, { 1, 0, 0 });

            double x_rot[3][4];
            axis_angle_to_transform(x_rot, axis, 0.02f);

            transform_mul(cam->transform, x_rot, cam->transform);
            
            break;
          }
          case SDLK_s: {
            Vec3 axis = transform_normal(cam->transform, { 1, 0, 0 });

            double x_rot[3][4];
            axis_angle_to_transform(x_rot, axis, -0.02f);

            transform_mul(cam->transform, x_rot, cam->transform);
            
            break;
          }
          case SDLK_d: {
            Vec3 axis = transform_normal(cam->transform, { 0, 1, 0 });

            double y_rot[3][4];
            axis_angle_to_transform(y_rot, axis, -0.02f);

            transform_mul(cam->transform, y_rot, cam->transform);
            
            break;
          }
          case SDLK_q: {
            Vec3 axis = transform_normal(cam->transform, { 0, 1, 0 });

            double y_rot[3][4];
            axis_angle_to_transform(y_rot, axis, 0.02f);

            transform_mul(cam->transform, y_rot, cam->transform);
            
            break;
          }
          case SDLK_e: {
            Vec3 axis = transform_normal(cam->transform, { 0, 0, 1 });

            double z_rot[3][4];
            axis_angle_to_transform(z_rot, axis, -0.02f);

            transform_mul(cam->transform, z_rot, cam->transform);
            
            break;
          }
          case SDLK_a: {
            Vec3 axis = transform_normal(cam->transform, { 0, 0, 1 });

            double z_rot[3][4];
            axis_angle_to_transform(z_rot, axis, 0.02f);

            transform_mul(cam->transform, z_rot, cam->transform);

            break;
          }
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
  int32_t window_width;
  int32_t window_height;

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
