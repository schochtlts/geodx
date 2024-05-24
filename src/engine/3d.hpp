#ifndef _3D_H_
#define _3D_H_

#include <SDL2/SDL.h>
#include <cstdint>
#include <cmath>
#include <vector>

#include "linalg.hpp"

#define IDENT_TRANSFORM { { 1., 0., 0., 0. }, { 0., 1., 0., 0. }, { 0., 0., 1., 0. } }

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
  double transform[3][4];
  Mesh* mesh;
}
Object;

typedef struct {
  double transform[3][4];
  double fov;
}
Camera;

Mesh generate_icosphere(double radius, uint8_t sub_level) {
  const double GOLDEN_RATIO = (std::sqrt(5) + 1.0f) / 2.0f;
  
  const double SIDE_LEN = radius * (4.0f / std::sqrt(10.0f + 2 * std::sqrt(5)));
  const double SHORT = SIDE_LEN / 2.0f;
  const double LONG = SIDE_LEN * GOLDEN_RATIO / 2.0f;

  const Vertex verts[3][4] = {
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
    (Triangle){ { verts[0][1], verts[2][3], verts[2][1] }, { 0 } },
    (Triangle){ { verts[0][1], verts[2][1], verts[1][1] }, { 0 } },
    (Triangle){ { verts[0][1], verts[1][1], verts[0][3] }, { 0 } },
    (Triangle){ { verts[0][1], verts[0][3], verts[1][3] }, { 0 } },
    (Triangle){ { verts[0][1], verts[1][3], verts[2][3] }, { 0 } },
    (Triangle){ { verts[0][0], verts[2][1], verts[2][3] }, { 0 } },
    (Triangle){ { verts[0][0], verts[2][3], verts[1][2] }, { 0 } },
    (Triangle){ { verts[0][0], verts[1][2], verts[0][2] }, { 0 } },
    (Triangle){ { verts[0][0], verts[0][2], verts[1][0] }, { 0 } },
    (Triangle){ { verts[0][0], verts[1][0], verts[2][1] }, { 0 } },
    (Triangle){ { verts[1][0], verts[1][1], verts[2][1] }, { 0 } },
    (Triangle){ { verts[1][0], verts[2][0], verts[1][1] }, { 0 } },
    (Triangle){ { verts[1][3], verts[1][2], verts[2][3] }, { 0 } },
    (Triangle){ { verts[1][3], verts[2][2], verts[1][2] }, { 0 } },
    (Triangle){ { verts[0][3], verts[1][1], verts[2][0] }, { 0 } },
    (Triangle){ { verts[0][3], verts[2][0], verts[2][2] }, { 0 } },
    (Triangle){ { verts[0][3], verts[2][2], verts[1][3] }, { 0 } },
    (Triangle){ { verts[0][2], verts[2][0], verts[1][0] }, { 0 } },
    (Triangle){ { verts[0][2], verts[2][2], verts[2][0] }, { 0 } },
    (Triangle){ { verts[0][2], verts[1][2], verts[2][2] }, { 0 } }
  };

  for(size_t i = 0; i < sub_level; i++) {
    size_t vec_sz = triangles.size();
    for(size_t j = 0; j < vec_sz; j++) {
      Triangle t = triangles[0];
      triangles.erase(triangles.begin());

      const Vertex verts[] = { { set_norm(middle(t.verts[0].pos, t.verts[1].pos), radius), { 0 } },
                               { set_norm(middle(t.verts[1].pos, t.verts[2].pos), radius), { 0 } },
                               { set_norm(middle(t.verts[2].pos, t.verts[0].pos), radius), { 0 } }
      };

      triangles.push_back((Triangle){ { verts[2],    verts[1],    t.verts[2] }, { 0 } });
      triangles.push_back((Triangle){ { t.verts[0], verts[0],    verts[2]    }, { 0 } });
      triangles.push_back((Triangle){ { verts[0],    verts[1],    verts[2]    }, { 0 } });
      triangles.push_back((Triangle){ { verts[0],    t.verts[1], verts[1]    }, { 0 } });
    }
  }

  return (Mesh){ triangles };
}

void compute_normals(Mesh* mesh) {
  for(size_t i = 0; i < mesh->triangles.size(); i++) {
    const Triangle t = mesh->triangles[i];
    
    const Vec3 v1 = {
      t.verts[2].pos.x - t.verts[0].pos.x,
      t.verts[2].pos.y - t.verts[0].pos.y,
      t.verts[2].pos.z - t.verts[0].pos.z
    };

    const Vec3 v2 = {
      t.verts[1].pos.x - t.verts[0].pos.x,
      t.verts[1].pos.y - t.verts[0].pos.y,
      t.verts[1].pos.z - t.verts[0].pos.z
    };

    Vec3 n = {
      v1.y * v2.z - v1.z * v2.y,
      v1.z * v2.x - v1.x * v2.z,
      v1.x * v2.y - v1.y * v2.x
    };

    const double len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    n.x /= len; n.y /= len; n.z /= len;

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
          rotated = rotate_x(p, angle);
          break;
        case 1:
          rotated = rotate_y(p, angle);
          break;
        case 2:
          rotated = rotate_z(p, angle);
          break;
      }

      mesh->triangles[i].verts[j].pos = rotated;
    }

    switch(axis) {
      case 0:
        rotated = rotate_x(t.normal, angle);
        break;
      case 1:
        rotated = rotate_y(t.normal, angle);
        break;
      case 2:
        rotated = rotate_z(t.normal, angle);
        break;
    }

    mesh->triangles[i].normal = rotated;
  }
}

void scalar_transform_mul(double dest[3][4], double scalar, double m[3][4]) {
  dest[0][0] = scalar * m[0][0];
  dest[0][1] = scalar * m[0][1];
  dest[0][2] = scalar * m[0][2];
  dest[0][3] = scalar * m[0][3];

  dest[1][0] = scalar * m[1][0];
  dest[1][1] = scalar * m[1][1];
  dest[1][2] = scalar * m[1][2];
  dest[1][3] = scalar * m[1][3];

  dest[2][0] = scalar * m[2][0];
  dest[2][1] = scalar * m[2][1];
  dest[2][2] = scalar * m[2][2];
  dest[2][3] = scalar * m[2][3];
}

void transform_add(double dest[3][4], double m1[3][4], double m2[3][4]) {
  dest[0][0] = m1[0][0] + m2[0][0];
  dest[0][1] = m1[0][1] + m2[0][1];
  dest[0][2] = m1[0][2] + m2[0][2];
  dest[0][3] = m1[0][3] + m2[0][3];

  dest[1][0] = m1[1][0] + m2[1][0];
  dest[1][1] = m1[1][1] + m2[1][1];
  dest[1][2] = m1[1][2] + m2[1][2];
  dest[1][3] = m1[1][3] + m2[1][3];

  dest[2][0] = m1[2][0] + m2[2][0];
  dest[2][1] = m1[2][1] + m2[2][1];
  dest[2][2] = m1[2][2] + m2[2][2];
  dest[2][3] = m1[2][3] + m2[2][3];
}

void transform_mul(double dest[3][4], double m1[3][4], double m2[3][4]) {
  double temp[3][4];

  temp[0][0] = m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0] + m1[0][2]*m2[2][0];
  temp[0][1] = m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1] + m1[0][2]*m2[2][1];
  temp[0][2] = m1[0][0]*m2[0][2] + m1[0][1]*m2[1][2] + m1[0][2]*m2[2][2];
  temp[0][3] = m1[0][0]*m2[0][3] + m1[0][1]*m2[1][3] + m1[0][2]*m2[2][3] + m1[0][3];

  temp[1][0] = m1[1][0]*m2[0][0] + m1[1][1]*m2[1][0] + m1[1][2]*m2[2][0];
  temp[1][1] = m1[1][0]*m2[0][1] + m1[1][1]*m2[1][1] + m1[1][2]*m2[2][1];
  temp[1][2] = m1[1][0]*m2[0][2] + m1[1][1]*m2[1][2] + m1[1][2]*m2[2][2];
  temp[1][3] = m1[1][0]*m2[0][3] + m1[1][1]*m2[1][3] + m1[1][2]*m2[2][3] + m1[1][3];

  temp[2][0] = m1[2][0]*m2[0][0] + m1[2][1]*m2[1][0] + m1[2][2]*m2[2][0];
  temp[2][1] = m1[2][0]*m2[0][1] + m1[2][1]*m2[1][1] + m1[2][2]*m2[2][1];
  temp[2][2] = m1[2][0]*m2[0][2] + m1[2][1]*m2[1][2] + m1[2][2]*m2[2][2];
  temp[2][3] = m1[2][0]*m2[0][3] + m1[2][1]*m2[1][3] + m1[2][2]*m2[2][3] + m1[2][3];

  memcpy(dest, temp, 12*sizeof(double));
}

Vec3 transform_vec(double m[3][4], Vec3 v) {
  return (Vec3){ m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3],
                 m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3],
                 m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]
  };
}

Vec3 transform_normal(double m[3][4], Vec3 n) {
  // scale and translation are ignored
  const double scale = 1.0f / std::sqrt(m[0][0]*m[0][0] + m[0][1]*m[0][1] + m[0][2]*m[0][2]);
  return (Vec3){ (m[0][0]*n.x + m[0][1]*n.y + m[0][2]*n.z) * scale,
                 (m[1][0]*n.x + m[1][1]*n.y + m[1][2]*n.z) * scale,
                 (m[2][0]*n.x + m[2][1]*n.y + m[2][2]*n.z) * scale
  };
}

void draw_object(SDL_Renderer* renderer, Camera* cam, Object* obj, double win_width, double win_height) {
  const double HF_W = win_width / 2.0f;
  const double HF_H = win_height / 2.0f;
  const double D = HF_W / std::tan(cam->fov / 2);

  const auto m = cam->transform;
  double view_mat[3][4] = { { m[0][0], m[1][0], m[2][0], -(m[0][0]*m[0][3] + m[1][0]*m[1][3] + m[2][0]*m[2][3]) },
                            { m[0][1], m[1][1], m[2][1], -(m[0][1]*m[0][3] + m[1][1]*m[1][3] + m[2][1]*m[2][3]) },
                            { m[0][2], m[1][2], m[2][2], -(m[0][2]*m[0][3] + m[1][2]*m[1][3] + m[2][2]*m[2][3]) } };

  double to_cam_space[3][4];
  transform_mul(to_cam_space, view_mat, obj->transform);

  for(size_t i = 0; i < obj->mesh->triangles.size(); i++) {
    const Triangle t = obj->mesh->triangles[i];
    
    const Vec3 n = transform_normal(to_cam_space, t.normal);
    const Vec3 p0 = transform_vec(to_cam_space, t.verts[0].pos);
    //if(p0.z < 0) continue;

    if(p0.x*n.x + p0.y*n.y + p0.z*n.z > 0) continue;    

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

void axis_angle_to_transform(double rot_mat[3][4], Vec3 axis, double angle) {
  double K[3][4] = { { 0.0f, -axis.z, axis.y, 0.0f },
                     { axis.z, 0.0f, -axis.x, 0.0f },
                     { -axis.y, axis.x, 0.0f, 0.0f } };

  double K_sq[3][4];
  transform_mul(K_sq, K, K);

  scalar_transform_mul(K, std::sin(angle), K);
  scalar_transform_mul(K_sq, 1.0f - std::cos(angle), K_sq);

  double R[3][4] = IDENT_TRANSFORM;
  transform_add(R, R, K);
  transform_add(R, R, K_sq);

  memcpy(rot_mat, R, 12*sizeof(double));
}

#endif
