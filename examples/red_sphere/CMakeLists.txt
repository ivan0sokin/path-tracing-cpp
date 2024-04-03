cmake_minimum_required(VERSION 3.5)
project(red_sphere)

add_executable(red_sphere red_sphere.cpp)
target_include_directories(red_sphere PRIVATE ${IMGUI_DIR} ${PTRACE_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
target_link_directories(red_sphere PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(red_sphere PRIVATE ptrace)