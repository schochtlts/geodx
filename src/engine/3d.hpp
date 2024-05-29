#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <cmath>
#include <vector>

#include "linalg.hpp"

typedef struct {
  Vec3 pos;
  SDL_Color color;
}
Vertex;

typedef struct {
  Vertex verts[3];
  Vec3 normal;
}
Triangle;

typedef struct {
  std::vector<Triangle> triangles;
}
Mesh;

typedef struct {
  Mat3x4 transform;
  Mesh* mesh;
}
Object;

typedef struct {
  Mat3x4 transform;
  double fov;
}
Camera;

Mesh generate_icosphere(double radius, uint8_t sub_level) {
  const double GOLDEN_RATIO = (std::sqrt(5) + 1.0f) / 2.0f;
  
  const double SIDE_LEN = radius * (4.0f / std::sqrt(10.0f + 2 * std::sqrt(5)));
  const double SHORT = SIDE_LEN / 2.0f;
  const double LONG = SIDE_LEN * GOLDEN_RATIO / 2.0f;

  const Vertex verts[3][4];
  verts[0][0].pos.setPosFromArray({0,SHORT, -LONG});
  verts[0][0].pos.color=0;
  verts[0][0].pos.setPosFromArray({0,SHORT, -LONG});
  verts[0][0].pos.color=0;
    { { { 0, SHORT, -LONG  }, { 0 } },
      { { 0, SHORT, LONG   }, { 0 } },
      { { 0, -SHORT, -LONG }, { 0 } },
      { { 0, -SHORT, LONG  }, { 0 } } },
    { { { LONG, 0, -SHORT  }, { 0 } },
      { { LONG, 0, SHORT   }, { 0 } },
      { { -LONG, 0, -SHORT }, { 0 } },
      { { -LONG, 0, SHORT  }, { 0 } } },
    { { { SHORT, -LONG, 0  }, { 0 } },
      { { SHORT, LONG, 0   }, { 0 } },
      { { -SHORT, -LONG, 0 }, { 0 } },
      { { -SHORT, LONG, 0  }, { 0 } } }
  };

  auto triangles = std::vector{
    (Triangle){ { verts[0][1], verts[2][3], verts[2][1] }, Vec3() },
    (Triangle){ { verts[0][1], verts[2][1], verts[1][1] }, Vec3() },
    (Triangle){ { verts[0][1], verts[1][1], verts[0][3] }, Vec3() },
    (Triangle){ { verts[0][1], verts[0][3], verts[1][3] }, Vec3() },
    (Triangle){ { verts[0][1], verts[1][3], verts[2][3] }, Vec3() },
    (Triangle){ { verts[0][0], verts[2][1], verts[2][3] }, Vec3() },
    (Triangle){ { verts[0][0], verts[2][3], verts[1][2] }, Vec3() },
    (Triangle){ { verts[0][0], verts[1][2], verts[0][2] }, Vec3() },
    (Triangle){ { verts[0][0], verts[0][2], verts[1][0] }, Vec3() },
    (Triangle){ { verts[0][0], verts[1][0], verts[2][1] }, Vec3() },
    (Triangle){ { verts[1][0], verts[1][1], verts[2][1] }, Vec3() },
    (Triangle){ { verts[1][0], verts[2][0], verts[1][1] }, Vec3() },
    (Triangle){ { verts[1][3], verts[1][2], verts[2][3] }, Vec3() },
    (Triangle){ { verts[1][3], verts[2][2], verts[1][2] }, Vec3() },
    (Triangle){ { verts[0][3], verts[1][1], verts[2][0] }, Vec3() },
    (Triangle){ { verts[0][3], verts[2][0], verts[2][2] }, Vec3() },
    (Triangle){ { verts[0][3], verts[2][2], verts[1][3] }, Vec3() },
    (Triangle){ { verts[0][2], verts[2][0], verts[1][0] }, Vec3() },
    (Triangle){ { verts[0][2], verts[2][2], verts[2][0] }, Vec3() },
    (Triangle){ { verts[0][2], verts[1][2], verts[2][2] }, Vec3() }
  };

  for(size_t i = 0; i < sub_level; i++) {
    size_t vec_sz = triangles.size();
    for(size_t j = 0; j < vec_sz; j++) {
      Triangle t = triangles[0];
      triangles.erase(triangles.begin());

      const Vertex verts[] = { { radius*middle(t.verts[0].pos, t.verts[1].pos).normalized(), { 0 } },
                               { radius*middle(t.verts[1].pos, t.verts[2].pos).normalized(), { 0 } },
                               { radius*middle(t.verts[2].pos, t.verts[0].pos).normalized(), { 0 } }
      };

      triangles.push_back((Triangle){ { verts[2],   verts[1],   t.verts[2] }, { 0 } });
      triangles.push_back((Triangle){ { t.verts[0], verts[0],   verts[2]   }, { 0 } });
      triangles.push_back((Triangle){ { verts[0],   verts[1],   verts[2]   }, { 0 } });
      triangles.push_back((Triangle){ { verts[0],   t.verts[1], verts[1]   }, { 0 } });
    }
  }

  return (Mesh){ triangles };
}

void compute_normals(Mesh* mesh) {
  for(size_t i = 0; i < mesh->triangles.size(); i++) {
    const Triangle t = mesh->triangles[i];
    
    const Vec3 v1 = t.verts[2].pos - t.verts[0].pos;
    const Vec3 v2 = t.verts[1].pos - t.verts[0].pos;

    Vec3 n = Vec3(
      v1.y * v2.z - v1.z * v2.y,
      v1.z * v2.x - v1.x * v2.z,
      v1.x * v2.y - v1.y * v2.x ).normalized();

    mesh->triangles[i].normal = n;
  }
}

void rotate_mesh(Mesh* mesh, double angle, uint8_t axis) {
  for(size_t i = 0; i < mesh->triangles.size(); i++) {
    const Triangle t = mesh->triangles[i];
    Vec3 rotated;

    for(size_t j = 0; j < 3; j++) {
      const Vec3 p = t.verts[j].pos;

      switch(axis) {
        case 0:
          rotated = p.rotated_x(angle);
          break;
        case 1:
          rotated = p.rotated_y(angle);
          break;
        case 2:
          rotated = p.rotated_z(angle);
          break;
      }

      mesh->triangles[i].verts[j].pos = rotated;
    }

    switch(axis) {
      case 0:
        rotated = t.normal.rotated_x(angle);
        break;
      case 1:
        rotated = t.normal.rotated_y(angle);
        break;
      case 2:
        rotated = t.normal.rotated_z(angle);
        break;
    }

    mesh->triangles[i].normal = rotated;
  }
}

Vec3 transform_vec(Mat3x4 m, Vec3 v) {
  return Vec3( m(0,0)*v.x + m(1,0)*v.y + m(2,0)*v.z + m(3,0),
               m(0,1)*v.x + m(1,1)*v.y + m(2,1)*v.z + m(3,1),
               m(0,2)*v.x + m(1,2)*v.y + m(2,2)*v.z + m(3,2) );
}

Vec3 transform_normal(Mat3x4 m, Vec3 n) {
  // scale and translation are ignored
  const double scale = 1.0f / std::sqrt(m(0,0)*m(0,0) + m(1,0)*m(1,0) + m(2,0)*m(2,0));
  return scale*Vec3( m(0,0)*n.x + m(1,0)*n.y + m(2,0)*n.z,
                     m(0,1)*n.x + m(1,1)*n.y + m(2,1)*n.z,
                     m(0,2)*n.x + m(1,2)*n.y + m(2,2)*n.z );
}

void draw_object(SDL_Renderer* renderer, Camera* cam, Object* obj, double win_width, double win_height) {
  const double HF_W = win_width / 2.0f;
  const double HF_H = win_height / 2.0f;
  const double D = HF_W / std::tan(cam->fov / 2.0f);

  const Mat3x4 m = cam->transform;
  const Mat3x4 view_mat = Mat3x4({
      { m(0,0), m(0,1), m(0,2), -(m(0,0)*m(3,0) + m(0,1)*m(3,1) + m(0,2)*m(3,2)) },
      { m(1,0), m(1,1), m(1,2), -(m(1,0)*m(3,0) + m(1,1)*m(3,1) + m(1,2)*m(3,2)) },
      { m(2,0), m(2,1), m(2,2), -(m(2,0)*m(3,0) + m(2,1)*m(3,1) + m(2,2)*m(3,2)) } });

  const Mat3x4 to_cam_space = view_mat*(obj->transform);

  for(size_t i = 0; i < obj->mesh->triangles.size(); i++) {
    const Triangle t = obj->mesh->triangles[i];
    
    const Vec3 n = transform_normal(to_cam_space, t.normal);
    const Vec3 p0 = transform_vec(to_cam_space, t.verts[0].pos);
    //if(p0.z < 0) continue;

    if(dot(p0 + Vec3( 0, 0, D ), n) > 0) continue;

    const Vec3 p1 = transform_vec(to_cam_space, t.verts[1].pos);
    //if(p1.z < 0) continue;
    const Vec3 p2 = transform_vec(to_cam_space, t.verts[2].pos);
    //if(p2.z < 0) continue;

    const double Z0 = D / (D + p0.z);
    const double Z1 = D / (D + p1.z);
    const double Z2 = D / (D + p2.z);

    const SDL_Vertex verts_2d[3] = {
      { (SDL_FPoint){ (float)(HF_W + Z0*p0.x), (float)(HF_H - Z0*p0.y) }, t.verts[0].color, (SDL_FPoint){ 0 } },
      { (SDL_FPoint){ (float)(HF_W + Z1*p1.x), (float)(HF_H - Z1*p1.y) }, t.verts[1].color, (SDL_FPoint){ 0 } },
      { (SDL_FPoint){ (float)(HF_W + Z2*p2.x), (float)(HF_H - Z2*p2.y) }, t.verts[2].color, (SDL_FPoint){ 0 } }
    };

    SDL_RenderGeometry(renderer, NULL, verts_2d, 3, NULL, 0);
  }
}
