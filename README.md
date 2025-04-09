# GLitch (WIP)
A very simple, ECS-based, cross-platform OpenGL library written in C. It has no major external dependencies: No SDL, no
GLFW, no Glad, no GLEW! Intended for quick-and-dirty graphics or simple games. Supports a single window, a single
camera, a single framebuffer. Compiles absolutely warning-free. More documentation coming soon, see the tests for
usage. **Not ready for production yet.**

## Requirements
- C11 compiler. Tested with gcc, clang and msvc.
- [Flecs](https://www.flecs.dev/flecs/)
- [cvkm](https://github.com/999pingGG/cvkm)
- Windows, Linux (x11) or Emscripten.
- OpenGL 3.3 or WebGL 2.0.

The tests need CMake, but they are easy to compile without it.

## Installation and building
Copy `glitch.h` and `glitch.c` to your project. Link against `X11` and `GL` in Linux and against `Opengl32` in Windows.
Emscripten needs the flags `-sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sALLOW_MEMORY_GROWTH=1`.

## Testing
Do a standard CMake build to build the tests.

## Motivation
Because I needed a dead simple library to quickly draw some stuff with Flecs, which can also be used to make simple
games.

## Roadmap
- Document the components.
- Textures.
- Loading gltf models.
- "High-level" components, like lights and predefined shaders.
