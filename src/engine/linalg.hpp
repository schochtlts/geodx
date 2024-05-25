
#pragma once

#include <cstdint>
#include <cmath>

class Vec3
{
private:
  double _x;
  double _y;
  double _z;
public:
  void Vec3(double x=0, double y=0, double z=0);//default values
  Vec3 operator+(Vec3 A, Vec3 B);
  Vec3 middle(Vec3 A, Vec3 B);
  Vec3 set_norm(Vec3 v, double norm);
  Vec3 rotate_x(Vec3 v, double angle);
  Vec3 rotate_y(Vec3 v, double angle);
  Vec3 rotate_z(Vec3 v, double angle);
  Vec3 mul(double scalar, Vec3 v);
};

void Vec3::Vec3(double x=0, double y=0, double z=0)
{
  _x=x;
  _y=y;
  _z=z;
}

Vec3 Vec3::operator+(Vec3 A, Vec3 B){
  Vec3 C;
  C._x=A._x+B._x
  C._y=A._y+B._y
  C._z=A._z+B._z
  return C;
}

Vec3 Vec3::middle(Vec3 A, Vec3 B) {
  Vec3 C;
  C._x=(A._x+B._x)/2.0f;
  C._y=(A._y+B._y)/2.0f;
  C._z=(A._z+B._z)/2.0f;
  return C;
}

Vec3 Vec3::set_norm(Vec3 v, double norm) {
  const double n = std::sqrt(v._x * v._x + v._y * v._y + v._z * v._z);
  Vec3 C;
  C._x=norm*v._x/n;
  C._y=norm*v._y/n;
  C._z=norm*v._z/n;
  return C;
}

Vec3 Vec3::rotate_x(Vec3 v, double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  Vec3 C;
  C._x=v._x;
  C._y=cos*v._y-sin*v._z;
  C._z=sin*v._y+cos*v._z;
  return C;
}

Vec3 Vec3::rotate_y(Vec3 v, double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  Vec3 C;
  C._x=cos*v._x+sin*v._z;
  C._y=v._y;
  C._z=cos*v._z-sin*v._x;
  return C;
}

Vec3 Vec3::rotate_z(Vec3 v, double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  Vec3 C;
  C._x=cos*v._x-sin*v._y;
  C._y=sin*v._x+cos*v._y;
  C._z=v._z;
  return C;
}

Vec3 Vec3::mul(double scalar, Vec3 v) {
  Vec3 C;
  C._x=scalar*v._x;
  C._y=scalar*v._y;
  C._z=scalar*v._z;
  return C;
}
