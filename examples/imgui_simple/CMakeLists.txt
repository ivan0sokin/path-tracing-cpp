cmake_minimum_required(VERSION 3.5)
project(imgui)

message("GOIDA!")
message(${IMGUI_SOURCES})

add_executable(imgui imgui.cpp ${IMGUI_SOURCES})
target_include_directories(imgui PRIVATE ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR} ${GLFW_INCLUDE_DIR})
target_link_directories(imgui PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(imgui PRIVATE glfw3 opengl32 glad)