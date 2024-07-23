cmake_minimum_required(VERSION 3.5)
project(path_tracing VERSION 1.0)

file(GLOB IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends/imgui_impl_opengl3.cpp"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking)
set(IMGUI_BACKENDS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui-docking/backends)
set(GLFW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4/include)
set(GLFW_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4/src)
set(GLM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glm/include)

add_subdirectory(glfw-3.4)

set(SOURCES src/Renderer.cpp
            src/image/Image.cpp
            src/image/ImageSaver.cpp
            src/Application.cpp
            src/Camera.cpp
            src/sampling/BSDF.cpp
            src/assets/Model.cpp
            src/assets/ModelInstance.cpp
            src/assets/AssetLoader.cpp
            src/hittable/Polygon.cpp
            src/Entrypoint.cpp)

add_executable(ptrace ${SOURCES} ${IMGUI_SOURCES})
target_include_directories(ptrace PRIVATE ${IMGUI_DIR} ${GLFW_INCLUDE_DIR})
target_link_directories(ptrace PRIVATE ${GLFW_LIB_DIR})
target_link_libraries(ptrace PRIVATE glfw3 opengl32)

set(PTRACE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)