// Before anything happens at all, the Windows API needs a crash course in staying out of the way...
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define _CRT_NONSTDC_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define CVKM_NO
#define CVKM_ENABLE_FLECS
#include <cvkm.h>
#include <flecs.h>
#include <glitch.h>

#ifdef GLI_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef GLXContext (*glXCreateContextAttribsARBProc)(
  Display* display,
  GLXFBConfig config,
  GLXContext share_context,
  Bool direct,
  const int* attrib_list
);

static GLXContext context;
static Display* display;
static Window window;
static Atom wm_delete;
#elif defined(GLI_WINDOWS)
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(
  HDC device_context_handle,
  HGLRC gl_rendering_context_handle,
  const int* attribute_list
);

static HGLRC context;
static HWND window_handle;
static HDC device_context_handle;

static LRESULT CALLBACK window_proc(HWND handle, UINT message, WPARAM w_param, LPARAM l_param) {
  switch (message) {
    case WM_CLOSE:
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    default:
      return DefWindowProc(handle, message, w_param, l_param);
  }
}
#else
#error Unsupported platform.
#endif

// Hardcoded for now.
#define GLI_WIDTH 800
#define GLI_HEIGHT 600

typedef GLuint (*glCreateShaderProc)(GLenum shaderType);
static glCreateShaderProc glCreateShader;
typedef void (*glDeleteShaderProc)(GLuint shader);
static glDeleteShaderProc glDeleteShader;
typedef void (*glShaderSourceProc)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
static glShaderSourceProc glShaderSource;
typedef void (*glCompileShaderProc)(GLuint shader);
static glCompileShaderProc glCompileShader;
typedef void (*glGetShaderivProc)(GLuint shader, GLenum pname, GLint* params);
static glGetShaderivProc glGetShaderiv;
typedef void (*glGetShaderInfoLogProc)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
static glGetShaderInfoLogProc glGetShaderInfoLog;
typedef GLuint (*glCreateProgramProc)(void);
static glCreateProgramProc glCreateProgram;
typedef void (*glDeleteProgramProc)(GLuint program);
static glDeleteProgramProc glDeleteProgram;
typedef void (*glAttachShaderProc)(GLuint program, GLuint shader);
static glAttachShaderProc glAttachShader;
typedef void (*glLinkProgramProc)(GLuint program);
static glLinkProgramProc glLinkProgram;
typedef void (*glGetProgramivProc)(GLuint program, GLenum pname, GLint* params);
static glGetProgramivProc glGetProgramiv;
typedef void (*glGetProgramInfoLogProc)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
static glGetProgramInfoLogProc glGetProgramInfoLog;
typedef void (*glGetActiveAttribProc)(
  GLuint program,
  GLuint index,
  GLsizei bufSize,
  GLsizei* length,
  GLint* size,
  GLenum* type,
  GLchar* name
);
static glGetActiveAttribProc glGetActiveAttrib;
typedef void (*glGetActiveUniformProc)(
  GLuint program,
  GLuint index,
  GLsizei bufSize,
  GLsizei* length,
  GLint* size,
  GLenum* type,
  GLchar* name
);
static glGetActiveUniformProc glGetActiveUniform;
typedef GLint (*glGetAttribLocationProc)(GLuint program, const GLchar* name);
static glGetAttribLocationProc glGetAttribLocation;
typedef GLint (*glGetUniformLocationProc)(GLuint program, const GLchar* name);
static glGetUniformLocationProc glGetUniformLocation;
typedef void (*glGenVertexArraysProc)(GLsizei n, GLuint* arrays);
static glGenVertexArraysProc glGenVertexArrays;
typedef void (*glDeleteVertexArraysProc)(GLsizei n, const GLuint *arrays);
static glDeleteVertexArraysProc glDeleteVertexArrays;
typedef void (*glBindVertexArrayProc)(GLuint array);
static glBindVertexArrayProc glBindVertexArray;
typedef void (*glGenBuffersProc)(GLsizei n, GLuint* buffers);
static glGenBuffersProc glGenBuffers;
typedef void (*glDeleteBuffersProc)(GLsizei n, const GLuint *buffers);
static glDeleteBuffersProc glDeleteBuffers;
typedef void (*glBindBufferProc)(GLenum target, GLuint buffer);
static glBindBufferProc glBindBuffer;
typedef void (*glVertexAttribPointerProc)(
  GLuint index,
  GLint size,
  GLenum type,
  GLboolean normalized,
  GLsizei stride,
  const void* pointer
);
static glVertexAttribPointerProc glVertexAttribPointer;
typedef void (*glEnableVertexAttribArrayProc)(GLuint index);
static glEnableVertexAttribArrayProc glEnableVertexAttribArray;
typedef void (*glBufferDataProc)(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
static glBufferDataProc glBufferData;
typedef void (*glUseProgramProc)(GLuint program);
static glUseProgramProc glUseProgram;
typedef void (*glUniformMatrix4fvProc)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
static glUniformMatrix4fvProc glUniformMatrix4fv;

ECS_COMPONENT_DECLARE(MeshData);
ECS_COMPONENT_DECLARE(Mesh);
ECS_COMPONENT_DECLARE(ShaderProgramSource);
ECS_COMPONENT_DECLARE(ShaderProgram);
ECS_COMPONENT_DECLARE(Camera2D);
ECS_COMPONENT_DECLARE(Camera3D);
ECS_COMPONENT_DECLARE(Color);
ECS_COMPONENT_DECLARE(ClearColor);

ECS_TAG_DECLARE(Uses);

ECS_CTOR(MeshData, ptr, {
  *ptr = (MeshData){ 0 };
})

ECS_MOVE(MeshData, dst, src, {
  // free(dst->vertices);
  *dst = *src;
  *src = (MeshData){ 0 };
})

ECS_DTOR(MeshData, ptr, {
  // free(ptr->vertices);
  *ptr = (MeshData){ 0 };
})

ECS_CTOR(Mesh, ptr, {
  *ptr = (Mesh){ 0 };
})

ECS_MOVE(Mesh, dst, src, {
  glDeleteBuffers(1, &dst->vertex_buffer_object);
  glDeleteVertexArrays(1, &dst->vertex_array_object);
  *dst = *src;
  *src = (Mesh){ 0 };
})

ECS_DTOR(Mesh, ptr, {
  glDeleteBuffers(1, &ptr->vertex_buffer_object);
  glDeleteVertexArrays(1, &ptr->vertex_array_object);
  *ptr = (Mesh){ 0 };
})

ECS_CTOR(ShaderProgramSource, ptr, {
  *ptr = (ShaderProgramSource){ 0 };
})

ECS_MOVE(ShaderProgramSource, dst, src, {
  // free(dst->vertex_shader);
  // free(dst->fragment_shader);
  *dst = *src;
  *src = (ShaderProgramSource){ 0 };
})

ECS_DTOR(ShaderProgramSource, ptr, {
  // free(ptr->vertex_shader);
  // free(ptr->fragment_shader);
  *ptr = (ShaderProgramSource){ 0 };
})

ECS_CTOR(ShaderProgram, ptr, {
  *ptr = (ShaderProgram){ 0 };
})

ECS_MOVE(ShaderProgram, dst, src, {
  free(dst->uniforms);
  glDeleteProgram(dst->program);
  *dst = *src;
  *src = (ShaderProgram){ 0 };
})

ECS_DTOR(ShaderProgram, ptr, {
  for (int j = 0; j < ptr->uniforms_count; j++) {
    free(ptr->uniforms[j].name);
  }
  free(ptr->uniforms);
  for (int j = 0; j < ptr->attributes_count; j++) {
    free(ptr->attributes[j].name);
  }
  free(ptr->attributes);
  glDeleteProgram(ptr->program);
  *ptr = (ShaderProgram){ 0 };
})

ECS_CTOR(Camera2D, ptr, {
  *ptr = (Camera2D){
    .position = CVKM_VEC2_ZERO,
    .zoom = 1.0f,
  };
})

ECS_CTOR(Camera3D, ptr, {
  *ptr = (Camera3D){
    .position = CVKM_VEC3_ZERO,
    .field_of_view = 80.0f,
  };
})

ECS_CTOR(Color, ptr, {
  *ptr = (Color){ { 0.0f, 0.0f, 0.0f, 1.0f } };
})

ECS_CTOR(ClearColor, ptr, {
  *ptr = (ClearColor){ { 0.0f, 0.0f, 0.0f, 1.0f } };
})

static GLuint compile_shader(const GLenum type, const char* source) {
  const GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[1000];
    glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
    fprintf(stderr, "Shader compilation failed: %s\n", info_log);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

static void MakeMeshes(ecs_iter_t* it) {
  const MeshData* mesh_datas = ecs_field(it, MeshData, 0);

  for (int i = 0; i < it->count; i++) {
    const MeshData* mesh_data = mesh_datas + i;

    Mesh* mesh = ecs_ensure(it->world, it->entities[i], Mesh);

    mesh->vertices_count = mesh_data->vertices_count;

    glGenVertexArrays(1, &mesh->vertex_array_object);
    glBindVertexArray(mesh->vertex_array_object);

    glGenBuffers(1, &mesh->vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer_object);

    ecs_modified(it->world, it->entities[i], Mesh);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBufferData(
      GL_ARRAY_BUFFER,
      (GLsizeiptr)(mesh_data->vertices_count * sizeof(vkm_vec2)),
      mesh_data->vertices,
      GL_STATIC_DRAW
    );

    ecs_remove(it->world, it->entities[i], MeshData);
  }
}

static void CompileShaders(ecs_iter_t* it) {
  const ShaderProgramSource* sources = ecs_field(it, ShaderProgramSource, 0);

  for (int i = 0; i < it->count; i++) {
    const ShaderProgramSource* source = sources + i;

    GLuint vertex_shader = 0, fragment_shader = 0;
    ShaderProgram shader_program;

    vertex_shader = compile_shader(GL_VERTEX_SHADER, source->vertex_shader);
    fragment_shader = compile_shader(GL_FRAGMENT_SHADER, source->fragment_shader);

    if (!vertex_shader || !fragment_shader) {
      ecs_delete(it->world, it->entities[i]);
      goto cleanup;
    }

    shader_program.program = glCreateProgram();
    glAttachShader(shader_program.program, vertex_shader);
    glAttachShader(shader_program.program, fragment_shader);
    glLinkProgram(shader_program.program);

    GLint success;
    glGetProgramiv(shader_program.program, GL_LINK_STATUS, &success);
    if (!success) {
      char info_log[1000];
      glGetProgramInfoLog(shader_program.program, sizeof(info_log), NULL, info_log);
      fprintf(stderr, "Program linking failed: %s\n", info_log);
      glDeleteProgram(shader_program.program);
      ecs_delete(it->world, it->entities[i]);
      goto cleanup;
    }

    GLint max_length;
    glGetProgramiv(shader_program.program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
    char* name_buffer = malloc(max_length);

    glGetProgramiv(shader_program.program, GL_ACTIVE_UNIFORMS, &shader_program.uniforms_count);
    shader_program.uniforms = malloc(shader_program.uniforms_count * sizeof(gli_shader_input_data));
    for (int j = 0; j < shader_program.uniforms_count; j++) {
      glGetActiveUniform(
        shader_program.program,
        j,
        max_length,
        NULL,
        &shader_program.uniforms[j].size,
        &shader_program.uniforms[j].type,
        name_buffer
      );
      shader_program.uniforms[j].name = strdup(name_buffer);
      shader_program.uniforms[j].location = glGetUniformLocation(shader_program.program, name_buffer);
    }

    free(name_buffer);

    glGetProgramiv(shader_program.program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);
    name_buffer = malloc(max_length);

    glGetProgramiv(shader_program.program, GL_ACTIVE_ATTRIBUTES, &shader_program.attributes_count);
    shader_program.attributes = malloc(shader_program.attributes_count * sizeof(gli_shader_input_data));
    for (int j = 0; j < shader_program.attributes_count; j++) {
      glGetActiveAttrib(
        shader_program.program,
        j,
        max_length,
        NULL,
        &shader_program.attributes[j].size,
        &shader_program.attributes[j].type,
        name_buffer
      );
      shader_program.attributes[j].name = strdup(name_buffer);
      shader_program.attributes[j].location = glGetAttribLocation(shader_program.program, name_buffer);
    }

    free(name_buffer);

    static const int reserved_terms = 5;
    ecs_query_desc_t query_description = {
      .terms = {
        {
          .id = ecs_id(Position2D),
          .inout = EcsIn,
          .oper = EcsOr,
        },
        {
          .id = ecs_id(Position3D),
          .inout = EcsIn,
        },
        {
          .first.id = ecs_id(Uses),
          .second.id = it->entities[i],
          .inout = EcsInOutNone,
        },
        {
          .first.id = ecs_id(Uses),
          .second.name = "$mesh",
          .inout = EcsInOutNone,
        },
        {
          .id = ecs_id(Mesh),
          .src.name = "$mesh",
          .inout = EcsIn,
        },
      },
      .cache_kind = EcsQueryCacheAuto,
    };
    for (
      int j = reserved_terms;
      source->uniform_components[j] && j < (int)GLI_COUNTOF(source->uniform_components) + reserved_terms;
      j++
    ) {
      const int uniform_index = j - reserved_terms;

      query_description.terms[j] = (ecs_term_t){
        .id = source->uniform_components[uniform_index],
        .src = (ecs_term_ref_t){
          .id = source->provided_by_entity[uniform_index] ? 0 : it->entities[i],
        },
        .inout = EcsIn,
      };
    }
    shader_program.rendered_entities_query = ecs_query_init(it->world, &query_description);

    ecs_set_id(it->world, it->entities[i], ecs_id(ShaderProgram), sizeof(ShaderProgram), &shader_program);

    cleanup:
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
  }
}

static GLint get_uniform_location(const ShaderProgram* program, const char* uniform_name) {
  for (int i = 0; i < program->uniforms_count; i++) {
    if (strcmp(uniform_name, program->uniforms[i].name) == 0) {
      return program->uniforms[i].location;
    }
  }

  return -1;
}

static void PreRenderFrame(ecs_iter_t* it) {
  const ClearColor* clear_color = ecs_field(it, ClearColor, 0);
  Camera2D* camera_2d = ecs_field(it, Camera2D, 1);
  Camera3D* camera_3d = ecs_field(it, Camera3D, 2);

  if (camera_2d) {
    // Apply 2D projection.
    const float zoom_factor = 1.0f / camera_2d->zoom;
    const float half_width = GLI_WIDTH * 0.5f * zoom_factor;
    const float half_height = GLI_HEIGHT * 0.5f * zoom_factor;
    vkm_orthogonal(
      -half_width,
      half_width,
      -half_height,
      half_height,
      -1000.f,
      1000.f,
      &camera_2d->view_projection_matrix
    );

    // Apply 2D view.
    vkm_vec2 offset_2d;
    vkm_invert(&camera_2d->position, &offset_2d);
    vkm_translate(&camera_2d->view_projection_matrix, &offset_2d);
  }

  if (camera_3d) {
    // Apply 3D projection.
    vkm_perspective(
      camera_3d->field_of_view,
      (float)GLI_WIDTH / GLI_HEIGHT,
      camera_3d->near_plane,
      camera_3d->far_plane,
      &camera_3d->view_projection_matrix
    );

    // Apply 3D view.
    vkm_vec3 offset_3d;
    vkm_invert(&camera_3d->position, &offset_3d);
    vkm_translate(&camera_3d->view_projection_matrix, &offset_3d);
  }

#ifdef GLI_LINUX
  while (XPending(display)) {
    XEvent event;
    XNextEvent(display, &event);
    switch (event.type) {
      case ClientMessage:
        if ((Atom)event.xclient.data.l[0] == wm_delete) {
          ecs_quit(it->world);
        }
        break;
      default:
        break;
    }
  }
#else
  MSG message;
  while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
    if (message.message == WM_QUIT) {
      ecs_quit(it->world);
      break;
    }
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
#endif

  if (clear_color) {
    glClearColor(clear_color->r, clear_color->g, clear_color->b, clear_color->a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}

static void Render(ecs_iter_t* it) {
  const ShaderProgram* shader_programs = ecs_field(it, ShaderProgram, 0);
  const Camera2D* camera_2d = ecs_field(it, Camera2D, 1);
  const Camera3D* camera_3d = ecs_field(it, Camera3D, 2);

  for (int i = 0; i < it->count; i++) {
    const ShaderProgram* shader_program = shader_programs + i;

    glUseProgram(shader_program->program);
    const GLint view_projection_matrix_location = get_uniform_location(shader_program, "view_projection_matrix");
    const GLint model_matrix_location = get_uniform_location(shader_program, "model_matrix");

    ecs_iter_t rendered_entities_it = ecs_query_iter(it->world, shader_program->rendered_entities_query);
    while (ecs_query_next(&rendered_entities_it)) {
      const bool is_2d = ecs_field_id(&rendered_entities_it, 0) == ecs_id(Position2D);

      // If the shader requires a camera, check that it exists and set the matrix,
      // or skip the rendering of this table of entities completely.
      if (view_projection_matrix_location >= 0) {
        if (is_2d) {
          if (!camera_2d) {
            continue;
          }

          glUniformMatrix4fv(
            view_projection_matrix_location,
            1,
            GL_FALSE,
            (const GLfloat*)camera_2d->view_projection_matrix.raw
          );
        } else {
          if (!camera_3d) {
            continue;
          }

          glUniformMatrix4fv(
            view_projection_matrix_location,
            1,
            GL_FALSE,
            (const GLfloat*)camera_3d->view_projection_matrix.raw
          );
        }
      }

      const void* positions = ecs_table_get_id(
        it->world,
        rendered_entities_it.table,
        is_2d ? ecs_id(Position2D) : ecs_id(Position3D),
        rendered_entities_it.offset
      );
      const Mesh* mesh = ecs_field(&rendered_entities_it, Mesh, 3);
      glBindVertexArray(mesh->vertex_array_object);

      for (int j = 0; j < rendered_entities_it.count; j++) {
        if (model_matrix_location >= 0) {
          vkm_mat4 model_matrix = CVKM_MAT4_IDENTITY;

          if (is_2d) {
            const Position2D* position = (Position2D*)positions + i;
            vkm_translate(&model_matrix, position);
          } else {
            const Position3D* position = (Position3D*)positions + i;
            vkm_translate(&model_matrix, position);
          }

          glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, (const GLfloat*)model_matrix.raw);
        }

        glDrawArrays(GL_TRIANGLES, 0, mesh->vertices_count);
      }
    }
  }
}

static void PostRenderFrame(ecs_iter_t* it) {
  (void)it;
#ifdef GLI_LINUX
  glXSwapBuffers(display, window);
#else
  SwapBuffers(device_context_handle);
#endif

  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR) {
    fprintf(stderr, "OpenGL error 0x%x\n", error);
  }
}

#ifdef GLI_LINUX
#define gli_get_proc_address(fun) glXGetProcAddressARB((const GLubyte*)#fun)
#else
#define gli_get_proc_address(fun) wglGetProcAddress((LPCSTR)#fun)
#endif

#define GLI_LOAD_PROC_ADDRESS(fun) do {\
  fun = (fun##Proc)gli_get_proc_address(fun);\
  if (!fun){\
    fprintf(stderr, "Failed to load "#fun"\n");\
    return;\
  }\
} while (false)

static void fini(ecs_world_t* world, void* unused) {
  (void)world;
  (void)unused;
#ifdef GLI_LINUX
  glXMakeCurrent(display, None, NULL);
  glXDestroyContext(display, context);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
#else
  wglMakeCurrent(NULL, NULL);
  wglDeleteContext(context);
  ReleaseDC(window_handle, device_context_handle);
  DestroyWindow(window_handle);
#endif
}

#ifndef _MSC_VER
#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
#endif
#endif

void glitchImport(ecs_world_t* world) {
  ECS_MODULE(world, glitch);

  ECS_IMPORT(world, cvkm);

  ecs_atfini(world, fini, NULL);

  ECS_COMPONENT_DEFINE(world, MeshData);
  ECS_COMPONENT_DEFINE(world, Mesh);
  ECS_COMPONENT_DEFINE(world, ShaderProgramSource);
  ECS_COMPONENT_DEFINE(world, ShaderProgram);
  ECS_COMPONENT_DEFINE(world, Camera2D);
  ecs_struct(world, {
    .entity = ecs_id(Camera2D),
    .members = {
      {
        .name = "position",
        .type = ecs_id(Position2D),
        .offset = offsetof(Camera2D, position),
      },
      {
        .name = "view_projection_matrix",
        .type = ecs_id(vkm_mat4),
        .offset = offsetof(Camera2D, view_projection_matrix),
      },
      {
        .name = "zoom",
        .type = ecs_id(ecs_f32_t),
        .offset = offsetof(Camera2D, zoom),
      },
    },
  });
  ECS_COMPONENT_DEFINE(world, Camera3D);
  ecs_struct(world, {
    .entity = ecs_id(Camera3D),
    .members = {
      {
        .name = "position",
        .type = ecs_id(Position3D),
        .offset = offsetof(Camera3D, position),
      },
      {
        .name = "view_projection_matrix",
        .type = ecs_id(vkm_mat4),
        .offset = offsetof(Camera3D, view_projection_matrix),
      },
      {
        .name = "field_of_view",
        .type = ecs_id(ecs_f32_t),
        .offset = offsetof(Camera3D, field_of_view),
      },
      {
        .name = "near_plane",
        .type = ecs_id(ecs_f32_t),
        .offset = offsetof(Camera3D, near_plane),
      },
      {
        .name = "far_plane",
        .type = ecs_id(ecs_f32_t),
        .offset = offsetof(Camera3D, far_plane),
      },
    },
  });
  ECS_COMPONENT_DEFINE(world, Color);
  ecs_add_pair(world, ecs_id(Color), EcsIsA, ecs_id(vkm_vec4));
  ECS_COMPONENT_DEFINE(world, ClearColor);
  ecs_add_pair(world, ecs_id(ClearColor), EcsIsA, ecs_id(Color));

  ECS_TAG_DEFINE(world, Uses);

#define GLI_SET_HOOKS(component) ecs_set_hooks(\
  world,\
  component,\
  {\
    .ctor = ecs_ctor(component),\
    .move = ecs_move(component),\
    .dtor = ecs_dtor(component),\
  }\
)

  GLI_SET_HOOKS(MeshData);
  GLI_SET_HOOKS(Mesh);
  GLI_SET_HOOKS(ShaderProgramSource);
  GLI_SET_HOOKS(ShaderProgram);
  ecs_set_hooks(world, Camera2D, { .ctor = ecs_ctor(Camera2D) });
  ecs_set_hooks(world, Camera3D, { .ctor = ecs_ctor(Camera3D) });
  ecs_set_hooks(world, Color, { .ctor = ecs_ctor(Color) });
  ecs_set_hooks(world, ClearColor, { .ctor = ecs_ctor(ClearColor) });

  static const char* window_name = "GLitch";

#ifdef GLI_LINUX
  display = XOpenDisplay(NULL);
  if (!display) {
    fprintf(stderr, "Cannot open display.\n");
    return;
  }

  // Attributes for the framebuffer configuration
  static int visual_attribs[] = {
    GLX_X_RENDERABLE, True,
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE, GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    GLX_RED_SIZE, 8,
    GLX_GREEN_SIZE, 8,
    GLX_BLUE_SIZE, 8,
    GLX_ALPHA_SIZE, 8,
    GLX_DEPTH_SIZE, 24,
    GLX_STENCIL_SIZE, 0,
    GLX_DOUBLEBUFFER, True,
    None
  };

  int framebuffer_count;
  GLXFBConfig* framebuffer_config = glXChooseFBConfig(
    display,
    DefaultScreen(display),
    visual_attribs,
    &framebuffer_count
  );
  if (!framebuffer_config) {
    fprintf(stderr, "Failed to retrieve a framebuffer configuration.\n");
    return;
  }
  // Pick the first suitable framebuffer configuration.
  GLXFBConfig best_config = framebuffer_config[0];
  XFree(framebuffer_config);

  // Get a visual info from the framebuffer configuration.
  XVisualInfo* visual_info = glXGetVisualFromFBConfig(display, best_config);
  if (!visual_info) {
    fprintf(stderr, "No appropriate visual found.\n");
    return;
  }

  // Create a colormap and set window attributes.
  XSetWindowAttributes set_window_attributes = {
    .background_pixmap = None,
    .event_mask = StructureNotifyMask | ExposureMask | KeyPressMask,
    .colormap = XCreateColormap(display, RootWindow(display, visual_info->screen), visual_info->visual, AllocNone),
  };

  window = XCreateWindow(
    display,
    RootWindow(display, visual_info->screen),
    0,
    0,
    GLI_WIDTH,
    GLI_HEIGHT,
    0,
    visual_info->depth,
    InputOutput,
    visual_info->visual,
    CWBorderPixel | CWColormap | CWEventMask,
    &set_window_attributes
  );
  XFree(visual_info);

  if (!window) {
    fprintf(stderr, "Failed to create window.\n");
    return;
  }

  wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wm_delete, 1);

  XStoreName(display, window, window_name);
  XMapWindow(display, window);

  // Get the pointer to glXCreateContextAttribsARB for modern context creation
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
    (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

  if (!glXCreateContextAttribsARB) {
    fprintf(stderr, "Failed to get the address of glXCreateContextAttribsARB.\n");
    return;
  }

  // Create a OpenGL context version 3.3
  context = glXCreateContextAttribsARB(
    display,
    best_config,
    0,
    True,
    (int[]){
      GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
      GLX_CONTEXT_MINOR_VERSION_ARB, 3,
      GLX_CONTEXT_PROFILE_MASK_ARB,
      GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
      None,
    }
  );
  if (!context) {
    fprintf(stderr, "Failed to create OpenGL 3.3 context.\n");
    return;
  }

  // Make the context current
  glXMakeCurrent(display, window, context);
#else
  static const char* class_name = "GLitchWindowClass";

  if (!RegisterClass(
    &(WNDCLASS) {
      .style = CS_OWNDC,
      .lpfnWndProc = window_proc,
      .hInstance = GetModuleHandle(NULL),
      .hCursor = LoadCursor(NULL, IDC_ARROW),
      .lpszClassName = class_name,
    }
  )) {
    MessageBox(NULL, "Failed to register window class.", "Error", MB_OK);
    return;
  }

  window_handle = CreateWindowEx(
    0,
    class_name,
    window_name,
    WS_OVERLAPPEDWINDOW | WS_VISIBLE,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    GLI_WIDTH,
    GLI_HEIGHT,
    NULL,
    NULL,
    GetModuleHandle(NULL),
    NULL
  );
  if (!window_handle) {
    MessageBox(NULL, "Failed to create window.", "Error", MB_OK);
    return;
  }

  device_context_handle = GetDC(window_handle);

  // Set up the pixel format descriptor.
  const PIXELFORMATDESCRIPTOR pixel_format_descriptor = {
    .nSize = sizeof(PIXELFORMATDESCRIPTOR),
    .nVersion = 1,
    .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    .iPixelType = PFD_TYPE_RGBA,
    .cColorBits = 32,
    .cDepthBits = 24,
    .cStencilBits = 0,
    .iLayerType = PFD_MAIN_PLANE,
  };

  const int pixel_format = ChoosePixelFormat(device_context_handle, &pixel_format_descriptor);
  if (pixel_format == 0) {
    MessageBox(NULL, "Failed to choose pixel format.", "Error", MB_OK);
    return;
  }

  if (!SetPixelFormat(device_context_handle, pixel_format, &pixel_format_descriptor)) {
    MessageBox(NULL, "Failed to set pixel format.", "Error", MB_OK);
    return;
  }

  const HGLRC dummy_context_handle = wglCreateContext(device_context_handle);
  if (!dummy_context_handle) {
    MessageBox(NULL, "Failed to create dummy OpenGL context.", "Error", MB_OK);
    return;
  }

  if (!wglMakeCurrent(device_context_handle, dummy_context_handle)) {
    MessageBox(NULL, "Failed to activate dummy OpenGL context.", "Error", MB_OK);
    return;
  }

  const PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
    (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

  if (!wglCreateContextAttribsARB) {
    MessageBox(NULL, "Failed to get the address of wglCreateContextAttribsARB", "Error", MB_OK);
    return;
  }

  context = wglCreateContextAttribsARB(
    device_context_handle,
    0,
    (int[]) {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
      WGL_CONTEXT_MINOR_VERSION_ARB, 3,
      WGL_CONTEXT_PROFILE_MASK_ARB,
      WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0,
    }
  );

  if (!context) {
    MessageBox(NULL, "Failed to create OpenGL 3.3 context.", "Error", MB_OK);
    return;
  }

  wglMakeCurrent(device_context_handle, context);
  wglDeleteContext(dummy_context_handle);
#endif

  GLI_LOAD_PROC_ADDRESS(glCreateShader);
  GLI_LOAD_PROC_ADDRESS(glDeleteShader);
  GLI_LOAD_PROC_ADDRESS(glShaderSource);
  GLI_LOAD_PROC_ADDRESS(glCompileShader);
  GLI_LOAD_PROC_ADDRESS(glGetShaderiv);
  GLI_LOAD_PROC_ADDRESS(glGetShaderInfoLog);
  GLI_LOAD_PROC_ADDRESS(glCreateProgram);
  GLI_LOAD_PROC_ADDRESS(glDeleteProgram);
  GLI_LOAD_PROC_ADDRESS(glAttachShader);
  GLI_LOAD_PROC_ADDRESS(glLinkProgram);
  GLI_LOAD_PROC_ADDRESS(glGetProgramiv);
  GLI_LOAD_PROC_ADDRESS(glGetProgramInfoLog);
  GLI_LOAD_PROC_ADDRESS(glGetActiveAttrib);
  GLI_LOAD_PROC_ADDRESS(glGetActiveUniform);
  GLI_LOAD_PROC_ADDRESS(glGetAttribLocation);
  GLI_LOAD_PROC_ADDRESS(glGetUniformLocation);
  GLI_LOAD_PROC_ADDRESS(glGenVertexArrays);
  GLI_LOAD_PROC_ADDRESS(glDeleteVertexArrays);
  GLI_LOAD_PROC_ADDRESS(glBindVertexArray);
  GLI_LOAD_PROC_ADDRESS(glGenBuffers);
  GLI_LOAD_PROC_ADDRESS(glDeleteBuffers);
  GLI_LOAD_PROC_ADDRESS(glBindBuffer);
  GLI_LOAD_PROC_ADDRESS(glVertexAttribPointer);
  GLI_LOAD_PROC_ADDRESS(glEnableVertexAttribArray);
  GLI_LOAD_PROC_ADDRESS(glBufferData);
  GLI_LOAD_PROC_ADDRESS(glUseProgram);
  GLI_LOAD_PROC_ADDRESS(glUniformMatrix4fv);

  ECS_SYSTEM(world, MakeMeshes, EcsOnLoad, [in] MeshData, [out] !Mesh);
  ecs_system(world, {
    .entity = ecs_entity(world, {
      .name = "CompileShaders",
      .add = ecs_ids(ecs_dependson(EcsOnLoad)),
    }),
    .query.expr = "[in] ShaderProgramSource, [out] !ShaderProgram",
    .callback = CompileShaders,
    .immediate = true,
  });
  ECS_SYSTEM(world, PreRenderFrame, EcsPreStore,
    [in] ?ClearColor(ClearColor),
    [inout] ?Camera2D(Camera2D),
    [inout] ?Camera3D(Camera3D),
  );
  ECS_SYSTEM(world, Render, EcsOnStore, [in] ShaderProgram, [in] ?Camera2D(Camera2D), [in] ?Camera3D(Camera3D));
  ECS_SYSTEM(world, PostRenderFrame, EcsPostFrame, 0);

  ecs_singleton_add(world, ClearColor);
}

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
