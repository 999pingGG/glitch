# GLitch (WIP)
A very simple, ECS-based, cross-platform OpenGL library written in C. It has no major external dependencies: No SDL, no
GLFW, no Glad, no GLEW! Intended for quick-and-dirty graphics. Supports a single window, a single camera, a single
framebuffer. Compiles absolutely warning-free. More documentation coming soon.

## Requirements
A C11 compiler, [Flecs](https://www.flecs.dev/flecs/) and [cvkm](https://github.com/999pingGG/cvkm). The tests need CMake, but they are easy to
compile without it.

## Installation and building
Copy `glitch.h` and `glitch.c` to your project. Link against `X11` and `GL` in Linux and against `Opengl32` in Windows.

## Testing
Do a standard CMake build to build the tests.

## Motivation
Because I needed a dead simple library to quickly draw some stuff with Flecs.

## Roadmap
- Document the components.
- 3D support.
- Vertex attributes other than a single vec2 position.
- Quite a bit more stuff.
