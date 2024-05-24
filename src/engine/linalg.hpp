#ifndef _LINALG_H_
#define _LINALG_H_

#include <cstdint>
#include <cmath>

typedef struct {
  double x;
  double y;
  double z;
}
Vec3;

Vec3 add(Vec3 v1, Vec3 v2) {
  return (Vec3){
    v1.x + v2.x,
    v1.y + v2.y,
    v1.z + v2.z
  };
}

Vec3 middle(Vec3 p1, Vec3 p2) {
  return (Vec3){
    (p1.x + p2.x) / 2.0f,
    (p1.y + p2.y) / 2.0f,
    (p1.z + p2.z) / 2.0f
  };
}

Vec3 set_norm(Vec3 v, double norm) {
  const double n = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  return (Vec3){
    norm * v.x / n,
    norm * v.y / n,
    norm * v.z / n
  };
}

Vec3 rotate_x(Vec3 v, double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  return (Vec3){
    v.x,
    cos * v.y - sin * v.z,
    sin * v.y + cos * v.z
  };
}

Vec3 rotate_y(Vec3 v, double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  return (Vec3){
    cos * v.x + sin * v.z,
    v.y,
    -sin * v.x + cos * v.z
  };
}

Vec3 rotate_z(Vec3 v, double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  return (Vec3){
    cos * v.x - sin * v.y,
    sin * v.x + cos * v.y,
    v.z
  };
}

Vec3 mul(double scalar, Vec3 v) {
  return (Vec3){
    scalar*v.x,
    scalar*v.y,
    scalar*v.z
  };
}

#endif
