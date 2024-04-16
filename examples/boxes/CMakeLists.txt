cmake_minimum_required(VERSION 3.5)
project(boxes)

add_executable(boxes boxes.cpp)
target_include_directories(boxes PRIVATE ${IMGUI_DIR} ${PTRACE_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
target_link_directories(boxes PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(boxes PRIVATE ptrace)