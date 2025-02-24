#pragma once
#ifndef _CVKM_H_
#define _CVKM_H_
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef CVKM_ENABLE_FLECS
#include <flecs.h>
#endif

#if defined(__GNUC__) || defined(__clang__)
#define IGNORE_WARNINGS_BEGIN\
  _Pragma("GCC diagnostic push")\
  _Pragma("GCC diagnostic ignored \"-Wunused-function\"")
#define IGNORE_WARNINGS_END\
  _Pragma("GCC diagnostic pop")
#else
#define IGNORE_WARNINGS_BEGIN
#define IGNORE_WARNINGS_END
#endif

IGNORE_WARNINGS_BEGIN

#define CVKM_DEFINE_VEC2(prefix, type) typedef union vkm_##prefix##vec2 {\
  struct {\
    type x, y;\
  };\
  struct {\
    type r, g;\
  };\
  struct {\
    type s, t;\
  };\
  struct {\
    type u, v;\
  };\
  type raw[2];\
} vkm_##prefix##vec2

#define CVKM_DEFINE_VEC3(prefix, type) typedef union vkm_##prefix##vec3 {\
  struct {\
    type x, y, z;\
  };\
  struct {\
    type r, g, b;\
  };\
  struct {\
    type s, t, p;\
  };\
  type raw[3];\
} vkm_##prefix##vec3

#define CVKM_DEFINE_VEC4(prefix, type) typedef union vkm_##prefix##vec4 {\
  struct {\
    type x, y, z, w;\
  };\
  struct {\
    type r, g, b, a;\
  };\
  struct {\
    type s, t, p, q;\
  };\
  type raw[4];\
} vkm_##prefix##vec4

CVKM_DEFINE_VEC2(b, int8_t);
CVKM_DEFINE_VEC2(ub, uint8_t);
CVKM_DEFINE_VEC2(s, int16_t);
CVKM_DEFINE_VEC2(us, uint16_t);
CVKM_DEFINE_VEC2(i, int32_t);
CVKM_DEFINE_VEC2(u, uint32_t);
CVKM_DEFINE_VEC2(l, int64_t);
CVKM_DEFINE_VEC2(ul, uint64_t);
CVKM_DEFINE_VEC2(, float);
CVKM_DEFINE_VEC2(d, double);

CVKM_DEFINE_VEC3(b, int8_t);
CVKM_DEFINE_VEC3(ub, uint8_t);
CVKM_DEFINE_VEC3(s, int16_t);
CVKM_DEFINE_VEC3(us, uint16_t);
CVKM_DEFINE_VEC3(i, int32_t);
CVKM_DEFINE_VEC3(u, uint32_t);
CVKM_DEFINE_VEC3(l, int64_t);
CVKM_DEFINE_VEC3(ul, uint64_t);
CVKM_DEFINE_VEC3(, float);
CVKM_DEFINE_VEC3(d, double);

CVKM_DEFINE_VEC4(b, int8_t);
CVKM_DEFINE_VEC4(ub, uint8_t);
CVKM_DEFINE_VEC4(s, int16_t);
CVKM_DEFINE_VEC4(us, uint16_t);
CVKM_DEFINE_VEC4(i, int32_t);
CVKM_DEFINE_VEC4(u, uint32_t);
CVKM_DEFINE_VEC4(l, int64_t);
CVKM_DEFINE_VEC4(ul, uint64_t);
CVKM_DEFINE_VEC4(, float);
CVKM_DEFINE_VEC4(d, double);

typedef union vkm_mat4 {
  vkm_vec4 columns[4];
  struct {
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
  };
  float raw[16];
} vkm_mat4;

#define CVKM_MAT4_IDENTITY (vkm_mat4){\
  1.0f, 0.0f, 0.0f, 0.0f,\
  0.0f, 1.0f, 0.0f, 0.0f,\
  0.0f, 0.0f, 1.0f, 0.0f,\
  0.0f, 0.0f, 0.0f, 1.0f,\
}

#define CVKM_VEC2_OPERATION(type, operation, operator) static void vkm_##type##_##operation(\
  const vkm_##type* a,\
  const vkm_##type* b,\
  vkm_##type* result\
) {\
  *result = (vkm_##type){ { a->x operator b->x, a->y operator b->y } };\
}

#define CVKM_VEC2_SCALAR_OPERATION(vec_type, scalar_type, operation, operator)\
static void vkm_##vec_type##_##operation##_scalar(\
  const vkm_##vec_type* vec,\
  const scalar_type scalar,\
  vkm_##vec_type* result\
) {\
  *result = (vkm_##vec_type){ { vec->x operator scalar, vec->y operator scalar } };\
}

#define CVKM_VEC2_ALL_OPERATIONS(vec_type, scalar_type) CVKM_VEC2_OPERATION(vec_type, add, +)\
  CVKM_VEC2_OPERATION(vec_type, sub, -)\
  CVKM_VEC2_OPERATION(vec_type, mul, *)\
  CVKM_VEC2_OPERATION(vec_type, div, /)\
  CVKM_VEC2_SCALAR_OPERATION(vec_type, scalar_type, mul, *)\
  CVKM_VEC2_SCALAR_OPERATION(vec_type, scalar_type, div, /)

CVKM_VEC2_ALL_OPERATIONS(bvec2, int8_t)
CVKM_VEC2_ALL_OPERATIONS(ubvec2, uint8_t)
CVKM_VEC2_ALL_OPERATIONS(svec2, int16_t)
CVKM_VEC2_ALL_OPERATIONS(usvec2, uint16_t)
CVKM_VEC2_ALL_OPERATIONS(ivec2, int32_t)
CVKM_VEC2_ALL_OPERATIONS(uvec2, uint32_t)
CVKM_VEC2_ALL_OPERATIONS(lvec2, int64_t)
CVKM_VEC2_ALL_OPERATIONS(ulvec2, uint64_t)
CVKM_VEC2_ALL_OPERATIONS(vec2, float)
CVKM_VEC2_ALL_OPERATIONS(dvec2, double)

#define CVKM_VEC3_OPERATION(type, operation, operator) static void vkm_##type##_##operation(\
  const vkm_##type* a,\
  const vkm_##type* b,\
  vkm_##type* result\
) {\
  *result = (vkm_##type){ { a->x operator b->x, a->y operator b->y, a->z operator b->z } };\
}

#define CVKM_VEC3_SCALAR_OPERATION(vec_type, scalar_type, operation, operator)\
static void vkm_##vec_type##_##operation##_scalar(\
  const vkm_##vec_type* vec,\
  const scalar_type scalar,\
  vkm_##vec_type* result\
) {\
  *result = (vkm_##vec_type){ { vec->x operator scalar, vec->y operator scalar, vec->z operator scalar } };\
}

#define CVKM_VEC3_ALL_OPERATIONS(vec_type, scalar_type) CVKM_VEC3_OPERATION(vec_type, add, +)\
  CVKM_VEC3_OPERATION(vec_type, sub, -)\
  CVKM_VEC3_OPERATION(vec_type, mul, *)\
  CVKM_VEC3_OPERATION(vec_type, div, /)\
  CVKM_VEC3_SCALAR_OPERATION(vec_type, scalar_type, mul, *)\
  CVKM_VEC3_SCALAR_OPERATION(vec_type, scalar_type, div, /)

CVKM_VEC3_ALL_OPERATIONS(bvec3, int8_t)
CVKM_VEC3_ALL_OPERATIONS(ubvec3, uint8_t)
CVKM_VEC3_ALL_OPERATIONS(svec3, int16_t)
CVKM_VEC3_ALL_OPERATIONS(usvec3, uint16_t)
CVKM_VEC3_ALL_OPERATIONS(ivec3, int32_t)
CVKM_VEC3_ALL_OPERATIONS(uvec3, uint32_t)
CVKM_VEC3_ALL_OPERATIONS(lvec3, int64_t)
CVKM_VEC3_ALL_OPERATIONS(ulvec3, uint64_t)
CVKM_VEC3_ALL_OPERATIONS(vec3, float)
CVKM_VEC3_ALL_OPERATIONS(dvec3, double)

#define CVKM_VEC4_OPERATION(type, operation, operator) static void vkm_##type##_##operation(\
  const vkm_##type* a,\
  const vkm_##type* b,\
  vkm_##type* result\
) {\
  *result = (vkm_##type){ { a->x operator b->x, a->y operator b->y, a->z operator b->z, a->w operator b->w } };\
}

#define CVKM_VEC4_SCALAR_OPERATION(vec_type, scalar_type, operation, operator)\
static void vkm_##vec_type##_##operation##_scalar(\
  const vkm_##vec_type* vec,\
  const scalar_type scalar,\
  vkm_##vec_type* result\
) {\
  *result = (vkm_##vec_type){ {\
    vec->x operator scalar,\
    vec->y operator scalar,\
    vec->z operator scalar,\
    vec->w operator scalar,\
  } };\
}

#define CVKM_VEC4_ALL_OPERATIONS(vec_type, scalar_type) CVKM_VEC4_OPERATION(vec_type, add, +)\
  CVKM_VEC4_OPERATION(vec_type, sub, -)\
  CVKM_VEC4_OPERATION(vec_type, mul, *)\
  CVKM_VEC4_OPERATION(vec_type, div, /)\
  CVKM_VEC4_SCALAR_OPERATION(vec_type, scalar_type, mul, *)\
  CVKM_VEC4_SCALAR_OPERATION(vec_type, scalar_type, div, /)

CVKM_VEC4_ALL_OPERATIONS(bvec4, int8_t)
CVKM_VEC4_ALL_OPERATIONS(ubvec4, uint8_t)
CVKM_VEC4_ALL_OPERATIONS(svec4, int16_t)
CVKM_VEC4_ALL_OPERATIONS(usvec4, uint16_t)
CVKM_VEC4_ALL_OPERATIONS(ivec4, int32_t)
CVKM_VEC4_ALL_OPERATIONS(uvec4, uint32_t)
CVKM_VEC4_ALL_OPERATIONS(lvec4, int64_t)
CVKM_VEC4_ALL_OPERATIONS(ulvec4, uint64_t)
CVKM_VEC4_ALL_OPERATIONS(vec4, float)
CVKM_VEC4_ALL_OPERATIONS(dvec4, double)

#define vkm_add(a, b, result) _Generic((result),\
  vkm_bvec2*: vkm_bvec2_add,\
  vkm_ubvec2*: vkm_ubvec2_add,\
  vkm_svec2*: vkm_svec2_add,\
  vkm_usvec2*: vkm_usvec2_add,\
  vkm_ivec2*: vkm_ivec2_add,\
  vkm_uvec2*: vkm_uvec2_add,\
  vkm_lvec2*: vkm_lvec2_add,\
  vkm_ulvec2*: vkm_ulvec2_add,\
  vkm_vec2*: vkm_vec2_add,\
  vkm_dvec2*: vkm_dvec2_add,\
  vkm_bvec3*: vkm_bvec3_add,\
  vkm_ubvec3*: vkm_ubvec3_add,\
  vkm_svec3*: vkm_svec3_add,\
  vkm_usvec3*: vkm_usvec3_add,\
  vkm_ivec3*: vkm_ivec3_add,\
  vkm_uvec3*: vkm_uvec3_add,\
  vkm_lvec3*: vkm_lvec3_add,\
  vkm_ulvec3*: vkm_ulvec3_add,\
  vkm_vec3*: vkm_vec3_add,\
  vkm_dvec3*: vkm_dvec3_add,\
  vkm_bvec4*: vkm_bvec4_add,\
  vkm_ubvec4*: vkm_ubvec4_add,\
  vkm_svec4*: vkm_svec4_add,\
  vkm_usvec4*: vkm_usvec4_add,\
  vkm_ivec4*: vkm_ivec4_add,\
  vkm_uvec4*: vkm_uvec4_add,\
  vkm_lvec4*: vkm_lvec4_add,\
  vkm_ulvec4*: vkm_ulvec4_add,\
  vkm_vec4*: vkm_vec4_add,\
  vkm_dvec4*: vkm_dvec4_add\
)((a), (b), (result))

#define vkm_sub(a, b, result) _Generic((result),\
  vkm_bvec2*: vkm_bvec2_sub,\
  vkm_ubvec2*: vkm_ubvec2_sub,\
  vkm_svec2*: vkm_svec2_sub,\
  vkm_usvec2*: vkm_usvec2_sub,\
  vkm_ivec2*: vkm_ivec2_sub,\
  vkm_uvec2*: vkm_uvec2_sub,\
  vkm_lvec2*: vkm_lvec2_sub,\
  vkm_ulvec2*: vkm_ulvec2_sub,\
  vkm_vec2*: vkm_vec2_sub,\
  vkm_dvec2*: vkm_dvec2_sub,\
  vkm_bvec3*: vkm_bvec3_sub,\
  vkm_ubvec3*: vkm_ubvec3_sub,\
  vkm_svec3*: vkm_svec3_sub,\
  vkm_usvec3*: vkm_usvec3_sub,\
  vkm_ivec3*: vkm_ivec3_sub,\
  vkm_uvec3*: vkm_uvec3_sub,\
  vkm_lvec3*: vkm_lvec3_sub,\
  vkm_ulvec3*: vkm_ulvec3_sub,\
  vkm_vec3*: vkm_vec3_sub,\
  vkm_dvec3*: vkm_dvec3_sub,\
  vkm_bvec4*: vkm_bvec4_sub,\
  vkm_ubvec4*: vkm_ubvec4_sub,\
  vkm_svec4*: vkm_svec4_sub,\
  vkm_usvec4*: vkm_usvec4_sub,\
  vkm_ivec4*: vkm_ivec4_sub,\
  vkm_uvec4*: vkm_uvec4_sub,\
  vkm_lvec4*: vkm_lvec4_sub,\
  vkm_ulvec4*: vkm_ulvec4_sub,\
  vkm_vec4*: vkm_vec4_sub,\
  vkm_dvec4*: vkm_dvec4_sub\
)((a), (b), (result))

#define CVKM_MUL_DIV_OPERATIONS(vec_type, scalar_type, operation, b) vkm_##vec_type*: _Generic((b),\
  vkm_##vec_type*: vkm_##vec_type##_##operation,\
  scalar_type: vkm_##vec_type##_##operation##_scalar,\
  default: ((void)0)\
)\

#define vkm_mul(a, b, result) _Generic((result),\
  CVKM_MUL_DIV_OPERATIONS(bvec2, int8_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(ubvec2, uint8_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(svec2, int16_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(usvec2, uint16_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(ivec2, int32_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(uvec2, uint32_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(lvec2, int64_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(ulvec2, uint64_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(vec2, float, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(dvec2, double, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(bvec3, int8_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(ubvec3, uint8_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(svec3, int16_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(usvec3, uint16_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(ivec3, int32_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(uvec3, uint32_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(lvec3, int64_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(ulvec3, uint64_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(vec3, float, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(dvec3, double, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(bvec4, int8_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(ubvec4, uint8_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(svec4, int16_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(usvec4, uint16_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(ivec4, int32_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(uvec4, uint32_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(lvec4, int64_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(ulvec4, uint64_t, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(vec4, float, mul, b),\
  CVKM_MUL_DIV_OPERATIONS(dvec4, double, mul, b),\
  vkm_mat4*: _Generic((b),\
    vkm_mat4*: vkm_mat4_mul,\
    default: ((void)0)\
  )\
)((a), (b), (result))

#define vkm_div(a, b, result) _Generic((result),\
  CVKM_MUL_DIV_OPERATIONS(bvec2, int8_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(ubvec2, uint8_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(svec2, int16_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(usvec2, uint16_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(ivec2, int32_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(uvec2, uint32_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(lvec2, int64_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(ulvec2, uint64_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(vec2, float, div, b),\
  CVKM_MUL_DIV_OPERATIONS(dvec2, double, div, b),\
  CVKM_MUL_DIV_OPERATIONS(bvec3, int8_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(ubvec3, uint8_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(svec3, int16_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(usvec3, uint16_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(ivec3, int32_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(uvec3, uint32_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(lvec3, int64_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(ulvec3, uint64_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(vec3, float, div, b),\
  CVKM_MUL_DIV_OPERATIONS(dvec3, double, div, b),\
  CVKM_MUL_DIV_OPERATIONS(bvec4, int8_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(ubvec4, uint8_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(svec4, int16_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(usvec4, uint16_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(ivec4, int32_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(uvec4, uint32_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(lvec4, int64_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(ulvec4, uint64_t, div, b),\
  CVKM_MUL_DIV_OPERATIONS(vec4, float, div, b),\
  CVKM_MUL_DIV_OPERATIONS(dvec4, double, div, b)\
)((a), (b), (result))

#define CVKM_SQRT(type, suffix) static type vkm_sqrt##suffix(const type x) {\
  return (type)sqrt((double)x);\
}

CVKM_SQRT(int8_t, b)
CVKM_SQRT(uint8_t, ub)
CVKM_SQRT(int16_t, s)
CVKM_SQRT(uint16_t, us)
CVKM_SQRT(int32_t, i)
CVKM_SQRT(uint32_t, ui)
CVKM_SQRT(int64_t, l)
CVKM_SQRT(uint64_t, ul)

#define vkm_sqrt(x) _Generic((x),\
  int8_t: vkm_sqrtb,\
  uint8_t: vkm_sqrtub,\
  int16_t: vkm_sqrts,\
  uint16_t: vkm_sqrtus,\
  int32_t: vkm_sqrti,\
  uint32_t: vkm_sqrtui,\
  int64_t: vkm_sqrtl,\
  uint64_t: vkm_sqrtul,\
  float: sqrtf,\
  double: sqrt\
)(x)

#define CVKM_VEC2_MISC_OPERATIONS_FOR_UNSIGNED_INTS(vec_type, scalar_type) static scalar_type vkm_##vec_type##_dot(\
  const vkm_##vec_type* a,\
  const vkm_##vec_type* b\
) {\
  return a->x * b->x + a->y * b->y;\
}\
\
static scalar_type vkm_##vec_type##_sqr_magnitude(const vkm_##vec_type* vec) {\
  return vec->x * vec->x + vec->y * vec->y;\
}\
\
static scalar_type vkm_##vec_type##_magnitude(const vkm_##vec_type* vec) {\
  return vkm_sqrt(vkm_##vec_type##_sqr_magnitude(vec));\
}\
\
static void vkm_##vec_type##_clear(vkm_##vec_type* vec) {\
  *vec = (vkm_##vec_type){ { (scalar_type)0, (scalar_type)0 } };\
}

#define CVKM_VEC2_INVERT(vec_type) static void vkm_##vec_type##_invert(\
  const vkm_##vec_type* vec,\
  vkm_##vec_type* result\
) {\
  *result = (vkm_##vec_type){ { -vec->x, -vec->y } };\
}

#define CVKM_VEC2_NORMALIZE(vec_type, scalar_type) static void vkm_##vec_type##_normalize(\
  const vkm_##vec_type* vec,\
  vkm_##vec_type* result\
) {\
  const scalar_type magnitude = vkm_##vec_type##_magnitude(vec);\
  if (magnitude > (scalar_type)0) {\
    vkm_##vec_type##_div_scalar(vec, magnitude, result);\
  }\
}

#define CVKM_VEC2_MISC_OPERATIONS_FOR_INTS(vec_type, scalar_type)\
  CVKM_VEC2_MISC_OPERATIONS_FOR_UNSIGNED_INTS(vec_type, scalar_type)\
  CVKM_VEC2_INVERT(vec_type)

#define CVKM_VEC2_MISC_OPERATIONS(vec_type, scalar_type)\
  CVKM_VEC2_MISC_OPERATIONS_FOR_INTS(vec_type, scalar_type)\
  CVKM_VEC2_NORMALIZE(vec_type, scalar_type)

CVKM_VEC2_MISC_OPERATIONS_FOR_INTS(bvec2, int8_t)
CVKM_VEC2_MISC_OPERATIONS_FOR_UNSIGNED_INTS(ubvec2, uint8_t)
CVKM_VEC2_MISC_OPERATIONS_FOR_INTS(svec2, int16_t)
CVKM_VEC2_MISC_OPERATIONS_FOR_UNSIGNED_INTS(usvec2, uint16_t)
CVKM_VEC2_MISC_OPERATIONS_FOR_INTS(ivec2, int32_t)
CVKM_VEC2_MISC_OPERATIONS_FOR_UNSIGNED_INTS(uvec2, uint32_t)
CVKM_VEC2_MISC_OPERATIONS_FOR_INTS(lvec2, int64_t)
CVKM_VEC2_MISC_OPERATIONS_FOR_UNSIGNED_INTS(ulvec2, uint64_t)
CVKM_VEC2_MISC_OPERATIONS(vec2, float)
CVKM_VEC2_MISC_OPERATIONS(dvec2, double)

#define CVKM_VEC3_MISC_OPERATIONS_FOR_UNSIGNED_INTS(vec_type, scalar_type) static scalar_type vkm_##vec_type##_dot(\
  const vkm_##vec_type* a,\
  const vkm_##vec_type* b\
) {\
  return a->x * b->x + a->y * b->y + a->z * b->z;\
}\
\
static void vkm_##vec_type##_cross(const vkm_##vec_type* a, const vkm_##vec_type* b, vkm_##vec_type* result){\
  *result = (vkm_##vec_type){ {\
    a->y * b->z - a->z * b->y,\
    a->z * b->x - a->x * b->z,\
    a->x * b->y - a->y * b->x,\
  } };\
}\
\
static scalar_type vkm_##vec_type##_sqr_magnitude(const vkm_##vec_type* vec) {\
  return vec->x * vec->x + vec->y * vec->y + vec->z * vec->z;\
}\
\
static scalar_type vkm_##vec_type##_magnitude(const vkm_##vec_type* vec) {\
  return vkm_sqrt(vkm_##vec_type##_sqr_magnitude(vec));\
}\
\
static void vkm_##vec_type##_clear(vkm_##vec_type* vec) {\
  *vec = (vkm_##vec_type){ { (scalar_type)0, (scalar_type)0, (scalar_type)0 } };\
}

#define CVKM_VEC3_INVERT(vec_type) static void vkm_##vec_type##_invert(\
  const vkm_##vec_type* vec,\
  vkm_##vec_type* result\
) {\
  *result = (vkm_##vec_type){ { -vec->x, -vec->y, -vec->z } };\
}

#define CVKM_VEC3_NORMALIZE(vec_type, scalar_type) static void vkm_##vec_type##_normalize(\
  const vkm_##vec_type* vec,\
  vkm_##vec_type* result\
) {\
  const scalar_type magnitude = vkm_##vec_type##_magnitude(vec);\
  if (magnitude > (scalar_type)0) {\
    vkm_##vec_type##_div_scalar(vec, magnitude, result);\
  }\
}

#define CVKM_VEC3_MISC_OPERATIONS_FOR_INTS(vec_type, scalar_type)\
  CVKM_VEC3_MISC_OPERATIONS_FOR_UNSIGNED_INTS(vec_type, scalar_type)\
  CVKM_VEC3_INVERT(vec_type)

#define CVKM_VEC3_MISC_OPERATIONS(vec_type, scalar_type)\
  CVKM_VEC3_MISC_OPERATIONS_FOR_INTS(vec_type, scalar_type)\
  CVKM_VEC3_NORMALIZE(vec_type, scalar_type)

CVKM_VEC3_MISC_OPERATIONS_FOR_INTS(bvec3, int8_t)
CVKM_VEC3_MISC_OPERATIONS_FOR_UNSIGNED_INTS(ubvec3, uint8_t)
CVKM_VEC3_MISC_OPERATIONS_FOR_INTS(svec3, int16_t)
CVKM_VEC3_MISC_OPERATIONS_FOR_UNSIGNED_INTS(usvec3, uint16_t)
CVKM_VEC3_MISC_OPERATIONS_FOR_INTS(ivec3, int32_t)
CVKM_VEC3_MISC_OPERATIONS_FOR_UNSIGNED_INTS(uvec3, uint32_t)
CVKM_VEC3_MISC_OPERATIONS_FOR_INTS(lvec3, int64_t)
CVKM_VEC3_MISC_OPERATIONS_FOR_UNSIGNED_INTS(ulvec3, uint64_t)
CVKM_VEC3_MISC_OPERATIONS(vec3, float)
CVKM_VEC3_MISC_OPERATIONS(dvec3, double)

#define CVKM_VEC4_MISC_OPERATIONS_FOR_UNSIGNED_INTS(vec_type, scalar_type) static scalar_type vkm_##vec_type##_dot(\
  const vkm_##vec_type* a,\
  const vkm_##vec_type* b\
) {\
  return a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;\
}\
\
static scalar_type vkm_##vec_type##_sqr_magnitude(const vkm_##vec_type* vec) {\
  return vec->x * vec->x + vec->y * vec->y + vec->z * vec->z + vec->w * vec->w;\
}\
\
static scalar_type vkm_##vec_type##_magnitude(const vkm_##vec_type* vec) {\
  return vkm_sqrt(vkm_##vec_type##_sqr_magnitude(vec));\
}\
\
static void vkm_##vec_type##_clear(vkm_##vec_type* vec) {\
  *vec = (vkm_##vec_type){ { (scalar_type)0, (scalar_type)0, (scalar_type)0, (scalar_type)0 } };\
}

#define CVKM_VEC4_INVERT(vec_type) static void vkm_##vec_type##_invert(\
  const vkm_##vec_type* vec,\
  vkm_##vec_type* result\
) {\
  *result = (vkm_##vec_type){ { -vec->x, -vec->y, -vec->z, -vec->w } };\
}

#define CVKM_VEC4_NORMALIZE(vec_type, scalar_type) static void vkm_##vec_type##_normalize(\
  const vkm_##vec_type* vec,\
  vkm_##vec_type* result\
) {\
  const scalar_type magnitude = vkm_##vec_type##_magnitude(vec);\
  if (magnitude > (scalar_type)0) {\
    vkm_##vec_type##_div_scalar(vec, magnitude, result);\
  }\
}

#define CVKM_VEC4_MISC_OPERATIONS_FOR_INTS(vec_type, scalar_type)\
  CVKM_VEC4_MISC_OPERATIONS_FOR_UNSIGNED_INTS(vec_type, scalar_type)\
  CVKM_VEC4_INVERT(vec_type)

#define CVKM_VEC4_MISC_OPERATIONS(vec_type, scalar_type)\
  CVKM_VEC4_MISC_OPERATIONS_FOR_INTS(vec_type, scalar_type)\
  CVKM_VEC4_NORMALIZE(vec_type, scalar_type)

CVKM_VEC4_MISC_OPERATIONS_FOR_INTS(bvec4, int8_t)
CVKM_VEC4_MISC_OPERATIONS_FOR_UNSIGNED_INTS(ubvec4, uint8_t)
CVKM_VEC4_MISC_OPERATIONS_FOR_INTS(svec4, int16_t)
CVKM_VEC4_MISC_OPERATIONS_FOR_UNSIGNED_INTS(usvec4, uint16_t)
CVKM_VEC4_MISC_OPERATIONS_FOR_INTS(ivec4, int32_t)
CVKM_VEC4_MISC_OPERATIONS_FOR_UNSIGNED_INTS(uvec4, uint32_t)
CVKM_VEC4_MISC_OPERATIONS_FOR_INTS(lvec4, int64_t)
CVKM_VEC4_MISC_OPERATIONS_FOR_UNSIGNED_INTS(ulvec4, uint64_t)
CVKM_VEC4_MISC_OPERATIONS(vec4, float)
CVKM_VEC4_MISC_OPERATIONS(dvec4, double)

#define vkm_dot(a, b) _Generic((a),\
  vkm_bvec2*: vkm_bvec2_dot,\
  const vkm_bvec2*: vkm_bvec2_dot,\
  vkm_ubvec2*: vkm_ubvec2_dot,\
  const vkm_ubvec2*: vkm_ubvec2_dot,\
  vkm_svec2*: vkm_svec2_dot,\
  const vkm_svec2*: vkm_svec2_dot,\
  vkm_usvec2*: vkm_usvec2_dot,\
  const vkm_usvec2*: vkm_usvec2_dot,\
  vkm_ivec2*: vkm_ivec2_dot,\
  const vkm_ivec2*: vkm_ivec2_dot,\
  vkm_uvec2*: vkm_uvec2_dot,\
  const vkm_uvec2*: vkm_uvec2_dot,\
  vkm_lvec2*: vkm_lvec2_dot,\
  const vkm_lvec2*: vkm_lvec2_dot,\
  vkm_ulvec2*: vkm_ulvec2_dot,\
  const vkm_ulvec2*: vkm_ulvec2_dot,\
  vkm_vec2*: vkm_vec2_dot,\
  const vkm_vec2*: vkm_vec2_dot,\
  vkm_dvec2*: vkm_dvec2_dot,\
  const vkm_dvec2*: vkm_dvec2_dot,\
  vkm_bvec3*: vkm_bvec3_dot,\
  const vkm_bvec3*: vkm_bvec3_dot,\
  vkm_ubvec3*: vkm_ubvec3_dot,\
  const vkm_ubvec3*: vkm_ubvec3_dot,\
  vkm_svec3*: vkm_svec3_dot,\
  const vkm_svec3*: vkm_svec3_dot,\
  vkm_usvec3*: vkm_usvec3_dot,\
  const vkm_usvec3*: vkm_usvec3_dot,\
  vkm_ivec3*: vkm_ivec3_dot,\
  const vkm_ivec3*: vkm_ivec3_dot,\
  vkm_uvec3*: vkm_uvec3_dot,\
  const vkm_uvec3*: vkm_uvec3_dot,\
  vkm_lvec3*: vkm_lvec3_dot,\
  const vkm_lvec3*: vkm_lvec3_dot,\
  vkm_ulvec3*: vkm_ulvec3_dot,\
  const vkm_ulvec3*: vkm_ulvec3_dot,\
  vkm_vec3*: vkm_vec3_dot,\
  const vkm_vec3*: vkm_vec3_dot,\
  vkm_dvec3*: vkm_dvec3_dot,\
  const vkm_dvec3*: vkm_dvec3_dot,\
  vkm_bvec4*: vkm_bvec4_dot,\
  const vkm_bvec4*: vkm_bvec4_dot,\
  vkm_ubvec4*: vkm_ubvec4_dot,\
  const vkm_ubvec4*: vkm_ubvec4_dot,\
  vkm_svec4*: vkm_svec4_dot,\
  const vkm_svec4*: vkm_svec4_dot,\
  vkm_usvec4*: vkm_usvec4_dot,\
  const vkm_usvec4*: vkm_usvec4_dot,\
  vkm_ivec4*: vkm_ivec4_dot,\
  const vkm_ivec4*: vkm_ivec4_dot,\
  vkm_uvec4*: vkm_uvec4_dot,\
  const vkm_uvec4*: vkm_uvec4_dot,\
  vkm_lvec4*: vkm_lvec4_dot,\
  const vkm_lvec4*: vkm_lvec4_dot,\
  vkm_ulvec4*: vkm_ulvec4_dot,\
  const vkm_ulvec4*: vkm_ulvec4_dot,\
  vkm_vec4*: vkm_vec4_dot,\
  const vkm_vec4*: vkm_vec4_dot,\
  vkm_dvec4*: vkm_dvec4_dot,\
  const vkm_dvec4*: vkm_dvec4_dot\
)((a), (b))

#define vkm_cross(a, b, result) _Generic((result),\
  vkm_bvec3*: vkm_bvec3_cross,\
  vkm_ubvec3*: vkm_ubvec3_cross,\
  vkm_svec3*: vkm_svec3_cross,\
  vkm_usvec3*: vkm_usvec3_cross,\
  vkm_ivec3*: vkm_ivec3_cross,\
  vkm_uvec3*: vkm_uvec3_cross,\
  vkm_lvec3*: vkm_lvec3_cross,\
  vkm_ulvec3*: vkm_ulvec3_cross,\
  vkm_vec3*: vkm_vec3_cross,\
  vkm_dvec3*: vkm_dvec3_cross\
)((a), (b), (result))

#define vkm_sqr_magnitude(vec) _Generic((vec),\
  vkm_bvec2*: vkm_bvec2_sqr_magnitude,\
  const vkm_bvec2*: vkm_bvec2_sqr_magnitude,\
  vkm_ubvec2*: vkm_ubvec2_sqr_magnitude,\
  const vkm_ubvec2*: vkm_ubvec2_sqr_magnitude,\
  vkm_svec2*: vkm_svec2_sqr_magnitude,\
  const vkm_svec2*: vkm_svec2_sqr_magnitude,\
  vkm_usvec2*: vkm_usvec2_sqr_magnitude,\
  const vkm_usvec2*: vkm_usvec2_sqr_magnitude,\
  vkm_ivec2*: vkm_ivec2_sqr_magnitude,\
  const vkm_ivec2*: vkm_ivec2_sqr_magnitude,\
  vkm_uvec2*: vkm_uvec2_sqr_magnitude,\
  const vkm_uvec2*: vkm_uvec2_sqr_magnitude,\
  vkm_lvec2*: vkm_lvec2_sqr_magnitude,\
  const vkm_lvec2*: vkm_lvec2_sqr_magnitude,\
  vkm_ulvec2*: vkm_ulvec2_sqr_magnitude,\
  const vkm_ulvec2*: vkm_ulvec2_sqr_magnitude,\
  vkm_vec2*: vkm_vec2_sqr_magnitude,\
  const vkm_vec2*: vkm_vec2_sqr_magnitude,\
  vkm_dvec2*: vkm_dvec2_sqr_magnitude,\
  const vkm_dvec2*: vkm_dvec2_sqr_magnitude,\
  vkm_bvec3*: vkm_bvec3_sqr_magnitude,\
  const vkm_bvec3*: vkm_bvec3_sqr_magnitude,\
  vkm_ubvec3*: vkm_ubvec3_sqr_magnitude,\
  const vkm_ubvec3*: vkm_ubvec3_sqr_magnitude,\
  vkm_svec3*: vkm_svec3_sqr_magnitude,\
  const vkm_svec3*: vkm_svec3_sqr_magnitude,\
  vkm_usvec3*: vkm_usvec3_sqr_magnitude,\
  const vkm_usvec3*: vkm_usvec3_sqr_magnitude,\
  vkm_ivec3*: vkm_ivec3_sqr_magnitude,\
  const vkm_ivec3*: vkm_ivec3_sqr_magnitude,\
  vkm_uvec3*: vkm_uvec3_sqr_magnitude,\
  const vkm_uvec3*: vkm_uvec3_sqr_magnitude,\
  vkm_lvec3*: vkm_lvec3_sqr_magnitude,\
  const vkm_lvec3*: vkm_lvec3_sqr_magnitude,\
  vkm_ulvec3*: vkm_ulvec3_sqr_magnitude,\
  const vkm_ulvec3*: vkm_ulvec3_sqr_magnitude,\
  vkm_vec3*: vkm_vec3_sqr_magnitude,\
  const vkm_vec3*: vkm_vec3_sqr_magnitude,\
  vkm_dvec3*: vkm_dvec3_sqr_magnitude,\
  const vkm_dvec3*: vkm_dvec3_sqr_magnitude,\
  vkm_bvec4*: vkm_bvec4_sqr_magnitude,\
  const vkm_bvec4*: vkm_bvec4_sqr_magnitude,\
  vkm_ubvec4*: vkm_ubvec4_sqr_magnitude,\
  const vkm_ubvec4*: vkm_ubvec4_sqr_magnitude,\
  vkm_svec4*: vkm_svec4_sqr_magnitude,\
  const vkm_svec4*: vkm_svec4_sqr_magnitude,\
  vkm_usvec4*: vkm_usvec4_sqr_magnitude,\
  const vkm_usvec4*: vkm_usvec4_sqr_magnitude,\
  vkm_ivec4*: vkm_ivec4_sqr_magnitude,\
  const vkm_ivec4*: vkm_ivec4_sqr_magnitude,\
  vkm_uvec4*: vkm_uvec4_sqr_magnitude,\
  const vkm_uvec4*: vkm_uvec4_sqr_magnitude,\
  vkm_lvec4*: vkm_lvec4_sqr_magnitude,\
  const vkm_lvec4*: vkm_lvec4_sqr_magnitude,\
  vkm_ulvec4*: vkm_ulvec4_sqr_magnitude,\
  const vkm_ulvec4*: vkm_ulvec4_sqr_magnitude,\
  vkm_vec4*: vkm_vec4_sqr_magnitude,\
  const vkm_vec4*: vkm_vec4_sqr_magnitude,\
  vkm_dvec4*: vkm_dvec4_sqr_magnitude,\
  const vkm_dvec4*: vkm_dvec4_sqr_magnitude\
)(vec)

#define vkm_magnitude(vec) _Generic((vec),\
  vkm_bvec2*: vkm_bvec2_magnitude,\
  const vkm_bvec2*: vkm_bvec2_magnitude,\
  vkm_ubvec2*: vkm_ubvec2_magnitude,\
  const vkm_ubvec2*: vkm_ubvec2_magnitude,\
  vkm_svec2*: vkm_svec2_magnitude,\
  const vkm_svec2*: vkm_svec2_magnitude,\
  vkm_usvec2*: vkm_usvec2_magnitude,\
  const vkm_usvec2*: vkm_usvec2_magnitude,\
  vkm_ivec2*: vkm_ivec2_magnitude,\
  const vkm_ivec2*: vkm_ivec2_magnitude,\
  vkm_uvec2*: vkm_uvec2_magnitude,\
  const vkm_uvec2*: vkm_uvec2_magnitude,\
  vkm_lvec2*: vkm_lvec2_magnitude,\
  const vkm_lvec2*: vkm_lvec2_magnitude,\
  vkm_ulvec2*: vkm_ulvec2_magnitude,\
  const vkm_ulvec2*: vkm_ulvec2_magnitude,\
  vkm_vec2*: vkm_vec2_magnitude,\
  const vkm_vec2*: vkm_vec2_magnitude,\
  vkm_dvec2*: vkm_dvec2_magnitude,\
  const vkm_dvec2*: vkm_dvec2_magnitude,\
  vkm_bvec3*: vkm_bvec3_magnitude,\
  const vkm_bvec3*: vkm_bvec3_magnitude,\
  vkm_ubvec3*: vkm_ubvec3_magnitude,\
  const vkm_ubvec3*: vkm_ubvec3_magnitude,\
  vkm_svec3*: vkm_svec3_magnitude,\
  const vkm_svec3*: vkm_svec3_magnitude,\
  vkm_usvec3*: vkm_usvec3_magnitude,\
  const vkm_usvec3*: vkm_usvec3_magnitude,\
  vkm_ivec3*: vkm_ivec3_magnitude,\
  const vkm_ivec3*: vkm_ivec3_magnitude,\
  vkm_uvec3*: vkm_uvec3_magnitude,\
  const vkm_uvec3*: vkm_uvec3_magnitude,\
  vkm_lvec3*: vkm_lvec3_magnitude,\
  const vkm_lvec3*: vkm_lvec3_magnitude,\
  vkm_ulvec3*: vkm_ulvec3_magnitude,\
  const vkm_ulvec3*: vkm_ulvec3_magnitude,\
  vkm_vec3*: vkm_vec3_magnitude,\
  const vkm_vec3*: vkm_vec3_magnitude,\
  vkm_dvec3*: vkm_dvec3_magnitude,\
  const vkm_dvec3*: vkm_dvec3_magnitude,\
  vkm_bvec4*: vkm_bvec4_magnitude,\
  const vkm_bvec4*: vkm_bvec4_magnitude,\
  vkm_ubvec4*: vkm_ubvec4_magnitude,\
  const vkm_ubvec4*: vkm_ubvec4_magnitude,\
  vkm_svec4*: vkm_svec4_magnitude,\
  const vkm_svec4*: vkm_svec4_magnitude,\
  vkm_usvec4*: vkm_usvec4_magnitude,\
  const vkm_usvec4*: vkm_usvec4_magnitude,\
  vkm_ivec4*: vkm_ivec4_magnitude,\
  const vkm_ivec4*: vkm_ivec4_magnitude,\
  vkm_uvec4*: vkm_uvec4_magnitude,\
  const vkm_uvec4*: vkm_uvec4_magnitude,\
  vkm_lvec4*: vkm_lvec4_magnitude,\
  const vkm_lvec4*: vkm_lvec4_magnitude,\
  vkm_ulvec4*: vkm_ulvec4_magnitude,\
  const vkm_ulvec4*: vkm_ulvec4_magnitude,\
  vkm_vec4*: vkm_vec4_magnitude,\
  const vkm_vec4*: vkm_vec4_magnitude,\
  vkm_dvec4*: vkm_dvec4_magnitude,\
  const vkm_dvec4*: vkm_dvec4_magnitude\
)(vec)

#define vkm_normalize(vec, result) _Generic((result),\
  vkm_vec2*: vkm_vec2_normalize,\
  vkm_dvec2*: vkm_dvec2_normalize,\
  vkm_vec3*: vkm_vec3_normalize,\
  vkm_dvec3*: vkm_dvec3_normalize,\
  vkm_vec4*: vkm_vec4_normalize,\
  vkm_dvec4*: vkm_dvec4_normalize\
)((vec), (result))

#define vkm_clear(vec) _Generic((vec),\
  vkm_bvec2*: vkm_bvec2_clear,\
  vkm_ubvec2*: vkm_ubvec2_clear,\
  vkm_svec2*: vkm_svec2_clear,\
  vkm_usvec2*: vkm_usvec2_clear,\
  vkm_ivec2*: vkm_ivec2_clear,\
  vkm_uvec2*: vkm_uvec2_clear,\
  vkm_lvec2*: vkm_lvec2_clear,\
  vkm_ulvec2*: vkm_ulvec2_clear,\
  vkm_vec2*: vkm_vec2_clear,\
  vkm_dvec2*: vkm_dvec2_clear,\
  vkm_bvec3*: vkm_bvec3_clear,\
  vkm_ubvec3*: vkm_ubvec3_clear,\
  vkm_svec3*: vkm_svec3_clear,\
  vkm_usvec3*: vkm_usvec3_clear,\
  vkm_ivec3*: vkm_ivec3_clear,\
  vkm_uvec3*: vkm_uvec3_clear,\
  vkm_lvec3*: vkm_lvec3_clear,\
  vkm_ulvec3*: vkm_ulvec3_clear,\
  vkm_vec3*: vkm_vec3_clear,\
  vkm_dvec3*: vkm_dvec3_clear,\
  vkm_bvec4*: vkm_bvec4_clear,\
  vkm_ubvec4*: vkm_ubvec4_clear,\
  vkm_svec4*: vkm_svec4_clear,\
  vkm_usvec4*: vkm_usvec4_clear,\
  vkm_ivec4*: vkm_ivec4_clear,\
  vkm_uvec4*: vkm_uvec4_clear,\
  vkm_lvec4*: vkm_lvec4_clear,\
  vkm_ulvec4*: vkm_ulvec4_clear,\
  vkm_vec4*: vkm_vec4_clear,\
  vkm_dvec4*: vkm_dvec4_clear\
)(vec)

#define vkm_invert(vec, result) _Generic((result),\
  vkm_bvec2*: vkm_bvec2_invert,\
  vkm_svec2*: vkm_svec2_invert,\
  vkm_ivec2*: vkm_ivec2_invert,\
  vkm_lvec2*: vkm_lvec2_invert,\
  vkm_vec2*: vkm_vec2_invert,\
  vkm_dvec2*: vkm_dvec2_invert,\
  vkm_bvec3*: vkm_bvec3_invert,\
  vkm_svec3*: vkm_svec3_invert,\
  vkm_ivec3*: vkm_ivec3_invert,\
  vkm_lvec3*: vkm_lvec3_invert,\
  vkm_vec3*: vkm_vec3_invert,\
  vkm_dvec3*: vkm_dvec3_invert,\
  vkm_bvec4*: vkm_bvec4_invert,\
  vkm_svec4*: vkm_svec4_invert,\
  vkm_ivec4*: vkm_ivec4_invert,\
  vkm_lvec4*: vkm_lvec4_invert,\
  vkm_vec4*: vkm_vec4_invert,\
  vkm_dvec4*: vkm_dvec4_invert\
)((vec), (result))

#define CVKM_VEC2_LOGICAL_OPERATION(type, operation, operator) static bool vkm_##type##_##operation(\
const vkm_##type* a,\
const vkm_##type* b\
) {\
  return a->x operator b->x && a->y operator b->y;\
}

#define CVKM_VEC2_LOGICAL_OPERATIONS(type) CVKM_VEC2_LOGICAL_OPERATION(type, eq, ==)\
CVKM_VEC2_LOGICAL_OPERATION(type, lt, <)\
CVKM_VEC2_LOGICAL_OPERATION(type, gt, >)\
CVKM_VEC2_LOGICAL_OPERATION(type, le, <=)\
CVKM_VEC2_LOGICAL_OPERATION(type, ge, >=)

CVKM_VEC2_LOGICAL_OPERATIONS(bvec2)
CVKM_VEC2_LOGICAL_OPERATIONS(ubvec2)
CVKM_VEC2_LOGICAL_OPERATIONS(svec2)
CVKM_VEC2_LOGICAL_OPERATIONS(usvec2)
CVKM_VEC2_LOGICAL_OPERATIONS(ivec2)
CVKM_VEC2_LOGICAL_OPERATIONS(uvec2)
CVKM_VEC2_LOGICAL_OPERATIONS(lvec2)
CVKM_VEC2_LOGICAL_OPERATIONS(ulvec2)
CVKM_VEC2_LOGICAL_OPERATIONS(vec2)
CVKM_VEC2_LOGICAL_OPERATIONS(dvec2)

#define CVKM_VEC3_LOGICAL_OPERATION(type, operation, operator) static bool vkm_##type##_##operation(\
  const vkm_##type* a,\
  const vkm_##type* b\
) {\
  return a->x operator b->x && a->y operator b->y && a->z operator b->z;\
}

#define CVKM_VEC3_LOGICAL_OPERATIONS(type) CVKM_VEC3_LOGICAL_OPERATION(type, eq, ==)\
  CVKM_VEC3_LOGICAL_OPERATION(type, lt, <)\
  CVKM_VEC3_LOGICAL_OPERATION(type, gt, >)\
  CVKM_VEC3_LOGICAL_OPERATION(type, le, <=)\
  CVKM_VEC3_LOGICAL_OPERATION(type, ge, >=)

CVKM_VEC3_LOGICAL_OPERATIONS(bvec3)
CVKM_VEC3_LOGICAL_OPERATIONS(ubvec3)
CVKM_VEC3_LOGICAL_OPERATIONS(svec3)
CVKM_VEC3_LOGICAL_OPERATIONS(usvec3)
CVKM_VEC3_LOGICAL_OPERATIONS(ivec3)
CVKM_VEC3_LOGICAL_OPERATIONS(uvec3)
CVKM_VEC3_LOGICAL_OPERATIONS(lvec3)
CVKM_VEC3_LOGICAL_OPERATIONS(ulvec3)
CVKM_VEC3_LOGICAL_OPERATIONS(vec3)
CVKM_VEC3_LOGICAL_OPERATIONS(dvec3)

#define CVKM_VEC4_LOGICAL_OPERATION(type, operation, operator) static bool vkm_##type##_##operation(\
const vkm_##type* a,\
const vkm_##type* b\
) {\
  return a->x operator b->x && a->y operator b->y && a->z operator b->z && a->w operator b->w;\
}

#define CVKM_VEC4_LOGICAL_OPERATIONS(type) CVKM_VEC4_LOGICAL_OPERATION(type, eq, ==)\
CVKM_VEC4_LOGICAL_OPERATION(type, lt, <)\
CVKM_VEC4_LOGICAL_OPERATION(type, gt, >)\
CVKM_VEC4_LOGICAL_OPERATION(type, le, <=)\
CVKM_VEC4_LOGICAL_OPERATION(type, ge, >=)

CVKM_VEC4_LOGICAL_OPERATIONS(bvec4)
CVKM_VEC4_LOGICAL_OPERATIONS(ubvec4)
CVKM_VEC4_LOGICAL_OPERATIONS(svec4)
CVKM_VEC4_LOGICAL_OPERATIONS(usvec4)
CVKM_VEC4_LOGICAL_OPERATIONS(ivec4)
CVKM_VEC4_LOGICAL_OPERATIONS(uvec4)
CVKM_VEC4_LOGICAL_OPERATIONS(lvec4)
CVKM_VEC4_LOGICAL_OPERATIONS(ulvec4)
CVKM_VEC4_LOGICAL_OPERATIONS(vec4)
CVKM_VEC4_LOGICAL_OPERATIONS(dvec4)

#define vkm_eq(a, b) _Generic((a),\
  vkm_bvec2*: vkm_bvec2_eq,\
  vkm_ubvec2*: vkm_ubvec2_eq,\
  vkm_svec2*: vkm_svec2_eq,\
  vkm_usvec2*: vkm_usvec2_eq,\
  vkm_ivec2*: vkm_ivec2_eq,\
  vkm_uvec2*: vkm_uvec2_eq,\
  vkm_lvec2*: vkm_lvec2_eq,\
  vkm_ulvec2*: vkm_ulvec2_eq,\
  vkm_vec2*: vkm_vec2_eq,\
  vkm_dvec2*: vkm_dvec2_eq,\
  vkm_bvec3*: vkm_bvec3_eq,\
  vkm_ubvec3*: vkm_ubvec3_eq,\
  vkm_svec3*: vkm_svec3_eq,\
  vkm_usvec3*: vkm_usvec3_eq,\
  vkm_ivec3*: vkm_ivec3_eq,\
  vkm_uvec3*: vkm_uvec3_eq,\
  vkm_lvec3*: vkm_lvec3_eq,\
  vkm_ulvec3*: vkm_ulvec3_eq,\
  vkm_vec3*: vkm_vec3_eq,\
  vkm_dvec3*: vkm_dvec3_eq,\
  vkm_bvec4*: vkm_bvec4_eq,\
  vkm_ubvec4*: vkm_ubvec4_eq,\
  vkm_svec4*: vkm_svec4_eq,\
  vkm_usvec4*: vkm_usvec4_eq,\
  vkm_ivec4*: vkm_ivec4_eq,\
  vkm_uvec4*: vkm_uvec4_eq,\
  vkm_lvec4*: vkm_lvec4_eq,\
  vkm_ulvec4*: vkm_ulvec4_eq,\
  vkm_vec4*: vkm_vec4_eq,\
  vkm_dvec4*: vkm_dvec4_eq\
)((a), (b))

#define vkm_lt(a, b) _Generic((a),\
  vkm_bvec2*: vkm_bvec2_lt,\
  vkm_ubvec2*: vkm_ubvec2_lt,\
  vkm_svec2*: vkm_svec2_lt,\
  vkm_usvec2*: vkm_usvec2_lt,\
  vkm_ivec2*: vkm_ivec2_lt,\
  vkm_uvec2*: vkm_uvec2_lt,\
  vkm_lvec2*: vkm_lvec2_lt,\
  vkm_ulvec2*: vkm_ulvec2_lt,\
  vkm_vec2*: vkm_vec2_lt,\
  vkm_dvec2*: vkm_dvec2_lt,\
  vkm_bvec3*: vkm_bvec3_lt,\
  vkm_ubvec3*: vkm_ubvec3_lt,\
  vkm_svec3*: vkm_svec3_lt,\
  vkm_usvec3*: vkm_usvec3_lt,\
  vkm_ivec3*: vkm_ivec3_lt,\
  vkm_uvec3*: vkm_uvec3_lt,\
  vkm_lvec3*: vkm_lvec3_lt,\
  vkm_ulvec3*: vkm_ulvec3_lt,\
  vkm_vec3*: vkm_vec3_lt,\
  vkm_dvec3*: vkm_dvec3_lt,\
  vkm_bvec4*: vkm_bvec4_lt,\
  vkm_ubvec4*: vkm_ubvec4_lt,\
  vkm_svec4*: vkm_svec4_lt,\
  vkm_usvec4*: vkm_usvec4_lt,\
  vkm_ivec4*: vkm_ivec4_lt,\
  vkm_uvec4*: vkm_uvec4_lt,\
  vkm_lvec4*: vkm_lvec4_lt,\
  vkm_ulvec4*: vkm_ulvec4_lt,\
  vkm_vec4*: vkm_vec4_lt,\
  vkm_dvec4*: vkm_dvec4_lt\
)((a), (b))

#define vkm_gt(a, b) _Generic((a),\
  vkm_bvec2*: vkm_bvec2_gt,\
  vkm_ubvec2*: vkm_ubvec2_gt,\
  vkm_svec2*: vkm_svec2_gt,\
  vkm_usvec2*: vkm_usvec2_gt,\
  vkm_ivec2*: vkm_ivec2_gt,\
  vkm_uvec2*: vkm_uvec2_gt,\
  vkm_lvec2*: vkm_lvec2_gt,\
  vkm_ulvec2*: vkm_ulvec2_gt,\
  vkm_vec2*: vkm_vec2_gt,\
  vkm_dvec2*: vkm_dvec2_gt,\
  vkm_bvec3*: vkm_bvec3_gt,\
  vkm_ubvec3*: vkm_ubvec3_gt,\
  vkm_svec3*: vkm_svec3_gt,\
  vkm_usvec3*: vkm_usvec3_gt,\
  vkm_ivec3*: vkm_ivec3_gt,\
  vkm_uvec3*: vkm_uvec3_gt,\
  vkm_lvec3*: vkm_lvec3_gt,\
  vkm_ulvec3*: vkm_ulvec3_gt,\
  vkm_vec3*: vkm_vec3_gt,\
  vkm_dvec3*: vkm_dvec3_gt,\
  vkm_bvec4*: vkm_bvec4_gt,\
  vkm_ubvec4*: vkm_ubvec4_gt,\
  vkm_svec4*: vkm_svec4_gt,\
  vkm_usvec4*: vkm_usvec4_gt,\
  vkm_ivec4*: vkm_ivec4_gt,\
  vkm_uvec4*: vkm_uvec4_gt,\
  vkm_lvec4*: vkm_lvec4_gt,\
  vkm_ulvec4*: vkm_ulvec4_gt,\
  vkm_vec4*: vkm_vec4_gt,\
  vkm_dvec4*: vkm_dvec4_gt\
)((a), (b))

#define vkm_le(a, b) _Generic((a),\
  vkm_bvec2*: vkm_bvec2_le,\
  vkm_ubvec2*: vkm_ubvec2_le,\
  vkm_svec2*: vkm_svec2_le,\
  vkm_usvec2*: vkm_usvec2_le,\
  vkm_ivec2*: vkm_ivec2_le,\
  vkm_uvec2*: vkm_uvec2_le,\
  vkm_lvec2*: vkm_lvec2_le,\
  vkm_ulvec2*: vkm_ulvec2_le,\
  vkm_vec2*: vkm_vec2_le,\
  vkm_dvec2*: vkm_dvec2_le,\
  vkm_bvec3*: vkm_bvec3_le,\
  vkm_ubvec3*: vkm_ubvec3_le,\
  vkm_svec3*: vkm_svec3_le,\
  vkm_usvec3*: vkm_usvec3_le,\
  vkm_ivec3*: vkm_ivec3_le,\
  vkm_uvec3*: vkm_uvec3_le,\
  vkm_lvec3*: vkm_lvec3_le,\
  vkm_ulvec3*: vkm_ulvec3_le,\
  vkm_vec3*: vkm_vec3_le,\
  vkm_dvec3*: vkm_dvec3_le,\
  vkm_bvec4*: vkm_bvec4_le,\
  vkm_ubvec4*: vkm_ubvec4_le,\
  vkm_svec4*: vkm_svec4_le,\
  vkm_usvec4*: vkm_usvec4_le,\
  vkm_ivec4*: vkm_ivec4_le,\
  vkm_uvec4*: vkm_uvec4_le,\
  vkm_lvec4*: vkm_lvec4_le,\
  vkm_ulvec4*: vkm_ulvec4_le,\
  vkm_vec4*: vkm_vec4_le,\
  vkm_dvec4*: vkm_dvec4_le\
)((a), (b))

#define vkm_ge(a, b) _Generic((a),\
  vkm_bvec2*: vkm_bvec2_ge,\
  vkm_ubvec2*: vkm_ubvec2_ge,\
  vkm_svec2*: vkm_svec2_ge,\
  vkm_usvec2*: vkm_usvec2_ge,\
  vkm_ivec2*: vkm_ivec2_ge,\
  vkm_uvec2*: vkm_uvec2_ge,\
  vkm_lvec2*: vkm_lvec2_ge,\
  vkm_ulvec2*: vkm_ulvec2_ge,\
  vkm_vec2*: vkm_vec2_ge,\
  vkm_dvec2*: vkm_dvec2_ge,\
  vkm_bvec3*: vkm_bvec3_ge,\
  vkm_ubvec3*: vkm_ubvec3_ge,\
  vkm_svec3*: vkm_svec3_ge,\
  vkm_usvec3*: vkm_usvec3_ge,\
  vkm_ivec3*: vkm_ivec3_ge,\
  vkm_uvec3*: vkm_uvec3_ge,\
  vkm_lvec3*: vkm_lvec3_ge,\
  vkm_ulvec3*: vkm_ulvec3_ge,\
  vkm_vec3*: vkm_vec3_ge,\
  vkm_dvec3*: vkm_dvec3_ge,\
  vkm_bvec4*: vkm_bvec4_ge,\
  vkm_ubvec4*: vkm_ubvec4_ge,\
  vkm_svec4*: vkm_svec4_ge,\
  vkm_usvec4*: vkm_usvec4_ge,\
  vkm_ivec4*: vkm_ivec4_ge,\
  vkm_uvec4*: vkm_uvec4_ge,\
  vkm_lvec4*: vkm_lvec4_ge,\
  vkm_ulvec4*: vkm_ulvec4_ge,\
  vkm_vec4*: vkm_vec4_ge,\
  vkm_dvec4*: vkm_dvec4_ge\
)((a), (b))

static void vkm_mat4_ortho_lh_zo(
  const float left,
  const float right,
  const float bottom,
  const float top,
  const float near_z,
  const float far_z,
  vkm_mat4* result
) {
  const float x_reciprocal = 1.0f / (right - left);
  const float y_reciprocal = 1.0f / (top - bottom);
  const float z_reciprocal = 1.0f / (far_z - near_z);

  // @formatter:off
  *result = (vkm_mat4){ .raw = {
    2.0f * x_reciprocal,            0.0f,                           0.0f,                   0.0f,
    0.0f,                           2.0f * y_reciprocal,            0.0f,                   0.0f,
    0.0f,                           0.0f,                           z_reciprocal,           0.0f,
    -(right + left) * x_reciprocal, -(top + bottom) * y_reciprocal, near_z * -z_reciprocal, 1.0f,
  }};
  // @formatter:on
}

static void vkm_ortho_lh_no(
  const float left,
  const float right,
  const float bottom,
  const float top,
  const float near_z,
  const float far_z,
  vkm_mat4* result
) {
  const float x_reciprocal = 1.0f / (right - left);
  const float y_reciprocal = 1.0f / (top - bottom);
  const float z_reciprocal = 1.0f / (far_z - near_z);

  // @formatter:off
  *result = (vkm_mat4){ .raw = {
    2.0f * x_reciprocal,            0.0f,                           0.0f,                             0.0f,
    0.0f,                           2.0f * y_reciprocal,            0.0f,                             0.0f,
    0.0f,                           0.0f,                           2.0f * z_reciprocal,              0.0f,
    -(right + left) * x_reciprocal, -(top + bottom) * y_reciprocal, (far_z + near_z) * -z_reciprocal, 1.0f,
  }};
  // @formatter:on
}

static void vkm_ortho_rh_zo(
  const float left,
  const float right,
  const float bottom,
  const float top,
  const float near_z,
  const float far_z,
  vkm_mat4* result
) {
  const float x_reciprocal = 1.0f / (right - left);
  const float y_reciprocal = 1.0f / (top - bottom);
  const float z_reciprocal = -1.0f / (far_z - near_z);

  // @formatter:off
  *result = (vkm_mat4){ .raw = {
    2.0f * x_reciprocal,            0.0f,                           0.0f,                  0.0f,
    0.0f,                           2.0f * y_reciprocal,            0.0f,                  0.0f,
    0.0f,                           0.0f,                           z_reciprocal,          0.0f,
    -(right + left) * x_reciprocal, -(top + bottom) * y_reciprocal, near_z * z_reciprocal, 1.0f,
  }};
  // @formatter:on
}

static void vkm_ortho_rh_no(
  const float left,
  const float right,
  const float bottom,
  const float top,
  const float near_z,
  const float far_z,
  vkm_mat4* result
) {
  const float x_reciprocal = 1.0f / (right - left);
  const float y_reciprocal = 1.0f / (top - bottom);
  const float z_reciprocal = -1.0f / (far_z - near_z);

  // @formatter:off
  *result = (vkm_mat4){ .raw = {
    2.0f * x_reciprocal,            0.0f,                           0.0f,                            0.0f,
    0.0f,                           2.0f * y_reciprocal,            0.0f,                            0.0f,
    0.0f,                           0.0f,                           2.0f * z_reciprocal,             0.0f,
    -(right + left) * x_reciprocal, -(top + bottom) * y_reciprocal, (far_z + near_z) * z_reciprocal, 1.0f,
  }};
  // @formatter:on
}

static void vkm_mat4_mul(const vkm_mat4* a, const vkm_mat4* b, vkm_mat4* result) {
  result->m00 = a->m00 * b->m00 + a->m10 * b->m01 + a->m20 * b->m02 + a->m30 * b->m03;
  result->m01 = a->m01 * b->m00 + a->m11 * b->m01 + a->m21 * b->m02 + a->m31 * b->m03;
  result->m02 = a->m02 * b->m00 + a->m12 * b->m01 + a->m22 * b->m02 + a->m32 * b->m03;
  result->m03 = a->m03 * b->m00 + a->m13 * b->m01 + a->m23 * b->m02 + a->m33 * b->m03;

  result->m10 = a->m00 * b->m10 + a->m10 * b->m11 + a->m20 * b->m12 + a->m30 * b->m13;
  result->m11 = a->m01 * b->m10 + a->m11 * b->m11 + a->m21 * b->m12 + a->m31 * b->m13;
  result->m12 = a->m02 * b->m10 + a->m12 * b->m11 + a->m22 * b->m12 + a->m32 * b->m13;
  result->m13 = a->m03 * b->m10 + a->m13 * b->m11 + a->m23 * b->m12 + a->m33 * b->m13;

  result->m20 = a->m00 * b->m20 + a->m10 * b->m21 + a->m20 * b->m22 + a->m30 * b->m23;
  result->m21 = a->m01 * b->m20 + a->m11 * b->m21 + a->m21 * b->m22 + a->m31 * b->m23;
  result->m22 = a->m02 * b->m20 + a->m12 * b->m21 + a->m22 * b->m22 + a->m32 * b->m23;
  result->m23 = a->m03 * b->m20 + a->m13 * b->m21 + a->m23 * b->m22 + a->m33 * b->m23;

  result->m30 = a->m00 * b->m30 + a->m10 * b->m31 + a->m20 * b->m32 + a->m30 * b->m33;
  result->m31 = a->m01 * b->m30 + a->m11 * b->m31 + a->m21 * b->m32 + a->m31 * b->m33;
  result->m32 = a->m02 * b->m30 + a->m12 * b->m31 + a->m22 * b->m32 + a->m32 * b->m33;
  result->m33 = a->m03 * b->m30 + a->m13 * b->m31 + a->m23 * b->m32 + a->m33 * b->m33;
}

static void vkm_mat4_mul_transform(const vkm_mat4* a, const vkm_mat4* b, vkm_mat4* result) {
  result->m00 = a->m00 * b->m00 + a->m10 * b->m01 + a->m20 * b->m02;
  result->m01 = a->m01 * b->m00 + a->m11 * b->m01 + a->m21 * b->m02;
  result->m02 = a->m02 * b->m00 + a->m12 * b->m01 + a->m22 * b->m02;
  result->m03 = a->m03 * b->m00 + a->m13 * b->m01 + a->m23 * b->m02;

  result->m10 = a->m00 * b->m10 + a->m10 * b->m11 + a->m20 * b->m12;
  result->m11 = a->m01 * b->m10 + a->m11 * b->m11 + a->m21 * b->m12;
  result->m12 = a->m02 * b->m10 + a->m12 * b->m11 + a->m22 * b->m12;
  result->m13 = a->m03 * b->m10 + a->m13 * b->m11 + a->m23 * b->m12;

  result->m20 = a->m00 * b->m20 + a->m10 * b->m21 + a->m20 * b->m22;
  result->m21 = a->m01 * b->m20 + a->m11 * b->m21 + a->m21 * b->m22;
  result->m22 = a->m02 * b->m20 + a->m12 * b->m21 + a->m22 * b->m22;
  result->m23 = a->m03 * b->m20 + a->m13 * b->m21 + a->m23 * b->m22;

  result->m30 = a->m00 * b->m30 + a->m10 * b->m31 + a->m20 * b->m32 + a->m30 * b->m33;
  result->m31 = a->m01 * b->m30 + a->m11 * b->m31 + a->m21 * b->m32 + a->m31 * b->m33;
  result->m32 = a->m02 * b->m30 + a->m12 * b->m31 + a->m22 * b->m32 + a->m32 * b->m33;
  result->m33 = a->m03 * b->m30 + a->m13 * b->m31 + a->m23 * b->m32 + a->m33 * b->m33;
}

static void vkm_mat4_mul_rotation(const vkm_mat4* a, const vkm_mat4* b, vkm_mat4* result) {
  result->m00 = a->m00 * b->m00 + a->m10 * b->m01 + a->m20 * b->m02;
  result->m01 = a->m01 * b->m00 + a->m11 * b->m01 + a->m21 * b->m02;
  result->m02 = a->m02 * b->m00 + a->m12 * b->m01 + a->m22 * b->m02;
  result->m03 = a->m03 * b->m00 + a->m13 * b->m01 + a->m23 * b->m02;

  result->m10 = a->m00 * b->m10 + a->m10 * b->m11 + a->m20 * b->m12;
  result->m11 = a->m01 * b->m10 + a->m11 * b->m11 + a->m21 * b->m12;
  result->m12 = a->m02 * b->m10 + a->m12 * b->m11 + a->m22 * b->m12;
  result->m13 = a->m03 * b->m10 + a->m13 * b->m11 + a->m23 * b->m12;

  result->m20 = a->m00 * b->m20 + a->m10 * b->m21 + a->m20 * b->m22;
  result->m21 = a->m01 * b->m20 + a->m11 * b->m21 + a->m21 * b->m22;
  result->m22 = a->m02 * b->m20 + a->m12 * b->m21 + a->m22 * b->m22;
  result->m23 = a->m03 * b->m20 + a->m13 * b->m21 + a->m23 * b->m22;

  result->m30 = a->m30;
  result->m31 = a->m31;
  result->m32 = a->m32;
  result->m33 = a->m33;
}

static void vkm_make_rotation(const float angle, const vkm_vec3* axis, vkm_mat4* result) {
  vkm_vec3 axis_normalized = *axis;
  vkm_normalize(axis, &axis_normalized);
  const float cosine = cosf(angle);
  vkm_vec3 vec_cosine, vec_sine;

  vkm_mul(&axis_normalized, 1.0f - cosine, &vec_cosine);
  vkm_mul(&axis_normalized, sinf(angle), &vec_sine);

  vkm_mul(&axis_normalized, vec_cosine.x, (vkm_vec3*)result->columns->raw);
  vkm_mul(&axis_normalized, vec_cosine.y, (vkm_vec3*)(result->columns->raw + 1));
  vkm_mul(&axis_normalized, vec_cosine.z, (vkm_vec3*)(result->columns->raw + 2));

  // @formatter:off
  result->m00 += cosine;     result->m10 -= vec_sine.z; result->m20 += vec_sine.y;
  result->m01 += vec_sine.z; result->m11 += cosine;     result->m21 -= vec_sine.x;
  result->m02 -= vec_sine.y; result->m12 += vec_sine.x; result->m22 += cosine;

  result->m03 = result->m13 = result->m23 = result->m30 = result->m31 = result->m32 = 0.0f;
  result->m33 = 1.0f;
  // @formatter:on
}

static void vkm_rotate(vkm_mat4* matrix, const float angle, const vkm_vec3* axis) {
  vkm_mat4 rotation;
  vkm_make_rotation(angle, axis, &rotation);
  vkm_mat4_mul_rotation(matrix, &rotation, matrix);
}

#if !defined(CVKM_LH_ZO) && !defined(CVKM_LH_NO) && !defined(CVKM_RH_ZO) && !defined(CVKM_RH_NO)
#define CVKM_RH_ZO
#endif

#ifdef CVKM_LH_ZO
#define vkm_ortho vkm_ortho_lh_zo
#elif defined(CVKM_LH_NO)
#define vkm_ortho vkm_ortho_lh_no
#elif defined(CVKM_RH_ZO)
#define vkm_ortho vkm_ortho_rh_zo
#elif defined(CVKM_RH_NO)
#define vkm_ortho vkm_ortho_rh_no
#endif

typedef vkm_vec2 Position2;
typedef vkm_vec3 Position3;
typedef vkm_vec4 Position4;
typedef vkm_dvec2 DoublePosition2;
typedef vkm_dvec3 DoublePosition3;
typedef vkm_dvec4 DoublePosition4;
typedef vkm_mat4 Transform;
typedef vkm_vec2 Velocity2;
typedef vkm_vec3 Velocity3;
typedef vkm_vec4 Velocity4;
typedef float Mass;
typedef float Damping;
typedef float GravityScale;

#ifdef CVKM_3D
typedef Velocity3 Velocity;

#ifdef CVKM_DOUBLE_PRECISION
typedef DoublePosition3 Position;
#else
typedef Position3 Position;
#endif

#elif CVKM_4D
typedef Velocity4 Velocity;

#ifdef CVKM_DOUBLE_PRECISION
typedef DoublePosition4 Position;
#else
typedef Position4 Position;
#endif

#else
typedef Velocity2 Velocity;

#ifdef CVKM_DOUBLE_PRECISION
typedef DoublePosition2 Position;
#else
typedef Position2 Position;
#endif

#endif

#ifdef CVKM_ENABLE_FLECS
extern ECS_COMPONENT_DECLARE(Position2);
extern ECS_COMPONENT_DECLARE(Position3);
extern ECS_COMPONENT_DECLARE(Position4);
extern ECS_COMPONENT_DECLARE(DoublePosition2);
extern ECS_COMPONENT_DECLARE(DoublePosition3);
extern ECS_COMPONENT_DECLARE(DoublePosition4);
extern ECS_COMPONENT_DECLARE(Position);
extern ECS_COMPONENT_DECLARE(Transform);
extern ECS_COMPONENT_DECLARE(Velocity2);
extern ECS_COMPONENT_DECLARE(Velocity3);
extern ECS_COMPONENT_DECLARE(Velocity4);
extern ECS_COMPONENT_DECLARE(Velocity);
extern ECS_COMPONENT_DECLARE(Mass);
extern ECS_COMPONENT_DECLARE(Damping);
extern ECS_COMPONENT_DECLARE(GravityScale);

void cvkmImport(ecs_world_t* world);

#ifdef CVKM_FLECS_IMPLEMENTATION
ECS_COMPONENT_DECLARE(Position2);
ECS_COMPONENT_DECLARE(Position3);
ECS_COMPONENT_DECLARE(Position4);
ECS_COMPONENT_DECLARE(DoublePosition2);
ECS_COMPONENT_DECLARE(DoublePosition3);
ECS_COMPONENT_DECLARE(DoublePosition4);
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Transform);
ECS_COMPONENT_DECLARE(Velocity2);
ECS_COMPONENT_DECLARE(Velocity3);
ECS_COMPONENT_DECLARE(Velocity4);
ECS_COMPONENT_DECLARE(Velocity);
ECS_COMPONENT_DECLARE(Mass);
ECS_COMPONENT_DECLARE(Damping);
ECS_COMPONENT_DECLARE(GravityScale);

#define CVKM_MEMBER(member, member_type, struct_type, unit_) {\
  .name = #member,\
  .type = ecs_id(member_type),\
  .offset = offsetof(struct_type, member),\
  .unit = unit_,\
}

#define CVKM_VEC2_COMPONENT(struct_type, member_type, unit_) ECS_COMPONENT_DEFINE(world, struct_type);\
  ecs_struct(\
    world,\
    {\
      .entity = ecs_id(struct_type),\
      .members = {\
        CVKM_MEMBER(x, member_type, struct_type, unit_),\
        CVKM_MEMBER(y, member_type, struct_type, unit_),\
      },\
    }\
  )

#define CVKM_VEC3_COMPONENT(struct_type, member_type, unit_) ECS_COMPONENT_DEFINE(world, struct_type);\
  ecs_struct(\
    world,\
    {\
      .entity = ecs_id(struct_type),\
      .members = {\
        CVKM_MEMBER(x, member_type, struct_type, unit_),\
        CVKM_MEMBER(y, member_type, struct_type, unit_),\
        CVKM_MEMBER(z, member_type, struct_type, unit_),\
      },\
    }\
  )

#define CVKM_VEC4_COMPONENT(struct_type, member_type, unit_) ECS_COMPONENT_DEFINE(world, struct_type);\
  ecs_struct(\
    world,\
    {\
      .entity = ecs_id(struct_type),\
      .members = {\
        CVKM_MEMBER(x, member_type, struct_type, unit_),\
        CVKM_MEMBER(y, member_type, struct_type, unit_),\
        CVKM_MEMBER(z, member_type, struct_type, unit_),\
        CVKM_MEMBER(w, member_type, struct_type, unit_),\
      },\
    }\
  )

#define CVKM_MAT4_COMPONENT(struct_type, member_type) ECS_COMPONENT_DEFINE(world, struct_type);\
  ecs_struct(\
    world,\
    {\
      .entity = ecs_id(struct_type),\
      .members = {\
        CVKM_MEMBER(m00, member_type, struct_type, 0),\
        CVKM_MEMBER(m01, member_type, struct_type, 0),\
        CVKM_MEMBER(m02, member_type, struct_type, 0),\
        CVKM_MEMBER(m03, member_type, struct_type, 0),\
        CVKM_MEMBER(m10, member_type, struct_type, 0),\
        CVKM_MEMBER(m11, member_type, struct_type, 0),\
        CVKM_MEMBER(m12, member_type, struct_type, 0),\
        CVKM_MEMBER(m13, member_type, struct_type, 0),\
        CVKM_MEMBER(m20, member_type, struct_type, 0),\
        CVKM_MEMBER(m21, member_type, struct_type, 0),\
        CVKM_MEMBER(m22, member_type, struct_type, 0),\
        CVKM_MEMBER(m23, member_type, struct_type, 0),\
        CVKM_MEMBER(m30, member_type, struct_type, 0),\
        CVKM_MEMBER(m31, member_type, struct_type, 0),\
        CVKM_MEMBER(m32, member_type, struct_type, 0),\
        CVKM_MEMBER(m33, member_type, struct_type, 0),\
      },\
    }\
  )

void cvkmImport(ecs_world_t* world) {
  ECS_MODULE(world, cvkm);

  ECS_IMPORT(world, FlecsUnits);

  CVKM_VEC2_COMPONENT(Position2, ecs_f32_t, EcsMeters);
  CVKM_VEC3_COMPONENT(Position3, ecs_f32_t, EcsMeters);
  CVKM_VEC3_COMPONENT(Position4, ecs_f32_t, EcsMeters);
  CVKM_VEC2_COMPONENT(DoublePosition2, ecs_f64_t, EcsMeters);
  CVKM_VEC3_COMPONENT(DoublePosition3, ecs_f64_t, EcsMeters);
  CVKM_VEC3_COMPONENT(DoublePosition4, ecs_f64_t, EcsMeters);
  CVKM_MAT4_COMPONENT(Transform, ecs_f32_t);
  CVKM_VEC2_COMPONENT(Velocity2, ecs_f32_t, EcsMetersPerSecond);
  CVKM_VEC2_COMPONENT(Velocity3, ecs_f32_t, EcsMetersPerSecond);
  CVKM_VEC2_COMPONENT(Velocity4, ecs_f32_t, EcsMetersPerSecond);
  ECS_COMPONENT_DEFINE(world, Mass);
  ecs_add_pair(world, ecs_id(Mass), EcsIsA, EcsKiloGrams);
  ECS_COMPONENT_DEFINE(world, Damping);
  ecs_primitive(world, { .entity = ecs_id(Damping), .kind = EcsF32 });
  ECS_COMPONENT_DEFINE(world, GravityScale);
  ecs_primitive(world, { .entity = ecs_id(GravityScale), .kind = EcsF32 });

#ifdef CVKM_3D
  CVKM_VEC3_COMPONENT(Velocity, ecs_f32_t, EcsMetersPerSecond);

#ifdef CVKM_DOUBLE_PRECISION
  CVKM_VEC3_COMPONENT(Position, ecs_f64_t, EcsMeters);
#else
  CVKM_VEC3_COMPONENT(Position, ecs_f32_t, EcsMeters);
#endif

#elif CVKM_4D
  CVKM_VEC4_COMPONENT(Velocity, ecs_f32_t, EcsMetersPerSecond);

#ifdef CVKM_DOUBLE_PRECISION
  CVKM_VEC4_COMPONENT(Position, ecs_f64_t, EcsMeters);
#else
  CVKM_VEC4_COMPONENT(Position, ecs_f32_t, EcsMeters);
#endif

#else
  CVKM_VEC2_COMPONENT(Velocity, ecs_f32_t, EcsMetersPerSecond);

#ifdef CVKM_DOUBLE_PRECISION
  CVKM_VEC2_COMPONENT(Position, ecs_f64_t, EcsMeters);
#else
  CVKM_VEC2_COMPONENT(Position, ecs_f32_t, EcsMeters);
#endif

#endif
}

#undef CVKM_FLECS_IMPLEMENTATION
#endif
#endif

IGNORE_WARNINGS_END
#endif
