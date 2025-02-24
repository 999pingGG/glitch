#pragma once
#ifndef _GLITCH_H_
#define _GLITCH_H_
#include <cvkm.h>
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

extern ECS_COMPONENT_DECLARE(MeshData);
extern ECS_COMPONENT_DECLARE(Mesh);
extern ECS_COMPONENT_DECLARE(ShaderProgramSource);
extern ECS_COMPONENT_DECLARE(ShaderProgram);
extern ECS_TAG_DECLARE(Uses);

void GLitchImport(ecs_world_t* world);
#endif
