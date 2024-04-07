cmake_minimum_required(VERSION 3.5)
project(triangle)

add_executable(triangle triangle.cpp)
target_include_directories(triangle PRIVATE ${IMGUI_DIR} ${PTRACE_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
target_link_directories(triangle PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(triangle PRIVATE ptrace)