cmake_minimum_required(VERSION 3.17)

project(tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(tests
  include/glitch.h
  src/glitch.c
  src/tests.c
  libs/cvkm/cvkm.h
  libs/flecs/flecs.c
  libs/flecs/flecs.h
)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
  target_link_libraries(tests PUBLIC ${MATH_LIBRARY})
endif()

# Because we don't control those.
set(CVKM_DISABLE_WARNINGS_LIST
  libs/flecs/flecs.c
  libs/flecs/flecs.h
)

if(MSVC)
  target_compile_options(tests PRIVATE /W4 /WX)
  set_source_files_properties(${CVKM_DISABLE_WARNINGS_LIST} PROPERTIES COMPILE_FLAGS /W0)
else()
  target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
  set_source_files_properties(${CVKM_DISABLE_WARNINGS_LIST} PROPERTIES COMPILE_FLAGS -w)
  target_compile_definitions(tests PRIVATE MUNIT_NO_FORK)
endif()

if(UNIX AND NOT ANDROID)
  target_compile_definitions(tests PRIVATE GLI_LINUX)
  target_link_libraries(tests PRIVATE X11 GL)
elseif(WIN32)
  target_compile_definitions(tests PRIVATE GLI_WINDOWS)
  target_link_libraries(tests PRIVATE Opengl32)
endif()

target_include_directories(tests PRIVATE include libs/flecs libs/cvkm)
