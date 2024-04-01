cmake_minimum_required(VERSION 3.5)
project(path_tracing VERSION 1.0)

file(GLOB IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends/imgui_impl_opengl3.cpp"
)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking)
set(IMGUI_BACKENDS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends)
set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4/include)
set(GLFW_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4/src)
set(GLAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
set(GLAD_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glad/src)

add_subdirectory(glfw-3.4)
add_subdirectory(glad)
add_subdirectory(examples)