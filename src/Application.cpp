#include "Application.h"
#include "Utilities.hpp"
#include "Timer.h"

#include "../imgui-docking/imgui.h"
#include "../imgui-docking/backends/imgui_impl_glfw.h"
#include "../imgui-docking/backends/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb-master/stb_image.h"

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

    m_SaveImageFilePath = new char[c_AnyInputFilePathLength];
    m_SceneFilePath = new char[c_AnyInputFilePathLength];
    m_ModelFilePath = new char[c_AnyInputFilePathLength];
    m_MaterialDirectory = new char[c_AnyInputFilePathLength];
    memset(m_SaveImageFilePath, 0, sizeof(m_SaveImageFilePath));
    memset(m_SceneFilePath, 0, sizeof(m_SceneFilePath));
    memset(m_ModelFilePath, 0, sizeof(m_ModelFilePath));
    memset(m_MaterialDirectory, 0, sizeof(m_MaterialDirectory));

    m_AddMaterial.albedo = Texture(Math::Vector3f(0.f));
    m_AddMaterial.metallic = Texture(Math::Vector3f(0.f));
    m_AddMaterial.specular = Texture(Math::Vector3f(0.f));
    m_AddMaterial.roughness = Texture(Math::Vector3f(1.f));
    m_AddMaterial.emissionPower = 0.f;
    m_AddMaterial.index = -1;

    m_AddSphere = Shapes::Sphere(Math::Vector3f(0.f), 0.f, nullptr);
    m_AddTriangle = Shapes::Triangle(Math::Vector3f(0.f), Math::Vector3f(0.f), Math::Vector3f(0.f), nullptr);
    m_AddBox = Shapes::Box(Math::Vector3f(0.f), Math::Vector3f(0.f), nullptr);

    m_AddSphereMaterialIndex = -1;
    m_AddTriangleMaterialIndex = -1;
    m_AddBoxMaterialIndex = -1;

    m_RayMissColor = Math::Vector3f(0.f);

    m_Scene.camera = Camera(windowWidth, windowHeight);

    m_AccelerationStructure = nullptr;
    m_ObjectsBLAS = nullptr;
    
    m_NonHittable = new NonHittable();
    std::array<IHittable*, 1> m_NonHittableArray = {m_NonHittable};
    m_NonHittableBLAS = new BLAS(new BVH(m_NonHittableArray));

    LoadSceneFromFile(c_DefaultScenePath);
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

    GLFWimage *icon = new GLFWimage();
    icon->pixels = stbi_load("icon/icon.png", &icon->width, &icon->height, 0, 4);
    glfwSetWindowIcon(m_Window, 1, icon);
    stbi_image_free(icon->pixels);
    delete icon;

    glfwMakeContextCurrent(m_Window);

    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    UpdateThemeStyle();

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

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

        ImGuiWindowFlags dockWindowFlags = ImGuiWindowFlags_NoDocking;
        dockWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        dockWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        {
            OnUpdate();
        }

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

        int width = -1, height = -1;
        glfwGetWindowSize(m_Window, &width, &height);

        if ((m_LastViewportWidth != viewportWidth || m_LastViewportHeight != viewportHeight) && width * height > 0) {
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
        m_LastID = 0;
        m_SomeObjectChanged = false;
        m_SomeGeometryChanged = false;

        ProcessSceneCollapsingHeaders();

        ImGui::Checkbox("Accumulate", Math::ValuePointer(m_Renderer.Accumulate()));
        ImGui::Checkbox("Accelerate", Math::ValuePointer(m_Renderer.Accelerate()));
        if (ImGui::InputInt("Used threads", Math::ValuePointer(m_Renderer.UsedThreadCount()))) {
            m_Renderer.SetUsedThreadCount(m_Renderer.UsedThreadCount());
        }
        ImGui::InputInt("Ray depth", Math::ValuePointer(m_Renderer.RayDepth()));
        ImGui::InputFloat("Gamma", Math::ValuePointer(m_Renderer.Gamma()));

        if (ImGui::ColorEdit3("Ray miss color", Math::ValuePointer(m_RayMissColor))) {
            m_Renderer.OnRayMiss([this](const Ray&){ return m_RayMissColor; });
        }

        if (ImGui::Button("Reset", {viewport->WorkSize.x * 0.05f, viewport->WorkSize.y * 0.1f}) || m_Renderer.Accumulate()) {
            m_Scene.camera.ComputeRayDirections();
            
            m_LastRenderTime = Timer::MeasureInMillis([this](){
                if (m_Renderer.Accelerate()) {
                    m_Renderer.Render(m_Scene.camera, m_AccelerationStructure, m_Lights, m_Scene.materials);
                } else {
                    m_Renderer.Render(m_Scene.camera, m_Objects, m_Lights, m_Scene.materials);
                }
            });

            if (m_Renderer.Accumulate()) {
                m_TotalRenderTime += m_LastRenderTime;
            } else {
                m_TotalRenderTime = 0.f;
            }
        }

        if (ImGui::Checkbox("Dark theme", Math::ValuePointer(m_DarkTheme))) {
            UpdateThemeStyle();
        }

        ImGui::InputText("##save_image", m_SaveImageFilePath, c_AnyInputFilePathLength);
        ImGui::SameLine();
        if (ImGui::Button("Save image")) {
            m_Renderer.SaveImage(m_SaveImageFilePath);
        }

        ImGui::InputText("##save_scene", m_SceneFilePath, c_AnyInputFilePathLength);
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

void Application::UpdateThemeStyle() noexcept {
    if (m_DarkTheme) {
        ImGui::StyleColorsDark();
    } else {
        ImGui::StyleColorsLight();
    }
}

void Application::ProcessSceneCollapsingHeaders() noexcept {
    ProcessCameraCollapsingHeader();
    ProcessSpheresCollapsingHeader();
    ProcessTrianglesCollapsingHeader();
    ProcessBoxesCollapsingHeader();
    ProcessModelsCollapsingHeader();
    ProcessMaterialsCollapsingHeader();

    if (m_SomeObjectChanged) {
        UpdateObjects();
    }

    if (m_SomeGeometryChanged) {
        UpdateTLAS();
    }
}

void Application::ProcessCameraCollapsingHeader() noexcept {
    ImGui::PushID(m_LastID++);

    if (ImGui::CollapsingHeader("Camera", nullptr)) {
        bool cameraNeedUpdate = false;
        ImGui::InputFloat3("Position", Math::ValuePointer(m_Scene.camera.Position()));
        ImGui::InputFloat3("Target", Math::ValuePointer(m_Scene.camera.Target()));
        ImGui::InputFloat("Vertical FOV", Math::ValuePointer(m_Scene.camera.VerticalFovInDegrees()));
        ImGui::InputFloat3("Up", Math::ValuePointer(m_Scene.camera.Up()));
    }

    ImGui::PopID();
}

void Application::ProcessSpheresCollapsingHeader() noexcept {
    int deleteIndex = -1;
    if (ImGui::CollapsingHeader("Spheres", nullptr)) {
        for (int i = 0; i < (int)m_Scene.spheres.size(); ++i) {
            ImGui::PushID(m_LastID++);

            Shapes::Sphere &sphere = m_Scene.spheres[i];
            ImGui::Text("Sphere %d:", i);

            if (ImGui::Button("Delete")) {
                deleteIndex = i;
            }

            if (ImGui::InputFloat("Radius", Math::ValuePointer(sphere.radius))) {
                sphere = Shapes::Sphere(sphere.center, sphere.radius, sphere.material);
                m_SomeGeometryChanged = true;
            }

            if (ImGui::InputFloat3("Position", Math::ValuePointer(sphere.center))) {
                m_SomeGeometryChanged = true;
            }

            if (ImGui::InputInt("Material index", Math::ValuePointer(m_SphereMaterialIndices[i]))) {
                sphere.material = &m_Scene.materials[m_SphereMaterialIndices[i]];
            }

            ImGui::PopID();
        }

        ImGui::PushID(m_LastID++);

        if (ImGui::Button("Add")) {
            m_Scene.spheres.push_back(m_AddSphere);
            m_SomeObjectChanged = true;
            m_SomeGeometryChanged = true;
        }
        
        if (ImGui::InputFloat("Radius", Math::ValuePointer(m_AddSphere.radius))) {
            m_AddSphere = Shapes::Sphere(m_AddSphere.center, m_AddSphere.radius, m_AddSphere.material);
        }

        ImGui::InputFloat3("Position", Math::ValuePointer(m_AddSphere.center));

        if (ImGui::InputInt("Material index", Math::ValuePointer(m_AddSphereMaterialIndex))) {
            m_AddSphere.material = &m_Scene.materials[m_AddSphereMaterialIndex];
        }

        ImGui::PopID();

        if (deleteIndex >= 0) {
            m_Scene.spheres.erase(m_Scene.spheres.cbegin() + deleteIndex);
            m_SomeObjectChanged = true;
            m_SomeGeometryChanged = true;
        }
    }
}

void Application::ProcessTrianglesCollapsingHeader() noexcept {
    int deleteIndex = -1;
    if (ImGui::CollapsingHeader("Triangles", nullptr)) {
        for (int i = 0; i < (int)m_Scene.triangles.size(); ++i) {
            ImGui::PushID(m_LastID++);

            Shapes::Triangle &triangle = m_Scene.triangles[i];
            ImGui::Text("Triangle %d:", i);
            
            if (ImGui::Button("Delete")) {
                deleteIndex = i;
            }

            if (ImGui::InputFloat3("Vertex 0", Math::ValuePointer(triangle.vertices[0]))) {
                triangle = Shapes::Triangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], triangle.material);
                m_SomeGeometryChanged = true;
            }
            if (ImGui::InputFloat3("Vertex 1", Math::ValuePointer(triangle.vertices[1]))) {
                triangle = Shapes::Triangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], triangle.material);
                m_SomeGeometryChanged = true;
            }
            if (ImGui::InputFloat3("Vertex 2", Math::ValuePointer(triangle.vertices[2]))) {
                triangle = Shapes::Triangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2], triangle.material);
                m_SomeGeometryChanged = true;
            }

            if (ImGui::InputInt("Material index", Math::ValuePointer(m_TriangleMaterialIndices[i]))) {
                triangle.material = &m_Scene.materials[m_TriangleMaterialIndices[i]];
            }

            ImGui::PopID();
        }

        ImGui::PushID(m_LastID++);

        if (ImGui::Button("Add")) {
            m_Scene.triangles.push_back(m_AddTriangle);
            m_SomeObjectChanged = true;
            m_SomeGeometryChanged = true;
        }
        
        if (ImGui::InputFloat3("Vertex 0", Math::ValuePointer(m_AddTriangle.vertices[0]))) {
            m_AddTriangle = Shapes::Triangle(m_AddTriangle.vertices[0], m_AddTriangle.vertices[1], m_AddTriangle.vertices[2], m_AddTriangle.material);
        }
        if (ImGui::InputFloat3("Vertex 1", Math::ValuePointer(m_AddTriangle.vertices[1]))) {
            m_AddTriangle = Shapes::Triangle(m_AddTriangle.vertices[0], m_AddTriangle.vertices[1], m_AddTriangle.vertices[2], m_AddTriangle.material);
        }
        if (ImGui::InputFloat3("Vertex 2", Math::ValuePointer(m_AddTriangle.vertices[2]))) {
            m_AddTriangle = Shapes::Triangle(m_AddTriangle.vertices[0], m_AddTriangle.vertices[1], m_AddTriangle.vertices[2], m_AddTriangle.material);
        }

        if (ImGui::InputInt("Material index", Math::ValuePointer(m_AddTriangleMaterialIndex))) {
            m_AddTriangle.material = &m_Scene.materials[m_AddTriangleMaterialIndex];
        }

        ImGui::PopID();

        if (deleteIndex >= 0) {
            m_Scene.triangles.erase(m_Scene.triangles.cbegin() + deleteIndex);
            m_SomeObjectChanged = true;
            m_SomeGeometryChanged = true;
        }
    }
}

void Application::ProcessBoxesCollapsingHeader() noexcept {
    int deleteIndex = -1;
    if (ImGui::CollapsingHeader("Boxes", nullptr)) {
        for (int i = 0; i < (int)m_Scene.boxes.size(); ++i) {
            ImGui::PushID(m_LastID++);

            ImGui::Text("Box: %d", i);

            if (ImGui::Button("Delete")) {
                deleteIndex = i;
            }

            Shapes::Box &box = m_Scene.boxes[i];

            if (ImGui::InputFloat3("First corner", Math::ValuePointer(box.min))) {
                box = Shapes::Box(box.min, box.max, box.material);
                m_SomeGeometryChanged = true;
            }

            if (ImGui::InputFloat3("Second corner", Math::ValuePointer(box.max))) {
                box = Shapes::Box(box.min, box.max, box.material);
                m_SomeGeometryChanged = true;
            }

            if (ImGui::InputInt("Material index", Math::ValuePointer(m_BoxMaterialIndices[i]))) {
                box = Shapes::Box(box.min, box.max, &m_Scene.materials[m_BoxMaterialIndices[i]]);
            }

            ImGui::PopID();
        }

        ImGui::PushID(m_LastID++);

        if (ImGui::Button("Add")) {
            m_Scene.boxes.push_back(m_AddBox);
            m_SomeObjectChanged = true;
            m_SomeGeometryChanged = true;
        }
        
        if (ImGui::InputFloat3("First corner", Math::ValuePointer(m_AddBox.min))) {
            m_AddBox = Shapes::Box(m_AddBox.min, m_AddBox.max, m_AddBox.material);
        }
        if (ImGui::InputFloat3("Second corner", Math::ValuePointer(m_AddBox.max))) {
            m_AddBox = Shapes::Box(m_AddBox.min, m_AddBox.max, m_AddBox.material);
        }

        if (ImGui::InputInt("Material index", Math::ValuePointer(m_AddBoxMaterialIndex))) {
            m_AddBox.material = &m_Scene.materials[m_AddBoxMaterialIndex];
        }

        ImGui::PopID();

        if (deleteIndex >= 0) {
            m_Scene.boxes.erase(m_Scene.boxes.cbegin() + deleteIndex);
            m_SomeObjectChanged = true;
            m_SomeGeometryChanged = true;
        }
    }
}

void Application::ProcessModelsCollapsingHeader() noexcept {
    int deleteIndex = -1;
    int cloneIndex = -1;
    if (ImGui::CollapsingHeader("Models", nullptr)) {
        for (int i = 0; i < (int)m_Scene.modelInstances.size(); ++i) {
            ImGui::PushID(m_LastID++);

            ImGui::Text("Model: %d", i);

            if (ImGui::Button("Delete")) {
                deleteIndex = i;
            }

            auto modelInstance = m_Scene.modelInstances[i];
            
            if (ImGui::Button("Clone")) {
                cloneIndex = i;
            }
            
            if (ImGui::InputFloat3("Translation", Math::ValuePointer(modelInstance->Translation()))) {
                modelInstance->Update();
                m_SomeGeometryChanged = true;
            }

            if (ImGui::InputFloat3("Angles", Math::ValuePointer(modelInstance->Angles()))) {
                modelInstance->Update();
                m_SomeGeometryChanged = true;
            }

            ImGui::PopID();
        }

        ImGui::PushID(m_LastID++);

        if (ImGui::Button("Import")) {
            ModelInstance *modelInstance;
            AssetLoader::Result result;
            double loadTime = Timer::MeasureInMillis([&](){
                auto [instance, res] = AssetLoader::Instance().LoadOBJ(m_ModelFilePath, m_MaterialDirectory);
                modelInstance = instance;
                result = res;
            });
            
            if (!result.warning.empty()) {
                std::cout << "Warnings occured while loading model " << m_ModelFilePath << " with material directory: " << m_MaterialDirectory << ": " << result.warning << '\n';
            }

            if (result.IsFailure()) {
                std::cerr << "Failed to import model " << m_ModelFilePath << " with material directory: " << m_MaterialDirectory << '\n';
            } else {
                std::cout << "Loaded model " << m_ModelFilePath << " with material directory: " << m_MaterialDirectory << "in " << loadTime << "ms\n";

                m_Scene.modelInstances.push_back(modelInstance);
                m_SomeObjectChanged = true;
                m_SomeGeometryChanged = true;
            }
        }
        
        ImGui::InputText("Path (.obj)", m_ModelFilePath, c_AnyInputFilePathLength);
        ImGui::InputText("Material folder", m_MaterialDirectory, c_AnyInputFilePathLength);

        ImGui::PopID();

        if (deleteIndex >= 0) {
            delete m_Scene.modelInstances[deleteIndex];
            m_Scene.modelInstances.erase(m_Scene.modelInstances.cbegin() + deleteIndex);
 
            m_SomeObjectChanged = true;
            m_SomeGeometryChanged = true;
        }

        if (deleteIndex != cloneIndex && cloneIndex >= 0) {
            m_Scene.modelInstances.push_back(m_Scene.modelInstances[cloneIndex]->Clone());
        }
    }
}

void Application::ProcessMaterialsCollapsingHeader() noexcept {
    int deleteIndex = -1;
    if (ImGui::CollapsingHeader("Materials", nullptr)) {
        for (int i = 0; i < (int)m_Scene.materials.size(); ++i) {
            ImGui::PushID(m_LastID++);

            Material &material = m_Scene.materials[i];
            ImGui::Text("Material %d:", material.index);

            if (ImGui::Button("Delete")) {
                deleteIndex = i;
            }

            ImGui::ColorEdit3("Albedo", material.albedo.GetData());
            ImGui::InputFloat("Emission power", Math::ValuePointer(material.emissionPower));
            ImGui::InputFloat("Metallic", material.metallic.GetData());
            ImGui::InputFloat("Roughness", material.roughness.GetData());
            ImGui::InputFloat("Specular", material.specular.GetData());
            
            ImGui::InputFloat("Transparency", &material.transparency);
            ImGui::InputFloat("Refraction", &material.refractionIndex);

            ImGui::PopID();
        }

        ImGui::PushID(m_LastID++);

        if (ImGui::Button("Add")) {
            int maxIndex = -1;
            for (const auto &material : m_Scene.materials) {
                maxIndex = Math::Max(maxIndex, material.index);
            }

            m_AddMaterial.index = maxIndex + 1;
            m_Scene.materials.push_back(m_AddMaterial);
            UpdateObjectMaterials();
        }

        ImGui::ColorEdit3("Albedo", m_AddMaterial.albedo.GetData());
        ImGui::InputFloat("Emission power", Math::ValuePointer(m_AddMaterial.emissionPower));
        ImGui::InputFloat("Metallic", m_AddMaterial.metallic.GetData());
        ImGui::InputFloat("Roughness", m_AddMaterial.roughness.GetData());
        ImGui::InputFloat("Specular", m_AddMaterial.specular.GetData());

        ImGui::PopID();

        if (deleteIndex >= 0) {
            m_Scene.materials.erase(m_Scene.materials.cbegin() + deleteIndex);
        }
    }
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
    } else {
        std::cout << "Loaded scene: " << pathToFile << '\n';
    }

    UpdateObjects();
    UpdateLights();
    UpdateTLAS();    
}

void Application::UpdateTLAS() noexcept {
    if (m_AccelerationStructure != nullptr) {
        delete m_AccelerationStructure;
        m_AccelerationStructure = nullptr;
    }

    std::vector<BLAS*> blas;
    if (m_ObjectsBLAS != nullptr) {
        blas.push_back(m_ObjectsBLAS);
    }

    for (auto modelInstance : m_Scene.modelInstances) {
        blas.push_back(modelInstance->GetBLAS());
    }

    if (blas.empty()) {
        blas.push_back(m_NonHittableBLAS);
    }

    m_AccelerationStructure = new TLAS(blas);
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

    m_SphereMaterialIndices.clear();
    for (auto &sphere : m_Scene.spheres) {
        m_Objects.push_back(&sphere);
        m_SphereMaterialIndices.push_back(sphere.material->index);
    }

    m_TriangleMaterialIndices.clear();
    for (auto &triangle : m_Scene.triangles) {
        m_Objects.push_back(&triangle);
        m_TriangleMaterialIndices.push_back(triangle.material->index);
    }

    m_BoxMaterialIndices.clear();
    for (auto &box : m_Scene.boxes) {
        m_Objects.push_back(&box);
        m_BoxMaterialIndices.push_back(box.material->index);
    }

    if (m_ObjectsBLAS != nullptr) {
        delete m_ObjectsBLAS->GetBVH();
        delete m_ObjectsBLAS;
        m_ObjectsBLAS = nullptr;
    }

    if (m_Objects.empty()) {
        return;
    }

    BVH *bvh = new BVH(m_Objects);
    m_ObjectsBLAS = new BLAS(bvh);
}

void Application::UpdateLights() noexcept {
    m_Lights.clear();
    
    for (auto &sphere : m_Scene.spheres) {
        if (sphere.material->emissionPower > 0.f) {
            m_Lights.emplace_back(&sphere);
        }
    }

    for (auto &triangle : m_Scene.triangles) {
        if (triangle.material->emissionPower > 0.f) {
            m_Lights.emplace_back(&triangle);
        }
    }

    for (auto &box : m_Scene.boxes) {
        if (box.material->emissionPower > 0.f) {
            m_Lights.emplace_back(&box);
        }
    }

    // for (auto model : m_Scene.models) {
    //     auto lights = model->GetLightSources();
    //     m_Lights.insert(m_Lights.cend(), lights.begin(), lights.end());
    // }
}

void Application::UpdateObjectMaterials() noexcept {
    int sphereCount = static_cast<int>(m_Scene.spheres.size());
    for (int i = 0; i < sphereCount; ++i) {
        m_Scene.spheres[i].material = &*std::find_if(m_Scene.materials.cbegin(), m_Scene.materials.cend(), [this, i](const auto &material) {
            return material.index == m_SphereMaterialIndices[i];
        });
    }

    int triangleCount = static_cast<int>(m_Scene.triangles.size());
    for (int i = 0; i < triangleCount; ++i) {
        m_Scene.triangles[i].material = &*std::find_if(m_Scene.materials.cbegin(), m_Scene.materials.cend(), [this, i](const auto &material) {
            return material.index == m_TriangleMaterialIndices[i];
        });
    }

    int boxCount = static_cast<int>(m_Scene.boxes.size());
    for (int i = 0; i < boxCount; ++i) {
        m_Scene.boxes[i].material = &*std::find_if(m_Scene.materials.cbegin(), m_Scene.materials.cend(), [this, i](const auto &material) {
            return material.index == m_BoxMaterialIndices[i];
        });
    }
}