#include <assert.h>
#include <stdlib.h>

#define CVKM_RH_NO
#define CVKM_FLECS_IMPLEMENTATION
#include <cvkm.h>
#include <flecs.h>
#include <glitch.h>

#ifdef GLI_LINUX
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef GLXContext (* glXCreateContextAttribsARBProc)(
  Display* display,
  GLXFBConfig config,
  GLXContext share_context,
  Bool direct,
  const int* attrib_list
);

static Display* display;
static Window window;
#endif

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
  glDeleteProgram(ptr->program);
  *ptr = (ShaderProgram){ 0 };
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
      goto cleanup;
    }

    GLint max_length;
    glGetProgramiv(shader_program.program,  GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
    char* name_buffer = malloc(max_length);

    glGetProgramiv(shader_program.program, GL_ACTIVE_UNIFORMS, &shader_program.uniforms_count);
    shader_program.uniforms = malloc(shader_program.uniforms_count * sizeof(gli_uniform));
    for (int j = 0; j < shader_program.uniforms_count; j++) {
      GLsizei length;
      GLint size;
      GLenum type;
      glGetActiveUniform(shader_program.program, j, max_length, &length, &size, &type, name_buffer);
      shader_program.uniforms[j] = (gli_uniform) {
        .name = strdup(name_buffer),
        .location = j,
      };
    }

    free(name_buffer);

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
  (void)it;
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void RenderFrame(ecs_iter_t* it) {
  const Position* positions = ecs_field(it, Position, 0);
  const Mesh* mesh = ecs_field(it, Mesh, 3);
  const ShaderProgram* shader_program = ecs_field(it, ShaderProgram, 4);

  assert(!ecs_field_is_self(it, 3));
  assert(!ecs_field_is_self(it, 4));

  vkm_mat4 view_projection_matrix;
  // Apply projection. Hardcoded for now.
  vkm_ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, &view_projection_matrix);

  // Apply view. (0, 0, -0.5) hardcoded for now.
  view_projection_matrix.m32 = -0.5f;

  glUseProgram(shader_program->program);
  const GLint view_projection_matrix_location = get_uniform_location(shader_program, "view_projection_matrix");
  if (view_projection_matrix_location >= 0) {
    glUniformMatrix4fv(view_projection_matrix_location, 1, GL_FALSE, (const GLfloat*)view_projection_matrix.raw);
  }

  glBindVertexArray(mesh->vertex_array_object);

  for (int i = 0; i < it->count; i++) {
    const GLint model_matrix_location = get_uniform_location(shader_program, "model_matrix");
    if (model_matrix_location >= 0) {
      const Position* position = positions + i;

      // @formatter:off
      const vkm_mat4 model_matrix = (vkm_mat4){ .raw = {
        1.0f, 0.0f, 0.0f, position->x,
        0.0f, 1.0f, 0.0f, position->y,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
      } };
      // @formatter:on

      glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, (const GLfloat*)model_matrix.raw);
    }

    glDrawArrays(GL_TRIANGLES, 0, mesh->vertices_count);
  }
}

static void PostRenderFrame(ecs_iter_t* it) {
  (void)it;
#ifdef GLI_LINUX
  glXSwapBuffers(display, window);
#endif
}

#define GLI_LOAD_PROC_ADDRESS(fun) do {\
  fun = (fun##Proc)glXGetProcAddressARB((const GLubyte*)#fun);\
  if (!fun){\
    fprintf(stderr, "Failed to load "#fun"\n");\
    return;\
  }\
} while (0)

void GLitchImport(ecs_world_t* world) {
  ECS_MODULE(world, GLitch);

  ECS_IMPORT(world, cvkm);

  ECS_COMPONENT_DEFINE(world, MeshData);
  ECS_COMPONENT_DEFINE(world, Mesh);
  ECS_COMPONENT_DEFINE(world, ShaderProgramSource);
  ECS_COMPONENT_DEFINE(world, ShaderProgram);
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
  XSetWindowAttributes swa = {
    .background_pixmap = None,
    .event_mask = StructureNotifyMask | ExposureMask | KeyPressMask,
    .colormap = XCreateColormap(display, RootWindow(display, visual_info->screen), visual_info->visual, AllocNone),
  };

  window = XCreateWindow(
    display,
    RootWindow(display, visual_info->screen),
    0, 0, 800, 600, 0,
    visual_info->depth,
    InputOutput,
    visual_info->visual,
    CWBorderPixel | CWColormap | CWEventMask,
    &swa
  );
  XFree(visual_info);

  if (!window) {
    fprintf(stderr, "Failed to create window.\n");
    return;
  }

  XStoreName(display, window, "GLitch");
  XMapWindow(display, window);

  // Get the pointer to glXCreateContextAttribsARB for modern context creation
  glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
    (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

  if (!glXCreateContextAttribsARB) {
    fprintf(stderr, "Failed to get the address of glXCreateContextAttribsARB.\n");
    return;
  }

  // Create a OpenGL context version 3.3
  GLXContext ctx = glXCreateContextAttribsARB(
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
  if (!ctx) {
    fprintf(stderr, "Failed to create OpenGL 3.3 context.\n");
    return;
  }

  // Make the context current
  glXMakeCurrent(display, window, ctx);
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
  GLI_LOAD_PROC_ADDRESS(glGetActiveUniform);
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
  ECS_SYSTEM(world, CompileShaders, EcsOnLoad, [in] ShaderProgramSource, [out] !ShaderProgram);
  ECS_SYSTEM(world, PreRenderFrame, EcsPreStore, 0);
  ECS_SYSTEM(world, RenderFrame, EcsOnStore,
    [in] cvkm.Position,
    [none] (Uses, $mesh),
    [none] (Uses, $shader_program),
    [in] Mesh($mesh),
    [in] ShaderProgram($shader_program)
  );
  ECS_SYSTEM(world, PostRenderFrame, EcsPostFrame, 0);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
