cmake_minimum_required(VERSION 3.5)
project(obj)

add_executable(obj obj.cpp)
target_include_directories(obj PRIVATE ${IMGUI_DIR} ${PTRACE_INCLUDE_DIR} ${GLM_INCLUDE_DIR})
target_link_directories(obj PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(obj PRIVATE ptrace)