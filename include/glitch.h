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

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_STATIC_DRAW 0x88E4
#define GL_LINK_STATUS 0x8B82
#define GL_COMPILE_STATUS 0x8B81
#define GL_ARRAY_BUFFER 0x8892
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_ACTIVE_UNIFORMS 0x8B86
#endif

#include <GL/gl.h>

#define GLI_COUNTOF(array) (sizeof(array) / sizeof(array[0]))

typedef struct MeshData {
  // Currently, only a single vec2 vertex attribute is supported.
  const vkm_vec2* vertices;
  int vertices_count;
} MeshData;

typedef struct Mesh {
  GLuint vertex_buffer_object, vertex_array_object;
  int vertices_count;
} Mesh;

typedef struct ShaderProgramSource {
  const char *vertex_shader, *fragment_shader;
} ShaderProgramSource;

typedef struct gli_uniform {
  char* name;
  GLint location;
} gli_uniform;

typedef struct ShaderProgram {
  gli_uniform* uniforms;
  int uniforms_count;
  GLuint program;
} ShaderProgram;

typedef struct Camera2D {
  Position2D position;
  float zoom;
} Camera2D;

typedef struct Camera3D {
  Position3D position;
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
