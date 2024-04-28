#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "../glfw-3.4/include/GLFW/glfw3.h"

#include "Scene.h"
#include "Camera.h"
#include "AccelerationStructure.h"
#include "Renderer.h"

#include <cstring>
#include <filesystem>

class Application {
public:
    Application(int windowWidth, int windowHeight) noexcept;

    int Run() noexcept;

private:
    void MainLoop() noexcept;

    void OnUpdate() noexcept;

    void LoadSceneFromFile(const std::filesystem::path &pathToFile) noexcept;

    void SaveSceneToFile(const std::filesystem::path &pathToFile) const noexcept;

    void UpdateObjects() noexcept;

private:
    int m_InitialWindowWidth, m_InitialWindowHeight;
    int m_LastViewportWidth, m_LastViewportHeight;
    GLFWwindow *m_Window = nullptr;

    float m_TotalRenderTime;
    float m_LastRenderTime;

    char *m_SaveImageFilePath;
    char *m_SceneFilePath;

    Scene m_Scene;
    AccelerationStructure m_AccelerationStructure;
    Renderer m_Renderer;

    std::vector<HittableObject*> m_Objects;

    constexpr static const char *c_WindowTitle = "Path Tracing";
    constexpr static const char *c_DefaultScenePath = "assets/default.scn";
    constexpr static int c_AnyInputFileSize = 128;
};

#endif