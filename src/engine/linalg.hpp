
#pragma once

#include <cstdint>
#include <cmath>
#include <initializer_list>
#include <cstdlib>
#include <iostream>

class Vec3
{
public:
{
public:
  double mat[3][4];

  Mat3x4(std::initializer_list<std::initializer_list<double>> m = {{0,0,0,0},{0,0,0,0},{0,0,0,0}}) {
    if(m.size() != 3) {
      std::cerr << "Error in Mat3x4 constructor: 3 rows are required!";
      exit(1);
    }
    double* ptr = (double*)mat;
    for(const auto& row : m) {
      if(row.size() != 4) {
        std::cerr << "Error in Mat3x4 constructor: 4 columns are required!";
        exit(1);
      }
      for(const auto& element : row) {
        *ptr = element;
        ptr++;
      }
    }
  }

  ~Mat3x4() {}

  double operator()(size_t x, size_t y) const { return mat[y][x]; };
  double& operator()(size_t x, size_t y) { return mat[y][x]; };
};

#define IDENT_MAT3x4 Mat3x4({ { 1., 0., 0., 0. }, { 0., 1., 0., 0. }, { 0., 0., 1., 0. } })

Mat3x4 operator+(Mat3x4 A, Mat3x4 B) {
  return Mat3x4({ { A(0,0) + B(0,0), A(1,0) + B(1,0), A(2,0) + B(2,0), A(3,0) + B(3,0) },
                  { A(0,1) + B(0,1), A(1,1) + B(1,1), A(2,1) + B(2,1), A(3,1) + B(3,1) },
                  { A(0,2) + B(0,2), A(1,2) + B(1,2), A(2,2) + B(2,2), A(3,2) + B(3,2) } });
}

Mat3x4 operator*(double s, Mat3x4 m) {
  return Mat3x4({ { s*m(0,0), s*m(1,0), s*m(2,0), s*m(3,0) },
                  { s*m(0,1), s*m(1,1), s*m(2,1), s*m(3,1) },
                  { s*m(0,2), s*m(1,2), s*m(2,2), s*m(3,2) } });
}

Mat3x4 operator*(Mat3x4 m1, Mat3x4 m2) {
  Mat3x4 res;

  res(0,0) = m1(0,0)*m2(0,0) + m1(1,0)*m2(0,1) + m1(2,0)*m2(0,2);
  res(1,0) = m1(0,0)*m2(1,0) + m1(1,0)*m2(1,1) + m1(2,0)*m2(1,2);
  res(2,0) = m1(0,0)*m2(2,0) + m1(1,0)*m2(2,1) + m1(2,0)*m2(2,2);
  res(3,0) = m1(0,0)*m2(3,0) + m1(1,0)*m2(3,1) + m1(2,0)*m2(3,2) + m1(3,0);

  res(0,1) = m1(0,1)*m2(0,0) + m1(1,1)*m2(0,1) + m1(2,1)*m2(0,2);
  res(1,1) = m1(0,1)*m2(1,0) + m1(1,1)*m2(1,1) + m1(2,1)*m2(1,2);
  res(2,1) = m1(0,1)*m2(2,0) + m1(1,1)*m2(2,1) + m1(2,1)*m2(2,2);
  res(3,1) = m1(0,1)*m2(3,0) + m1(1,1)*m2(3,1) + m1(2,1)*m2(3,2) + m1(3,1);

  res(0,2) = m1(0,2)*m2(0,0) + m1(1,2)*m2(0,1) + m1(2,2)*m2(0,2);
  res(1,2) = m1(0,2)*m2(1,0) + m1(1,2)*m2(1,1) + m1(2,2)*m2(1,2);
  res(2,2) = m1(0,2)*m2(2,0) + m1(1,2)*m2(2,1) + m1(2,2)*m2(2,2);
  res(3,2) = m1(0,2)*m2(3,0) + m1(1,2)*m2(3,1) + m1(2,2)*m2(3,2) + m1(3,2);

  return res;
}

Mat3x4 axis_angle_to_mat(Vec3 axis, double angle) {
  const double cos = std::cos(angle), sin = std::sin(angle);
  const Mat3x4 K = Mat3x4({ { 0.0f, -axis.z, axis.y, 0.0f },
                            { axis.z, 0.0f, -axis.x, 0.0f },
                            { -axis.y, axis.x, 0.0f, 0.0f } });

  return IDENT_MAT3x4 + sin*K + (1.0f - cos)*(K*K);
}

void Vec3::setPosFromArray(double pos[3]){
  _x=pos[0];
  _y=pos[1];
  _z=pos[2];
}
