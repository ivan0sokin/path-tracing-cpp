#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "../glfw-3.4/include/GLFW/glfw3.h"

#include "Scene.h"
#include "Camera.h"
#include "AccelerationStructure.h"
#include "Renderer.h"

#include <cstring>
#include <filesystem>

//! Path tracing application class which holds basic logic
class Application {
public:
    //! Creates new Application with initial window size
    Application(int windowWidth, int windowHeight) noexcept;

    //! Runs application
    int Run() noexcept;

private:
    void MainLoop() noexcept;

    void OnUpdate() noexcept;

    void ProcessSceneCollapsingHeaders() noexcept;

    void ProcessCameraCollapsingHeader() noexcept;

    void ProcessSpheresCollapsingHeader() noexcept;

    void ProcessTrianglesCollapsingHeader() noexcept;

    void ProcessBoxesCollapsingHeader() noexcept;

    void ProcessModelsCollapsingHeader() noexcept;

    void ProcessMaterialsCollapsingHeader() noexcept;

    void UpdateThemeStyle() noexcept;

    void LoadSceneFromFile(const std::filesystem::path &pathToFile) noexcept;

    void SaveSceneToFile(const std::filesystem::path &pathToFile) const noexcept;

    void UpdateObjects() noexcept;

    void UpdateLights() noexcept;

    void UpdateObjectMaterials() noexcept;

private:
    int m_InitialWindowWidth, m_InitialWindowHeight;
    int m_LastViewportWidth, m_LastViewportHeight;
    GLFWwindow *m_Window = nullptr;
    bool m_DarkTheme = false;

    int m_LastID;
    bool m_SomeObjectChanged;
    bool m_SomeGeometryChanged;

    double m_TotalRenderTime;
    double m_LastRenderTime;

    char *m_SaveImageFilePath;
    char *m_SceneFilePath;

    Scene m_Scene;
    AccelerationStructure m_AccelerationStructure;
    Renderer m_Renderer;

    Math::Vector3f m_RayMissColor;

    std::vector<int> m_SphereMaterialIndices;
    std::vector<int> m_TriangleMaterialIndices;
    std::vector<int> m_BoxMaterialIndices;

    std::vector<IHittable*> m_Objects;
    std::vector<Light> m_Lights;

    Material m_AddMaterial;
    Shapes::Sphere m_AddSphere;
    Shapes::Triangle m_AddTriangle;
    Shapes::Box m_AddBox;
    char *m_ModelFilePath;
    char *m_MaterialDirectory;

    int m_AddSphereMaterialIndex;
    int m_AddTriangleMaterialIndex;
    int m_AddBoxMaterialIndex;

    constexpr static const char *c_WindowTitle = "Path Tracing";
    constexpr static const char *c_DefaultScenePath = "assets/dft2.scn";
    constexpr static int c_AnyInputFilePathLength = 128;
};

#endif