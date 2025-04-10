#include <stdlib.h>

#define CVKM_NO
#define CVKM_ENABLE_FLECS
#define CVKM_FLECS_IMPLEMENTATION
#include <cvkm.h>
#include <flecs.h>
#include <glitch.h>

#define TARGET_FPS 60

typedef float Lifetime;
static ECS_COMPONENT_DECLARE(Lifetime);
typedef float TimeScale;
static ECS_COMPONENT_DECLARE(TimeScale);
typedef vkm_vec4 Tint;
static ECS_COMPONENT_DECLARE(Tint);

static ECS_CTOR(Lifetime, ptr, {
  *ptr = 0;
})

static ECS_CTOR(TimeScale, ptr, {
  *ptr = 1.0f;
})

static ECS_CTOR(Tint, ptr, {
  *ptr = (vkm_vec4){ { 1.0f, 1.0f, 1.0f, 1.0f } };
})

static void Age(ecs_iter_t* it) {
  Lifetime* lifetimes = ecs_field(it, Lifetime, 0);
  const TimeScale* time_scales = ecs_field(it, TimeScale, 1);

  for (int i = 0; i < it->count; i++) {
    lifetimes[i] += it->delta_time * time_scales[i];
  }
}

static void Move(ecs_iter_t* it) {
  Position2D* positions = ecs_field(it, Position2D, 0);
  const Lifetime* lifetimes = ecs_field(it, Lifetime, 1);

  for (int i = 0; i < it->count; i++) {
    positions[i] = (Position2D){ { vkm_cos(lifetimes[i]) * 300.0f, vkm_sin(lifetimes[i]) * 300.0f } };
  }
}

static void Rotate(ecs_iter_t* it) {
  const TimeScale* time_scales = ecs_field(it, TimeScale, 1);

  const ecs_id_t rotation_id = ecs_field_id(it, 0);
  if (rotation_id == ecs_id(Rotation2D)) {
    Rotation2D* rotations = ecs_table_get_id(it->world, it->table, ecs_id(Rotation2D), it->offset);

    for (int i = 0; i < it->count; i++) {
      Rotation2D* rotation = rotations + i;
      const float factor = (time_scales ? time_scales[i] : 1.0f) * 2.0f;
      *rotation += it->delta_time * factor;
    }
  } else {
    Rotation3D* rotations = ecs_table_get_id(it->world, it->table, ecs_id(Rotation3D), it->offset);

    for (int i = 0; i < it->count; i++) {
      Rotation3D* rotation = rotations + i;
      
      const float factor = time_scales ? time_scales[i] : 1.0f;
      vkm_quat delta = CVKM_QUAT_IDENTITY;
      vkm_euler_to_quat(&(vkm_vec3) { {
        factor * it->delta_time * 0.6f,
        factor * it->delta_time,
        factor * it->delta_time * 0.3f,
      } }, &delta);

      vkm_mul(rotation, &delta, rotation);
    }
  }
}

#ifdef GLI_EMSCRIPTEN
static bool emscripten_main_loop(const double time, void* world) {
  static double last_time = 0.0;
  const double delta_time = last_time <= 0.0 ? 1.0 / TARGET_FPS : (time - last_time) / 1000.0;
  last_time = time;

  const bool quit = !ecs_progress(world, (ecs_ftime_t)delta_time);
  if (quit) {
    ecs_fini(world);
    return false;
  }

  return true;
}
#endif

int main(const int argc, char** argv) {
  ecs_world_t* world = ecs_init_w_args(argc, argv);

  ECS_IMPORT(world, glitch);

  ECS_COMPONENT_DEFINE(world, Lifetime);
  ecs_set_hooks(world, Lifetime, { .ctor = ecs_ctor(Lifetime) });
  ecs_add_pair(world, ecs_id(Lifetime), EcsIsA, EcsSeconds);

  ECS_COMPONENT_DEFINE(world, TimeScale);
  ecs_primitive(world, { .entity = ecs_id(TimeScale), .kind = EcsF32 });
  ecs_set_hooks(world, TimeScale, { .ctor = ecs_ctor(TimeScale) });

  ECS_COMPONENT_DEFINE(world, Tint);
  ecs_add_pair(world, ecs_id(Tint), EcsIsA, ecs_id(vkm_vec4));
  ecs_set_hooks(world, Tint, { .ctor = ecs_ctor(Tint) });

  ECS_TAG(world, Rotating);

  ECS_SYSTEM(world, Age, EcsOnUpdate, [inout] Lifetime, [in] TimeScale);
  ECS_SYSTEM(world, Move, EcsOnUpdate, [out] cvkm.Position2D, [in] Lifetime);
  ECS_SYSTEM(world, Rotate, EcsOnUpdate, [inout] cvkm.Rotation2D || cvkm.Rotation3D, [in] ?TimeScale, [none] Rotating);

  ecs_set_id(world, ecs_id(Window), ecs_id(Window), sizeof(GLitchWindow), &(GLitchWindow){
    .name = "GLitch tests",
    .size = { { 800, 600 } },
  });

  // Equilateral triangle
  static const float triangle_vertices[] = {
    // Position
       0.0f, 173.205081f,
    -100.0f,   0.0f,
     100.0f,   0.0f,
    
    // Light intensity
    1.0f,
    0.5f,
    0.0f,
  };
  void* triangle_mesh_buffer = malloc(sizeof(triangle_vertices));
  memcpy(triangle_mesh_buffer, triangle_vertices, sizeof(triangle_vertices));

  static const float square_vertices[] = {
    // Position
    -100.0f,  100.0f,
    -100.0f, -100.0f,
     100.0f, -100.0f,
    -100.0f,  100.0f,
     100.0f, -100.0f,
     100.0f,  100.0f,

    // Light intensity
    1.0f,
    0.8f,
    0.6f,
    0.4f,
    0.2f,
    0.0f,
  };
  void* square_mesh_buffer = malloc(sizeof(square_vertices));
  memcpy(square_mesh_buffer, square_vertices, sizeof(square_vertices));

  static const float cube_vertices[] = {
    // Position

    // Right face (facing positive x)
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,

    // Left face (facing negative x)
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,

    // Up face (facing positive y)
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,

    // Down face(facing negative y)
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,

    // Back face (facing positive z)
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

    // Front face (facing negative z)
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    // Color
    
    // Right face
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,

    // Left face
    0.1f, 0.0f, 0.0f, 1.0f,
    0.1f, 0.0f, 0.0f, 1.0f,
    0.1f, 0.0f, 0.0f, 1.0f,
    0.1f, 0.0f, 0.0f, 1.0f,

    // Up face
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,

    // Down face
    0.0f, 0.1f, 0.0f, 1.0f,
    0.0f, 0.1f, 0.0f, 1.0f,
    0.0f, 0.1f, 0.0f, 1.0f,
    0.0f, 0.1f, 0.0f, 1.0f,

    // Back face
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,

    // Front face
    0.0f, 0.0f, 0.1f, 1.0f,
    0.0f, 0.0f, 0.1f, 1.0f,
    0.0f, 0.0f, 0.1f, 1.0f,
    0.0f, 0.0f, 0.1f, 1.0f,
  };
  static unsigned cube_indices[] = {
     0,  1,  2,
     2,  3,  0,
    
     4,  5,  6,
     6,  7,  4,

     8,  9, 10,
     8, 10, 11,

    12, 14, 13,
    12, 15, 14,

    16, 18, 17,
    16, 19, 18,

    20, 21, 22,
    20, 22, 23,
  };
  void* cube_mesh_buffer = malloc(sizeof(cube_vertices));
  memcpy(cube_mesh_buffer, cube_vertices, sizeof(cube_vertices));
  void* cube_indices_buffer = malloc(sizeof(cube_indices));
  memcpy(cube_indices_buffer, cube_indices, sizeof(cube_indices));
  
  const ecs_entity_t triangle_mesh = ecs_entity(world, {
    .set = ecs_values(
      {
        .type = ecs_id(MeshData),
        .ptr = &(MeshData) {
          .data = triangle_mesh_buffer,
          .vertices_count = 3,
          .primitive = GLI_TRIANGLES,
          .vertex_attributes = {
            { .type = GLI_VEC2 },
            { .type = GLI_FLOAT },
          },
        },
      }
    ),
  });

  const ecs_entity_t square_mesh = ecs_entity(world, {
    .set = ecs_values(
      {
        .type = ecs_id(MeshData),
        .ptr = &(MeshData) {
          .data = square_mesh_buffer,
          .vertices_count = 6,
          .primitive = GLI_TRIANGLES,
          .vertex_attributes = {
            { .type = GLI_VEC2 },
            { .type = GLI_FLOAT },
          },
        },
      }
    ),
  });

  const ecs_entity_t cube_mesh = ecs_entity(world, {
    .set = ecs_values(
      {
        .type = ecs_id(MeshData),
        .ptr = &(MeshData) {
          .data = cube_mesh_buffer,
          .indices = cube_indices_buffer,
          .vertices_count = 24,
          .indices_count = 36,
          .primitive = GLI_TRIANGLES,
          .vertex_attributes = {
            { .type = GLI_VEC3 },
            { .type = GLI_VEC4 },
          },
        },
      }
    ),
  });

  static const char* vertex_shader_source_2d =
    "layout(location = 0) in vec2 position;\n"
    "layout(location = 1) in float light;\n"
    "\n"
    "out float light_intensity;\n"
    "\n"
    "void main() {\n"
    "  light_intensity = light;\n"
    "  gl_Position = projection * view * model * vec4(position, 0.0, 1.0);\n"
    "}\n";

  static const char* fragment_shader_source_2d =
    "in float light_intensity;\n"
    "\n"
    "out vec4 fragment_color;\n"
    "\n"
    "uniform vec4 entityColor;\n"
    "uniform float does_not_exist;\n"
    "uniform float entityTimeScale;\n"
    "uniform vec4 Tint;\n"
    "uniform int unused;\n"
    "\n"
    "void main() {\n"
    "  fragment_color =\n"
    "    entityColor\n"
    "    * light_intensity\n"
    "    * Tint\n"
    "    * (sin(time * entityTimeScale) * 0.5 + 0.5)\n"
    "    + does_not_exist;\n"
    "}\n";

  static const char* vertex_shader_source_3d =
    "layout(location = 0) in vec3 position;\n"
    "layout(location = 1) in vec4 color_;\n"
    "\n"
    "out vec4 color;\n"
    "\n"
    "void main() {\n"
    "  color = color_;\n"
    "  gl_Position = projection * view * model * vec4(position, 1.0);\n"
    "}\n";

  static const char* fragment_shader_source_3d =
    "in vec4 color;\n"
    "\n"
    "out vec4 fragment_color;\n"
    "\n"
    "void main() {\n"
    "  fragment_color = color;\n"
    "}\n";

  const ecs_entity_t shader_program_2d = ecs_entity(world, {
    .name = "Test shader program 2D",
    .set = ecs_values(
      {
        .type = ecs_id(ShaderProgramSource),
        .ptr = &(ShaderProgramSource) {
          .vertex_shader = strdup(vertex_shader_source_2d),
          .fragment_shader = strdup(fragment_shader_source_2d),
        },
      },
      {
        .type = ecs_id(Tint),
        .ptr = &(Tint){ { 1.5f, 1.5f, 1.5f, 1.0f } },
      }
    ),
  });

  const ecs_entity_t shader_program_3d = ecs_entity(world, {
    .name = "Test shader program 3D",
    .set = ecs_values(
      {
        .type = ecs_id(ShaderProgramSource),
        .ptr = &(ShaderProgramSource) {
          .vertex_shader = strdup(vertex_shader_source_3d),
          .fragment_shader = strdup(fragment_shader_source_3d),
        },
      }
    ),
  });

  ecs_entity(world, {
    .name = "Triangle",
    .add = ecs_ids(
      ecs_id(Position2D),
      ecs_id(Rotation2D),
      ecs_id(TimeScale),
      ecs_id(Lifetime),
      ecs_pair(ecs_id(Uses), triangle_mesh),
      ecs_pair(ecs_id(Uses), shader_program_2d),
      ecs_id(Rotating)
    ),
    .set = ecs_values(
      { .type = ecs_id(Color), .ptr = &(Color) { { 1.0f, 0.0f, 0.0f, 1.0f } } }
    ),
  });

  ecs_entity(world, {
    .name = "Square",
    .add = ecs_ids(
      ecs_id(Position2D),
      ecs_id(Rotation2D),
      ecs_id(Lifetime),
      ecs_pair(ecs_id(Uses), square_mesh),
      ecs_pair(ecs_id(Uses), shader_program_2d),
      ecs_id(Rotating)
    ),
    .set = ecs_values(
      { .type = ecs_id(TimeScale), .ptr = &(float){ 2.0f } },
      { .type = ecs_id(Color), .ptr = &(Color) { { 0.0f, 1.0f, 0.0f, 1.0f } } }
    ),
  });

  ecs_entity(world, {
    .name = "Cube",
    .add = ecs_ids(
      ecs_id(Rotation3D),
      ecs_pair(ecs_id(Uses), cube_mesh),
      ecs_pair(ecs_id(Uses), shader_program_3d),
      ecs_id(Rotating)
    ),
    .set = ecs_values(
      { .type = ecs_id(Position3D), .ptr = &(Position3D) { { 0.0f, 0.0f, -3.0f } } }
    ),
  });

#ifdef GLI_EMSCRIPTEN
  emscripten_request_animation_frame_loop(emscripten_main_loop, world);
#else
  ecs_app_run(world, &(ecs_app_desc_t){
    .target_fps = TARGET_FPS,
    .frames = 0,
    .enable_rest = true,
    .enable_stats = true,
  });

  ecs_fini(world);
#endif
}
