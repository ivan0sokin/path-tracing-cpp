cmake_minimum_required(VERSION 3.5)
project(with_the_lights_out)

add_executable(with_the_lights_out with_the_lights_out.cpp)
target_include_directories(with_the_lights_out PRIVATE ${IMGUI_DIR} ${PTRACE_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
target_link_directories(with_the_lights_out PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(with_the_lights_out PRIVATE ptrace)