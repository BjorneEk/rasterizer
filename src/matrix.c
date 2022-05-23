#include "include/matrix.h"
#include "include/vector.h"
#include <util/util.h>
#include <math.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////
///        @author Gustaf Franzén :: https://github.com/BjorneEk;        ///
////////////////////////////////////////////////////////////////////////////


matrix3x3_t rot_x_matrix(float ang)
{
  return (matrix3x3_t)
  {
    .r1 = {1.0,      0.0,       0.0},
    .r2 = {0.0, cos(ang), -sin(ang)},
    .r3 = {0.0, sin(ang),  cos(ang)}
  };
}

matrix3x3_t rot_y_matrix(float ang)
{
  return (matrix3x3_t)
  {
    .r1 = { cos(ang), 0.0, sin(ang)},
    .r2 = {      0.0, 1.0,      0.0},
    .r3 = {-sin(ang), 0.0, cos(ang)}
  };
}

matrix3x3_t rot_z_matrix(float ang)
{
  return (matrix3x3_t)
  {
    .r1 = {cos(ang), -sin(ang), 0.0},
    .r2 = {sin(ang),  cos(ang), 0.0},
    .r3 = {0.0,            0.0, 1.0}
  };
}

matrix4x4_t projection_matrix(float near, float far, float fov, float ar)
{
  float fov_rad = 1.0f / tan(fov * 0.5f / 180.0f * 3.1415926535f);
  return (matrix4x4_t)
  {
    .m[0] = {ar * fov_rad, 0.0f    , 0.0f                        , 0.0f},
    .m[1] = {0.0f         , fov_rad, 0.0f                        , 0.0f},
    .m[2] = {0.0f         , 0.0f    , far / (far - near)         , 1.0f},
    .m[3] = {0.0f         , 0.0f    , (-far * near) / (far- near), 0.0f}
  };
}



vec3d_t mat4x4_mult(vec3d_t v, matrix4x4_t m)
{
  vec3d_t res;
  res.x   = (v.x * m.m[0][0]) + (v.y * m.m[1][0]) + (v.z * m.m[2][0]) + m.m[3][0];
  res.y   = (v.x * m.m[0][1]) + (v.y * m.m[1][1]) + (v.z * m.m[2][1]) + m.m[3][1];
  res.z   = (v.x * m.m[0][2]) + (v.y * m.m[1][2]) + (v.z * m.m[2][2]) + m.m[3][2];
  float w = (v.x * m.m[0][3]) + (v.y * m.m[1][3]) + (v.z * m.m[2][3]) + m.m[3][3];
  if (w != 0) {
    res.x /= w;
    res.y /= w;
    res.z /= w;
  }
  return res;
}

vec3d_t mat3x3_mult(vec3d_t v, matrix3x3_t mat)
{
  return (vec3d_t)
  {
    (mat.r1.x * v.x) + (mat.r1.y * v.y) + (mat.r1.z * v.z),
    (mat.r2.x * v.x) + (mat.r2.y * v.y) + (mat.r2.z * v.z),
    (mat.r3.x * v.x) + (mat.r3.y * v.y) + (mat.r3.z * v.z)
  };
}
vec3d_t rotate_x(vec3d_t v, float angle)
{
  return mat3x3_mult(v, rot_x_matrix(angle));
}
vec3d_t rotate_y(vec3d_t v, float angle)
{
  return mat3x3_mult(v, rot_y_matrix(angle));
}
vec3d_t rotate_z(vec3d_t v, float angle)
{
  return mat3x3_mult(v, rot_z_matrix(angle));
}
