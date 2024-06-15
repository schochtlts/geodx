#pragma once

#include <cstdint>
#include <cmath>
#include <limits>

#include "linalg.hpp"

static const uint8_t perm[256] = {
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

double hashish(uint32_t a) {
  /*
  a -= a << 6;
  a ^= a >> 17;
  a -= a << 9;
  a ^= a << 4;
  a -= a << 3;
  a ^= a << 10;
  a ^= a >> 15;
  */

  return perm[(uint8_t)a];
}

// Thanks Thomas Wang for this beautiful function, it works very well
// https://burtleburtle.net/bob/hash/integer.html
double hash(Vec3 pos, uint32_t seed) {
    //uint32_t a = ((uint32_t)pos.z << 24) + ((uint32_t)pos.y << 16) + ((uint32_t)pos.x << 8) + seed;
    
    //const uint32_t p1 = 73856093, p2 = 19349663, p3 = 83492791;
    //uint32_t a = p1*(uint32_t)pos.x ^ p2*(uint32_t)pos.y ^ p3*(uint32_t)pos.z + seed;

    //uint32_t a = (uint32_t)pos.x + (uint32_t)pos.y + (uint32_t)pos.z;

    //uint32_t state = a * 747796405u + 2891336453u;
    //uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    //a = (word >> 22u) ^ word;

    /*
    a -= a << 6;
    a ^= a >> 17;
    a -= a << 9;
    a ^= a << 4;
    a -= a << 3;
    a ^= a << 10;
    a ^= a >> 15;
    */

    uint32_t a = hashish( (uint32_t)pos.z );
    a = hashish( a + (uint32_t)pos.y );
    a = hashish( seed + a + (uint32_t)pos.x );

    return (double)a / 255; //std::numeric_limits<uint32_t>::max();
}

double fade(double x) {
  return x*x*x*(x*(x*6.0f - 15.0f) + 10.0f);
}

double simplex_noise(Vec3 pos, uint32_t seed) {
  Vec3 grid_pos;

  pos = pos + Vec3( 1000, 1000, 1000 );

  const Vec3 loc_pos = {
    std::modf(pos.x, &(grid_pos.x)),
    std::modf(pos.y, &(grid_pos.y)),
    std::modf(pos.z, &(grid_pos.z))
  };

  const uint8_t tetra_index = ((loc_pos.z > loc_pos.y) * 4) +
                              ((loc_pos.z > loc_pos.x) * 2) +
                              (loc_pos.y > loc_pos.x);

  const double hash0 = hash(grid_pos, seed);
  const double hash3 = hash(grid_pos + Vec3( 1.0f, 1.0f, 1.0f ), seed);
  double hash1, hash2;

  // barycentric coordinates
  double a, b, c, d;

  // case 2 and 5 make no geometrical sense
  switch(tetra_index) {
    case 0: {
      hash1 = hash(grid_pos + Vec3( 1.0f, 0.0f, 0.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 1.0f, 1.0f, 0.0f ), seed);

      a = 1.0f - loc_pos.x;
      b = loc_pos.x - loc_pos.y;
      c = loc_pos.y - loc_pos.z;
      d = loc_pos.z;
    } break;
    case 1: {
      hash1 = hash(grid_pos + Vec3( 0.0f, 1.0f, 0.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 1.0f, 1.0f, 0.0f ), seed);

      a = 1.0f - loc_pos.y;
      b = loc_pos.y - loc_pos.x;
      c = loc_pos.x - loc_pos.z;
      d = loc_pos.z;
    } break;
    case 3: {
      hash1 = hash(grid_pos + Vec3( 0.0f, 1.0f, 0.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 0.0f, 1.0f, 1.0f ), seed);

      a = 1.0f - loc_pos.y;
      b = loc_pos.y - loc_pos.z;
      c = loc_pos.z - loc_pos.x;
      d = loc_pos.x;
    } break;
    case 4: {
      hash1 = hash(grid_pos + Vec3( 1.0f, 0.0f, 0.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 1.0f, 0.0f, 1.0f ), seed);

      a = 1.0f - loc_pos.x;
      b = loc_pos.x - loc_pos.z;
      c = loc_pos.z - loc_pos.y;
      d = loc_pos.y;
    } break;
    case 6: {
      hash1 = hash(grid_pos + Vec3( 0.0f, 0.0f, 1.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 1.0f, 0.0f, 1.0f ), seed);

      a = 1.0f - loc_pos.z;
      b = loc_pos.z - loc_pos.x;
      c = loc_pos.x - loc_pos.y;
      d = loc_pos.y;
    } break;
    case 7: {
      hash1 = hash(grid_pos + Vec3( 0.0f, 0.0f, 1.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 0.0f, 1.0f, 1.0f ), seed);

      a = 1.0f - loc_pos.z;
      b = loc_pos.z - loc_pos.y;
      c = loc_pos.y - loc_pos.x;
      d = loc_pos.x;
    } break;
  }

  return fade(hash0*a + hash1*b + hash2*c + hash3*d);
}
