cmake_minimum_required(VERSION 3.5)
project(imgui_image)

add_executable(imgui_image imgui_image.cpp ${IMGUI_SOURCES})
target_include_directories(imgui_image PRIVATE ${IMGUI_DIR} ${IMGUI_BACKENDS_DIR} ${GLFW_INCLUDE_DIR} ${GLAD_INCLUDE_DIR})
target_link_directories(imgui_image PRIVATE ${GLFW_LIB_DIR} ${GLAD_LIB_DIR})
target_link_libraries(imgui_image PRIVATE glfw3 glad opengl32)