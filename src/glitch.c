// Before anything happens at all, the Windows API needs a crash course in staying out of the way...
#ifdef GLI_WINDOWS
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
#define CVKM_FLECS_IMPLEMENTATION
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
ECS_COMPONENT_DECLARE(Camera2D);

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

ECS_CTOR(Camera2D, ptr, {
  *ptr = (Camera2D){
    .position = CVKM_VEC2_ZERO,
    .zoom = 1.0f,
  };
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
    glGetProgramiv(shader_program.program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_length);
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

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void RenderFrame(ecs_iter_t* it) {
  const Position* positions = ecs_field(it, Position, 0);
  const Mesh* mesh = ecs_field(it, Mesh, 3);
  const ShaderProgram* shader_program = ecs_field(it, ShaderProgram, 4);
  const Camera2D* camera = ecs_field(it, Camera2D, 5);

  assert(!ecs_field_is_self(it, 3));
  assert(!ecs_field_is_self(it, 4));
  assert(!ecs_field_is_self(it, 5));

  const float zoom_factor = 1.0f / camera->zoom;
  const float half_width = GLI_WIDTH * 0.5f * zoom_factor;
  const float half_height = GLI_HEIGHT * 0.5f * zoom_factor;
  vkm_mat4 view_projection_matrix;
  // Apply projection.
  vkm_ortho(-half_width, half_width, -half_height, half_height, -1000.f, 1000.f, &view_projection_matrix);

  // Apply view.
  vkm_translate(&view_projection_matrix, (&(vkm_vec2){ { -camera->position.x, -camera->position.y } }));

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

      vkm_mat4 model_matrix = CVKM_MAT4_IDENTITY;
      vkm_translate(&model_matrix, position);

      glUniformMatrix4fv(model_matrix_location, 1, GL_FALSE, (const GLfloat*)model_matrix.raw);
    }

    glDrawArrays(GL_TRIANGLES, 0, mesh->vertices_count);
  }
}

static void PostRenderFrame(ecs_iter_t* it) {
  (void)it;
#ifdef GLI_LINUX
  glXSwapBuffers(display, window);
#else
  SwapBuffers(device_context_handle);
#endif
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

void glitchImport(ecs_world_t* world) {
  ECS_MODULE(world, glitch);

  ECS_IMPORT(world, cvkm);

  ecs_atfini(world, fini, NULL);

  ECS_COMPONENT_DEFINE(world, MeshData);
  ECS_COMPONENT_DEFINE(world, Mesh);
  ECS_COMPONENT_DEFINE(world, ShaderProgramSource);
  ECS_COMPONENT_DEFINE(world, ShaderProgram);
  ECS_COMPONENT_DEFINE(world, Camera2D);

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
    [in] ShaderProgram($shader_program),
    [in] Camera2D(Camera2D),
  );
  ECS_SYSTEM(world, PostRenderFrame, EcsPostFrame, 0);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
