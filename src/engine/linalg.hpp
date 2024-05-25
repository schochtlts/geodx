#pragma once

#include <cstdint>
#include <cmath>

class Vec3
{
public:
  double x;
  double y;
  double z;

  Vec3(double X = 0, double Y = 0, double Z = 0) {
    x = X;
    y = Y;
    z = Z;
  };

  double norm();
  Vec3 normalized();
  Vec3 rotated_x(double);
  Vec3 rotated_y(double);
  Vec3 rotated_z(double);
};

double Vec3::norm() {
  return std::sqrt(x*x + y*y + z*z);
}

Vec3 Vec3::normalized() {
  const double n = norm();
  return Vec3(x/n, y/n, z/n);
}

Vec3 Vec3::rotated_x(double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  Vec3 C;
  C.x = x;
  C.y = cos*y - sin*z;
  C.z = sin*y + cos*z;
  return C;
}

Vec3 Vec3::rotated_y(double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  Vec3 C;
  C.x = cos*x + sin*z;
  C.y = y;
  C.z = cos*z - sin*x;
  return C;
}

Vec3 Vec3::rotated_z(double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  Vec3 C;
  C.x = cos*x - sin*y;
  C.y = sin*x + cos*y;
  C.z = z;
  return C;
}

Vec3 operator+(Vec3 A, Vec3 B) {
  return Vec3(A.x + B.x, A.y + B.y, A.z + B.z);
}

Vec3 operator*(double s, Vec3 v) {
  return Vec3(s*v.x, s*v.y, s*v.z);
}

Vec3 middle(Vec3 A, Vec3 B) {
  Vec3 C;
  C.x = (A.x + B.x)/2.0f;
  C.y = (A.y + B.y)/2.0f;
  C.z = (A.z + B.z)/2.0f;
  return C;
}

double dot(Vec3 A, Vec3 B) {
  return A.x*B.x + A.y*B.y + A.z*B.z;
}

class Mat3x4
{
public:
  double mat[3][4];

  double& operator() (size_t x, size_t y) { return mat[y][x]; };
};
