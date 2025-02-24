#include <flecs.h>
#include <glitch.h>

int main(const int argc, char** argv) {
  ecs_world_t* world = ecs_init_w_args(argc, argv);

  ECS_IMPORT(world, GLitch);

  static const vkm_vec2 triangle_vertices[] = {
    { {  0.0f,  0.5f } },
    { { -0.5f, -0.5f } },
    { {  0.5f, -0.5f } },
  };

  static const vkm_vec2 rectangle_vertices[] = {
    { { -0.9f, 0.9f } },
    { { -0.9f, 0.6f } },
    { { -0.6f, 0.6f } },
    { { -0.9f, 0.9f } },
    { { -0.6f, 0.6f } },
    { { -0.6f, 0.9f } },
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
  ecs_add_pair(world, triangle_entity, ecs_id(Uses), triangle_mesh);
  ecs_add_pair(world, triangle_entity, ecs_id(Uses), shader_program);

  const ecs_entity_t rectangle_entity = ecs_new(world);
  ecs_set(world, rectangle_entity, Position, { 0 });
  ecs_add_pair(world, rectangle_entity, ecs_id(Uses), rectangle_mesh);
  ecs_add_pair(world, rectangle_entity, ecs_id(Uses), shader_program);

  ecs_app_run(world, &(ecs_app_desc_t){
    .target_fps = 60,
    .frames = 300,
    .enable_rest = true,
    .enable_stats = true,
  });

  ecs_fini(world);
}
