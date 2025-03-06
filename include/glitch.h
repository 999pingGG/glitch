#pragma once
#ifndef _GLITCH_H_
#define _GLITCH_H_
#define CVKM_NO
#include <cvkm.h>

#ifdef _WIN32
#define GLI_WINDOWS
#endif

#ifdef __linux__
#define GLI_LINUX
#endif

#ifdef GLI_WINDOWS
// Window's OpenGL header is ancient (version 1.1).
// So we need to provide some stuff here by ourselves.
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>

typedef char GLchar;
typedef intptr_t GLsizeiptr;
typedef intptr_t GLintptr;

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x0001

#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_STATIC_DRAW 0x88E4
#define GL_LINK_STATUS 0x8B82
#define GL_COMPILE_STATUS 0x8B81
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_STREAM_DRAW 0x88E0
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_UNSIGNED_INT_VEC2 0x8DC6
#define GL_UNSIGNED_INT_VEC3 0x8DC7
#define GL_UNSIGNED_INT_VEC4 0x8DC8
#define GL_FLOAT_MAT4 0x8B5C
#endif

#include <GL/gl.h>

#define GLI_COUNTOF(array) (sizeof(array) / sizeof(array[0]))
#define GLI_MAX_ATTRIBUTES 16
#define GLI_MAX_UNIFORMS 25

typedef enum gli_data_type_t {
  GLI_BYTE = 1,
  GLI_UBYTE,
  GLI_SHORT,
  GLI_USHORT,
  GLI_INT,
  GLI_UINT,
  GLI_FLOAT,
  GLI_BVEC2,
  GLI_UBVEC2,
  GLI_SVEC2,
  GLI_USVEC2,
  GLI_IVEC2,
  GLI_UVEC2,
  GLI_VEC2,
  GLI_BVEC3,
  GLI_UBVEC3,
  GLI_SVEC3,
  GLI_USVEC3,
  GLI_IVEC3,
  GLI_UVEC3,
  GLI_VEC3,
  GLI_BVEC4,
  GLI_UBVEC4,
  GLI_SVEC4,
  GLI_USVEC4,
  GLI_IVEC4,
  GLI_UVEC4,
  GLI_VEC4,
  GLI_MAT4,
  GLI_DATA_TYPE_MAX,
} gli_data_type_t;

typedef struct MeshData {
  // This memory is owned by this component.
  void* data;
  unsigned* indices;
  int vertices_count, indices_count;
  // Zero-terminated array, with count up to GLI_MAX_ATTRIBUTES.
  struct attribute {
    // The type is actually gli_data_type_t
    int8_t type;
    bool convert_to_float;
    bool is_normalized;
  } vertex_attributes[GLI_MAX_ATTRIBUTES];
} MeshData;

typedef struct Mesh {
  GLuint vertex_buffer, index_buffer, vertex_array;
  int vertices_count, indices_count;
} Mesh;

typedef struct ShaderProgramSource {
  // Those buffers are owned by this component.
  char* vertex_shader, *fragment_shader;
} ShaderProgramSource;

typedef struct gli_shader_input_data {
  char* name;
  GLint location;
  GLenum type;
} gli_shader_input_data;

typedef struct ShaderProgram {
  ecs_query_t* rendered_entities_query;
  gli_shader_input_data* uniforms, *attributes;
  int uniforms_count, attributes_count;
  GLuint program;
  // The type of this one is actually gli_data_type_t
  uint8_t ecs_uniform_types[GLI_MAX_UNIFORMS];
} ShaderProgram;

typedef struct Camera2D {
  Position2D position;
  vkm_mat4 view, projection;
  float zoom;
} Camera2D;

typedef struct Camera3D {
  Position3D position;
  vkm_mat4 view, projection;
  float field_of_view, near_plane, far_plane;
} Camera3D;

typedef vkm_vec4 Color;
typedef Color ClearColor;

extern ECS_COMPONENT_DECLARE(MeshData);
extern ECS_COMPONENT_DECLARE(Mesh);
extern ECS_COMPONENT_DECLARE(ShaderProgramSource);
extern ECS_COMPONENT_DECLARE(ShaderProgram);
extern ECS_COMPONENT_DECLARE(Camera2D);
extern ECS_COMPONENT_DECLARE(Camera3D);
extern ECS_COMPONENT_DECLARE(Color);
extern ECS_COMPONENT_DECLARE(ClearColor);

extern ECS_TAG_DECLARE(Uses);

void glitchImport(ecs_world_t* world);
#endif
