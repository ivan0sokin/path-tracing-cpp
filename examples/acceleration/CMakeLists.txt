cmake_minimum_required(VERSION 3.5)
project(acceleration)

add_executable(acceleration acceleration.cpp)
target_include_directories(acceleration PRIVATE ${IMGUI_DIR} ${PTRACE_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
target_link_directories(acceleration PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(acceleration PRIVATE ptrace)