cmake_minimum_required(VERSION 3.6)

project(heman C)

include_directories(include .)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(OpenGL REQUIRED)

set(glfw3_DIR "../vcpkg/installed/x86-windows/share/glfw3")
find_package(glfw3 CONFIG REQUIRED) #  generates an import target glfw


set(HEMAN_SOURCE
    src/color.c
    src/distance.c
    src/draw.c
    src/export.c
    src/gaussian.c
    src/generate.c
    src/image.c
    src/image.h
    src/import.c
    src/lighting.c
    src/noise.c
    src/noise.h
    src/ops.c
    src/points.c)

set(MATH_SOURCE
    kazmath/aabb2.c
    kazmath/aabb3.c
    kazmath/mat3.c
    kazmath/mat4.c
    kazmath/plane.c
    kazmath/quaternion.c
    kazmath/ray2.c
    kazmath/ray3.c
    kazmath/utility.c
    kazmath/vec2.c
    kazmath/vec3.c
    kazmath/vec4.c)

add_library(heman STATIC ${HEMAN_SOURCE} ${MATH_SOURCE})


add_executable(main
${MATH_SOURCE}
${HEMAN_SOURCE}
main.c
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/packages/glfw3_x86-windows/bin/glfw3.dll  ${CMAKE_CURRENT_BINARY_DIR}/glfw3.dll COPYONLY)

target_link_libraries(
main PRIVATE
${OPENGL_LIBRARIES}
glfw)
