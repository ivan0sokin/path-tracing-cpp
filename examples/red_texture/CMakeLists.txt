cmake_minimum_required(VERSION 3.5)
project(red_texture)

add_executable(red_texture red_texture.cpp)
target_include_directories(red_texture PRIVATE ${IMGUI_DIR} ${PTRACE_INCLUDE_DIR})
target_link_directories(red_texture PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(red_texture PRIVATE ptrace)