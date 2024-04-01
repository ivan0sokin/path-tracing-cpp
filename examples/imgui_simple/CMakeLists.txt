cmake_minimum_required(VERSION 3.5)
project(imgui_simple)

add_executable(imgui_simple imgui_simple.cpp ${IMGUI_SOURCES})
target_include_directories(imgui_simple PRIVATE ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR} ${GLFW_INCLUDE_DIR})
target_link_directories(imgui_simple PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(imgui_simple PRIVATE glfw3 opengl32)