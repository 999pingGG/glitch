#define CVKM_NO
#define CVKM_ENABLE_FLECS
#include <cvkm.h>
#include <flecs.h>
#include <glitch.h>

typedef float Lifetime;
static ECS_COMPONENT_DECLARE(Lifetime);
typedef float TimeScale;
static ECS_COMPONENT_DECLARE(TimeScale);

static ECS_CTOR(Lifetime, ptr, {
  *ptr = 0;
})

static ECS_CTOR(TimeScale, ptr, {
  *ptr = 1;
})

static void Age(ecs_iter_t* it) {
  Lifetime* lifetimes = ecs_field(it, Lifetime, 0);
  const TimeScale* time_scales = ecs_field(it, TimeScale, 1);

  for (int i = 0; i < it->count; i++) {
    lifetimes[i] += it->delta_time * time_scales[i];
  }
}

static void Move(ecs_iter_t* it) {
  Position* positions = ecs_field(it, Position, 0);
  const Lifetime* lifetimes = ecs_field(it, Lifetime, 1);

  for (int i = 0; i < it->count; i++) {
    positions[i] = (Position){ { vkm_cos(lifetimes[i]) * 100.0f, vkm_sin(lifetimes[i]) * 100.0f } };
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

  ECS_SYSTEM(world, Age, EcsOnUpdate, [inout] Lifetime, [in] TimeScale);
  ECS_SYSTEM(world, Move, EcsOnUpdate, [out] cvkm.Position, [in] Lifetime);

  // Equilateral triangle
  static const vkm_vec2 triangle_vertices[] = {
    { {  0.0f,  173.205081f } },
    { { -100.0f, 0.0f } },
    { {  100.0f, 0.0f } },
  };

  static const vkm_vec2 rectangle_vertices[] = {
    { { -100.0f,  100.0f } },
    { { -100.0f, -100.0f } },
    { {  100.0f, -100.0f } },
    { { -100.0f,  100.0f } },
    { {  100.0f, -100.0f } },
    { {  100.0f,  100.0f } },
  };

  const ecs_entity_t triangle_mesh = ecs_new(world);
  ecs_set(world, triangle_mesh, MeshData, {
    .vertices = triangle_vertices,
    .vertices_count = GLI_COUNTOF(triangle_vertices),
  });

  const ecs_entity_t rectangle_mesh = ecs_new(world);
  ecs_set(world, rectangle_mesh, MeshData, {
    .vertices = rectangle_vertices,
    .vertices_count = GLI_COUNTOF(rectangle_vertices),
  });

  static const char* vertex_shader_source = "#version 330 core\n"
    "layout(location = 0) in vec2 position;\n"
    "\n"
    "uniform mat4 view_projection_matrix;\n"
    "uniform mat4 model_matrix;\n"
    "\n"
    "void main() {\n"
    "  gl_Position = view_projection_matrix * model_matrix * vec4(position, 0.0, 1.0);\n"
    "}\n";

  static const char* fragment_shader_source = "#version 330 core\n"
    "out vec4 fragment_color;\n"
    "void main() {\n"
    "  fragment_color = vec4(1.0, 0.5, 0.2, 1.0);\n"
    "}\n";

  const ecs_entity_t shader_program = ecs_new(world);
  ecs_set(world, shader_program, ShaderProgramSource, {
    .vertex_shader = vertex_shader_source,
    .fragment_shader = fragment_shader_source,
  });

  const ecs_entity_t triangle_entity = ecs_new(world);
  ecs_set(world, triangle_entity, Position, { 0 });
  ecs_add(world, triangle_entity, Lifetime);
  ecs_add(world, triangle_entity, TimeScale);
  ecs_add_pair(world, triangle_entity, ecs_id(Uses), triangle_mesh);
  ecs_add_pair(world, triangle_entity, ecs_id(Uses), shader_program);

  const ecs_entity_t rectangle_entity = ecs_new(world);
  ecs_set(world, rectangle_entity, Position, { 0 });
  ecs_add(world, rectangle_entity, Lifetime);
  ecs_set(world, rectangle_entity, TimeScale, { 2 });
  ecs_add_pair(world, rectangle_entity, ecs_id(Uses), rectangle_mesh);
  ecs_add_pair(world, rectangle_entity, ecs_id(Uses), shader_program);

  ecs_singleton_add(world, Camera2D);

  ecs_app_run(world, &(ecs_app_desc_t){
    .target_fps = 60,
    .frames = 0,
    .enable_rest = true,
    .enable_stats = true,
  });

  ecs_fini(world);
}
