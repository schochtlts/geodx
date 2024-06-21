#pragma once

#include <cstdint>
#include <cmath>
#include <limits>

#include "linalg.h"

// Thanks Thomas Wang for this beautiful function, it works very well
// https://burtleburtle.net/bob/hash/integer.html
double hash(Vec3 pos, uint32_t seed) {
    uint32_t a = ((uint32_t)std::abs(pos.z) << 24) + ((uint32_t)std::abs(pos.y) << 16) + ((uint32_t)std::abs(pos.x) << 8) + seed;
    
    a -= a << 6;
    a ^= a >> 17;
    a -= a << 9;
    a ^= a << 4;
    a -= a << 3;
    a ^= a << 10;
    a ^= a >> 15;
    
    return (double)a / std::numeric_limits<uint32_t>::max();
}

double fade(double x) {
  return x*x*x*(x*(x*6.0f - 15.0f) + 10.0f);
}

double simplex_noise(Vec3 pos, uint32_t seed) {
  Vec3 grid_pos;

  const Vec3 loc_pos = {
    std::modf(pos.x, &(grid_pos.x)),
    std::modf(pos.y, &(grid_pos.y)),
    std::modf(pos.z, &(grid_pos.z))
  };

  const uint8_t tetra_index = ((loc_pos.z > loc_pos.y) << 2) +
                              ((loc_pos.z > loc_pos.x) << 1) +
                              (loc_pos.y > loc_pos.x);

  const double hash0 = hash(grid_pos, seed);
  const double hash3 = hash(grid_pos + Vec3( 1.0f, 1.0f, 1.0f ), seed);
  double hash1, hash2;

  // baryocentric coordinates
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
    }
    case 1: {
      hash1 = hash(grid_pos + Vec3( 0.0f, 1.0f, 0.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 1.0f, 1.0f, 0.0f ), seed);

      a = 1.0f - loc_pos.y;
      b = loc_pos.y - loc_pos.x;
      c = loc_pos.x - loc_pos.z;
      d = loc_pos.z;
    }
    case 3: {
      hash1 = hash(grid_pos + Vec3( 0.0f, 1.0f, 0.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 0.0f, 1.0f, 1.0f ), seed);

      a = 1.0f - loc_pos.y;
      b = loc_pos.y - loc_pos.z;
      c = loc_pos.z - loc_pos.x;
      d = loc_pos.x;
    }
    case 4: {
      hash1 = hash(grid_pos + Vec3( 1.0f, 0.0f, 0.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 1.0f, 0.0f, 1.0f ), seed);

      a = 1.0f - loc_pos.x;
      b = loc_pos.x - loc_pos.z;
      c = loc_pos.z - loc_pos.y;
      d = loc_pos.y;
    }
    case 6: {
      hash1 = hash(grid_pos + Vec3( 0.0f, 0.0f, 1.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 1.0f, 0.0f, 1.0f ), seed);

      a = 1.0f - loc_pos.z;
      b = loc_pos.z - loc_pos.x;
      c = loc_pos.x - loc_pos.y;
      d = loc_pos.y;
    }
    case 7: {
      hash1 = hash(grid_pos + Vec3( 0.0f, 0.0f, 1.0f ), seed);
      hash2 = hash(grid_pos + Vec3( 0.0f, 1.0f, 1.0f ), seed);

      a = 1.0f - loc_pos.z;
      b = loc_pos.z - loc_pos.y;
      c = loc_pos.y - loc_pos.x;
      d = loc_pos.x;
    }
  }

  return fade(hash0*a + hash1*b + hash2*c + hash3*d);
}
