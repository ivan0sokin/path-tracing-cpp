#include "Application.h"
#include "Utilities.hpp"

#include "../imgui-docking/imgui.h"
#include "../imgui-docking/backends/imgui_impl_glfw.h"
#include "../imgui-docking/backends/imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <iostream>
#include <chrono>
#include <fstream>

Application::Application(int windowWidth, int windowHeight) noexcept :
    m_InitialWindowWidth(windowWidth), m_InitialWindowHeight(windowHeight),
    m_LastViewportWidth(-1), m_LastViewportHeight(-1),
    m_Renderer(windowWidth, windowHeight),
    m_TotalRenderTime(0.f), m_LastRenderTime(0.f) {
    m_SaveImageFilePath = new char[c_AnyInputFileSize];
    m_SceneFilePath = new char[c_AnyInputFileSize];
    memset(m_SaveImageFilePath, 0, sizeof(m_SaveImageFilePath));
    memset(m_SceneFilePath, 0, sizeof(m_SceneFilePath));

    m_Scene.camera = Camera(windowWidth, windowHeight);
}

int Application::Run() noexcept {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

#if defined(IMGUI_IMPL_OPENGL_ES2)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    m_Window = glfwCreateWindow(m_InitialWindowWidth, m_InitialWindowHeight, c_WindowTitle, nullptr, nullptr);
    if (m_Window == nullptr) {
        std::cerr << "Failed to create window\n";
        return -1;
    }

    glfwMakeContextCurrent(m_Window);

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    LoadSceneFromFile(c_DefaultScenePath);

    MainLoop();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_Window);
    glfwTerminate();

    return 0;
}

void Application::MainLoop() noexcept {
    while (!glfwWindowShouldClose(m_Window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        ImGuiWindowFlags dockWindowFlags = ImGuiWindowFlags_NoDocking;
        dockWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        dockWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Dock", nullptr, dockWindowFlags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("Dockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        {
            OnUpdate();
        }

        ImGui::End();

        ImGui::Render();
        ImGui::UpdatePlatformWindows();
        
        int display_w, display_h;
        glfwGetFramebufferSize(m_Window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(m_Window);
    }
}

void Application::OnUpdate() noexcept {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiWindowFlags frameFlags = ImGuiViewportFlags_IsPlatformWindow
                                | ImGuiViewportFlags_NoDecoration
                                | ImGuiViewportFlags_NoTaskBarIcon
                                | ImGuiViewportFlags_NoAutoMerge
                                | ImGuiWindowFlags_NoCollapse
                                | ImGuiWindowFlags_NoResize;

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize({viewport->WorkSize.x * 0.7f, viewport->WorkSize.y});
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::Begin("Frame", nullptr, frameFlags);
    {
        int viewportWidth = (int)ImGui::GetContentRegionAvail().x;
        int viewportHeight = (int)ImGui::GetContentRegionAvail().y;

        if (m_LastViewportWidth != viewportWidth || m_LastViewportHeight != viewportHeight) {
            m_LastViewportWidth = viewportWidth;
            m_LastViewportHeight = viewportHeight;

            m_Scene.camera.OnViewportResize(m_LastViewportWidth, m_LastViewportHeight);

            m_Renderer.OnResize(m_LastViewportWidth, m_LastViewportHeight);
        }
    

        Image *image = m_Renderer.GetImage();
        if (image != nullptr) {
            ImGui::Image((void*)(intptr_t)image->GetDescriptor(), ImGui::GetContentRegionAvail());
        }
    }
    ImGui::End();
    
    ImGui::SetNextWindowPos({viewport->WorkPos.x + viewport->WorkSize.x * 0.7f, viewport->WorkPos.y});
    ImGui::SetNextWindowSize({viewport->WorkSize.x * 0.3f, viewport->WorkSize.y});
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoTitleBar);
    {
        if (ImGui::CollapsingHeader("Camera", nullptr)) {
            bool cameraNeedUpdate = false;
            if (ImGui::InputFloat3("Position", Math::ValuePointer(m_Scene.camera.Position()))) {
                cameraNeedUpdate = true;
            }
            if (ImGui::InputFloat3("Target", Math::ValuePointer(m_Scene.camera.Target()))) {
                cameraNeedUpdate = true;
            }
            if (ImGui::InputFloat("Vertical FOV", &m_Scene.camera.VerticalFovInDegrees())) {
                cameraNeedUpdate = true;
            }
            if (ImGui::InputFloat3("Up", Math::ValuePointer(m_Scene.camera.Up()))) {
                cameraNeedUpdate = true;
            }

            if (cameraNeedUpdate) {
                m_Scene.camera.OnViewportResize(m_LastViewportWidth, m_LastViewportHeight);
            }
        }

        if (ImGui::CollapsingHeader("Scene", nullptr)) {
            if (ImGui::CollapsingHeader("Spheres", nullptr)) {
                for (int i = 0; i < (int)m_Scene.spheres.size(); ++i) {
                    ImGui::PushID(i);

                    Shapes::Sphere &sphere = m_Scene.spheres[i];
                    ImGui::Text("Sphere %d:", i);

                    if (ImGui::InputFloat("Radius", &sphere.radius)) {
                        sphere = Shapes::Sphere(sphere.center, sphere.radius, sphere.materialIndex);
                    }

                    ImGui::InputFloat3("Position", Math::ValuePointer(sphere.center));
                    ImGui::InputInt("Material index", Math::ValuePointer(sphere.materialIndex));

                    ImGui::PopID();
                }
            }

            if (ImGui::CollapsingHeader("Triangles", nullptr)) {
                for (int i = 0; i < (int)m_Scene.triangles.size(); ++i) {
                    ImGui::PushID(i);

                    Shapes::Triangle &triangle = m_Scene.triangles[i];
                    ImGui::Text("Triangle %d:", i);
                    
                    if (ImGui::InputFloat3("Vertex 0", Math::ValuePointer(triangle.vertices[0]))) {
                        triangle = Shapes::Triangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], triangle.materialIndex);
                    }
                    if (ImGui::InputFloat3("Vertex 1", Math::ValuePointer(triangle.vertices[1]))) {
                        triangle = Shapes::Triangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], triangle.materialIndex);
                    }
                    if (ImGui::InputFloat3("Vertex 2", Math::ValuePointer(triangle.vertices[2]))) {
                        triangle = Shapes::Triangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], triangle.materialIndex);
                    }

                    ImGui::InputInt("Material index", Math::ValuePointer(triangle.materialIndex));

                    ImGui::PopID();
                }
            }

            if (ImGui::CollapsingHeader("Boxes", nullptr)) {
                for (int i = 0; i < (int)m_Scene.boxes.size(); ++i) {
                    ImGui::PushID(i);

                    Shapes::Box &box = m_Scene.boxes[i];
                    ImGui::InputInt("Material index", Math::ValuePointer(box.materialIndex));

                    ImGui::PopID();
                }
            }
        }

        if (ImGui::CollapsingHeader("Materials", nullptr)) {
            for (int i = 0; i < (int)m_Scene.materials.size(); ++i) {
                ImGui::PushID(i);

                Material &material = m_Scene.materials[i];
                ImGui::Text("Material %d:", i);
                ImGui::ColorEdit3("Albedo", Math::ValuePointer(material.albedo));
                ImGui::InputFloat("Emission power", &material.emissionPower);
                ImGui::InputFloat("Metallic", &material.metallic);
                ImGui::InputFloat("Roughness", &material.roughness);
                ImGui::InputFloat("Specular", &material.specular);

                ImGui::PopID();
            }
        }

        ImGui::Checkbox("Accumulate", Math::ValuePointer(m_Renderer.Accumulate()));
        ImGui::Checkbox("Accelerate", Math::ValuePointer(m_Renderer.Accelerate()));

        if (ImGui::InputInt("Used threads", Math::ValuePointer(m_Renderer.UsedThreadCount()))) {
            m_Renderer.SetUsedThreadCount(m_Renderer.UsedThreadCount());
        }

        ImGui::InputInt("Ray depth", Math::ValuePointer(m_Renderer.RayDepth()));

        ImGui::InputFloat("Gamma", Math::ValuePointer(m_Renderer.Gamma()));

        if (ImGui::Button("Reset", {viewport->WorkSize.x * 0.05f, viewport->WorkSize.y * 0.1f}) || m_Renderer.Accumulate()) {
            auto t1 = std::chrono::high_resolution_clock::now();
            if (m_Renderer.Accelerate()) {
                m_Renderer.Render(m_Scene.camera, m_AccelerationStructure, m_Scene.materials);
            } else {
                m_Renderer.Render(m_Scene.camera, m_Objects, m_Scene.materials);
            }
            auto t2 = std::chrono::high_resolution_clock::now();
            
            m_LastRenderTime = static_cast<std::chrono::duration<float, std::milli>>(t2 - t1).count();
            if (m_Renderer.Accumulate()) {
                m_TotalRenderTime += m_LastRenderTime;
            } else {
                m_TotalRenderTime = 0.f;
            }
        }

        ImGui::InputText("##save_image", m_SaveImageFilePath, c_AnyInputFileSize);
        ImGui::SameLine();
        if (ImGui::Button("Save image")) {
            m_Renderer.SaveImage(m_SaveImageFilePath);
        }

        ImGui::InputText("##save_scene", m_SceneFilePath, c_AnyInputFileSize);
        if (ImGui::Button("Save scene")) {
            SaveSceneToFile(m_SceneFilePath);
        }

        ImGui::SameLine();
        if (ImGui::Button("Load scene")) {
            LoadSceneFromFile(m_SceneFilePath);
        }

        ImGui::Text("Last render time: %fms", m_LastRenderTime);
        ImGui::Text("Average render time: %fms", m_TotalRenderTime / (Math::Max(m_Renderer.GetFrameIndex() - 1, 1)));
        ImGui::Text("Accumulated frame count: %d", Math::Max(m_Renderer.GetFrameIndex() - 1, 1));
    }

    ImGui::End();
}

void Application::LoadSceneFromFile(const std::filesystem::path &pathToFile) noexcept {
    std::ifstream fileStream(pathToFile, std::ios::binary);
    if (!fileStream) {
        std::cerr << "Failed to open file: " << pathToFile << '\n';
        return;
    }

    auto error = m_Scene.Deserialize(fileStream);
    if (error.has_value()) {
        std::cerr << "Failed to deserialize scene: " << pathToFile << '\n';
        return;
    }

    // update objects and acceleration structure

    UpdateObjects();
}

void Application::SaveSceneToFile(const std::filesystem::path &pathToFile) const noexcept {
    std::ofstream fileStream(pathToFile, std::ios::binary);
    if (!fileStream) {
        std::cerr << "Failed to open file: " << pathToFile << '\n';
        return;
    }

    auto error = m_Scene.Serialize(fileStream);
    if (error.has_value()) {
        std::cerr << "Failed to serialize scene: " << pathToFile << '\n';
    }
}

void Application::UpdateObjects() noexcept {
    m_Objects.clear();
    m_Objects.shrink_to_fit();

    for (auto &sphere : m_Scene.spheres) {
        m_Objects.push_back(&sphere);
    }

    for (auto &triangle : m_Scene.triangles) {
        m_Objects.push_back(&triangle);
    }

    for (auto &box : m_Scene.boxes) {
        m_Objects.push_back(&box);
    }
}