#define CVKM_NO
#define CVKM_ENABLE_FLECS
#define CVKM_FLECS_IMPLEMENTATION
#include <cvkm.h>
#include <flecs.h>
#include <glitch.h>

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
    positions[i] = (Position2D){ { vkm_cos(lifetimes[i]) * 100.0f, vkm_sin(lifetimes[i]) * 100.0f } };
  }
}

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

  ECS_SYSTEM(world, Age, EcsOnUpdate, [inout] Lifetime, [in] TimeScale);
  ECS_SYSTEM(world, Move, EcsOnUpdate, [out] cvkm.Position2D, [in] Lifetime);

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
  
  const ecs_entity_t triangle_mesh = ecs_entity(world, {
    .set = ecs_values(
      {
        .type = ecs_id(MeshData),
        .ptr = &(MeshData) {
          .data = triangle_mesh_buffer,
          .vertices_count = 3,
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
          .vertex_attributes = {
            { .type = GLI_VEC2 },
            { .type = GLI_FLOAT },
          },
        },
      }
    ),
  });

  static const char* vertex_shader_source =
    "layout(location = 0) in vec2 position;\n"
    "layout(location = 1) in float light;\n"
    "\n"
    "out float light_intensity;\n"
    "\n"
    "void main() {\n"
    "  light_intensity = light;\n"
    "  gl_Position = view_projection_matrix * model_matrix * vec4(position, 0.0, 1.0);\n"
    "}\n";

  static const char* fragment_shader_source =
    "in float light_intensity;\n"
    "\n"
    "out vec4 fragment_color;\n"
    "\n"
    "uniform vec4 entityColor;\n"
    "uniform float does_not_exist = 1.0;\n"
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
    "    * does_not_exist;\n"
    "}\n";

  const ecs_entity_t shader_program = ecs_entity(world, {
    .name = "Test shader program",
    .set = ecs_values(
      {
        .type = ecs_id(ShaderProgramSource),
        .ptr = &(ShaderProgramSource) {
          .vertex_shader = strdup(vertex_shader_source),
          .fragment_shader = strdup(fragment_shader_source),
        },
      },
      {
        .type = ecs_id(Tint),
        .ptr = &(Tint){ { 1.5f, 1.5f, 1.5f, 1.0f } },
      }
    ),
  });

  ecs_entity(world, {
    .name = "Triangle",
    .add = ecs_ids(
      ecs_id(TimeScale),
      ecs_id(Position2D),
      ecs_id(Lifetime),
      ecs_pair(ecs_id(Uses), triangle_mesh),
      ecs_pair(ecs_id(Uses), shader_program)
    ),
    .set = ecs_values(
      { .type = ecs_id(Color), .ptr = &(Color) { { 1.0f, 0.0f, 0.0f, 1.0f } } }
    ),
  });

  ecs_entity(world, {
    .name = "Square",
    .add = ecs_ids(
      ecs_id(Position2D),
      ecs_id(Lifetime),
      ecs_pair(ecs_id(Uses), square_mesh),
      ecs_pair(ecs_id(Uses), shader_program)
    ),
    .set = ecs_values(
      { .type = ecs_id(TimeScale), .ptr = &(float){ 2.0f } },
      { .type = ecs_id(Color), .ptr = &(Color) { { 0.0f, 1.0f, 0.0f, 1.0f } } }
    ),
  });

  ecs_app_run(world, &(ecs_app_desc_t){
    .target_fps = 60,
    .frames = 0,
    .enable_rest = true,
    .enable_stats = true,
  });

  ecs_fini(world);
}
