#include <imgui.h>
#include <Application.h>
#include <Renderer.h>
#include <chrono>
#include <algorithm>

#include <glm/gtc/type_ptr.hpp>

#define dbg(msg) fprintf(stdout, (msg));

int main() {
    Application app(1580, 720, "Triangles");

    glm::vec3 position(-277.5f, 277.5f, 800.f);
    glm::vec3 target(-277.5f, 277.5f, 0.f);
    float verticalFovInDegrees = 40.f;
    glm::vec3 up(0.f, 1.f, 0.f);
    Camera camera(1580, 720, position, target, glm::radians(verticalFovInDegrees), up);
    
    Renderer renderer(1580, 720);
    renderer.OnRayMiss([](const Ray&){ return glm::vec3(0.1f, 0.2f, 0.5f);});

    Scene scene;
    scene.triangles.emplace_back(glm::vec3{-555.f, 0.f, 0.f}, glm::vec3{-555.f, 555.f, 0.f}, glm::vec3{-555.f, 0.f, -555.f}, 0);
    scene.triangles.emplace_back(glm::vec3{-555.f, 555.f, 0.f}, glm::vec3{-555.f, 555.f, -555.f}, glm::vec3{-555.f, 0.f, -555.f}, 0); // left
    scene.triangles.emplace_back(glm::vec3{-555.f, 0.f, 0.f}, glm::vec3{-555.f, 0.f, -555.f}, glm::vec3{0.f, 0.f, -555.f}, 1);
    scene.triangles.emplace_back(glm::vec3{0.f, 0.f, -555.f}, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{-555.f, 0.f, 0.f}, 1); // bottom
    scene.triangles.emplace_back(glm::vec3{-555.f, 0.f, -555.f}, glm::vec3{-555.f, 555.f, -555.f}, glm::vec3{0.f, 555.f, -555.f}, 1);
    scene.triangles.emplace_back(glm::vec3{0.f, 555.f, -555.f}, glm::vec3{0.f, 0.f, -555.f}, glm::vec3{-555.f, 0.f, -555.f}, 1); // forward
    scene.triangles.emplace_back(glm::vec3{-555.f, 555.f, 0.f}, glm::vec3{-555.f, 555.f, -555.f}, glm::vec3{0.f, 555.f, -555.f}, 1);
    scene.triangles.emplace_back(glm::vec3{0.f, 555.f, -555.f}, glm::vec3{0.f, 555.f, 0.f}, glm::vec3{-555.f, 555.f, 0.f}, 1); // up
    scene.triangles.emplace_back(glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 0.f, -555.f}, glm::vec3{0.f, 555.f, -555.f}, 2);
    scene.triangles.emplace_back(glm::vec3{0.f, 555.f, -555.f}, glm::vec3{0.f, 555.f, 0.f}, glm::vec3{0.f, 0.f, 0.f}, 2); // right
    scene.triangles.emplace_back(glm::vec3{-343.f, 554.f, -332.f + 105.f}, glm::vec3{-343.f, 554.f, -332.f}, glm::vec3{-343.f + 130.f, 554.f, -332.f}, 3);
    scene.triangles.emplace_back(glm::vec3{-343.f + 130.f, 554.f, -332.f}, glm::vec3{-343.f + 130.f, 554.f, -332.f + 105.f}, glm::vec3{-343.f, 554.f, -332.f + 105.f}, 3); // top light

    scene.spheres.emplace_back(glm::vec3{-400.0, 50.5, -400.5}, 100.f, 4);

    Material green;
    green.albedo = {0.12f, 0.45f, 0.15f};
    scene.materials.push_back(green);
    
    Material white;
    white.albedo = {0.73f, 0.73f, 0.73f};
    scene.materials.push_back(white);

    Material red;
    red.albedo = {0.65f, 0.05f, 0.05f};
    scene.materials.push_back(red);

    Material light;
    light.albedo = {1.f, 1.f, 1.f};
    light.emissionColor = light.albedo;
    light.emissionPower = 30.f;
    scene.materials.push_back(light);

    Material reflective;
    reflective.albedo = {1.f, 1.f, 1.f};
    reflective.reflectance = 1.f;
    scene.materials.push_back(reflective);

    bool accumulate = false;
    float lastRenderTime = 0.f;
    float totalRenderTime = 0.f;

    char *filename = new char[128];
    memset(filename, 0, sizeof(char) * sizeof(filename));

    int rayDepth = renderer.GetMaxRayDepth();
    int usedThreads = renderer.GetUsedThreadCount();
    float gamma = renderer.GetGamma();

    app.SetOnUpdate([&](){
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGuiWindowFlags frameFlags = ImGuiViewportFlags_IsPlatformWindow
                                    | ImGuiViewportFlags_NoDecoration
                                    | ImGuiViewportFlags_NoTaskBarIcon
                                    | ImGuiViewportFlags_NoAutoMerge
                                    | ImGuiWindowFlags_NoCollapse
                                    | ImGuiWindowFlags_NoResize;

        int viewportWidth = (int)(viewport->WorkSize.x * 0.7f);
        int viewportHeight = (int)(viewport->WorkSize.y);

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize({viewport->WorkSize.x * 0.7f, viewport->WorkSize.y});
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::Begin("Frame", nullptr, frameFlags);
        {
            viewportWidth = (int)ImGui::GetContentRegionAvail().x;
            viewportHeight = (int)ImGui::GetContentRegionAvail().y;
            Image *image = renderer.GetImage();
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
                if (ImGui::InputFloat3("Position", glm::value_ptr(position))) {
                    camera.SetPosition(position);
                }
                if (ImGui::InputFloat3("Target", glm::value_ptr(target))) {
                    camera.SetTarget(target);
                }
                if (ImGui::InputFloat("Vertical FOV", &verticalFovInDegrees)) {
                    camera.SetVerticalFovInRadians(glm::radians(verticalFovInDegrees));
                }
                if (ImGui::InputFloat3("Up", glm::value_ptr(up))) {
                    camera.SetUp(up);
                }
            }

            if (ImGui::CollapsingHeader("Scene", nullptr)) {
                if (ImGui::CollapsingHeader("Spheres", nullptr)) {
                    for (int i = 0; i < (int)scene.spheres.size(); ++i) {
                        ImGui::PushID(i);

                        Sphere &sphere = scene.spheres[i];
                        ImGui::Text("Sphere %d:", i);
                        if (ImGui::InputFloat("Radius", &sphere.radius)) {
                            sphere.radiusSquared = sphere.radius * sphere.radius;
                            sphere.inverseRadius = 1.f / sphere.inverseRadius;
                        }
                        ImGui::InputFloat3("Position", glm::value_ptr(sphere.center));
                        ImGui::InputInt("Material index", &sphere.materialIndex);

                        ImGui::PopID();
                    }
                }

                if (ImGui::CollapsingHeader("Triangles", nullptr)) {
                    for (int i = 0; i < (int)scene.triangles.size(); ++i) {
                        ImGui::PushID(i);

                        Triangle &triangle = scene.triangles[i];
                        ImGui::Text("Triangle %d:", i);
                        ImGui::InputFloat3("Vertex 0", glm::value_ptr(triangle.vertices[0]));
                        ImGui::InputFloat3("Vertex 1", glm::value_ptr(triangle.vertices[1]));
                        ImGui::InputFloat3("Vertex 2", glm::value_ptr(triangle.vertices[2]));
                        ImGui::InputInt("Material index", &triangle.materialIndex);

                        ImGui::PopID();
                    }
                }

                if (ImGui::CollapsingHeader("Materials", nullptr)) {
                    for (int i = 0; i < (int)scene.materials.size(); ++i) {
                        ImGui::PushID(i);

                        Material &material = scene.materials[i];
                        ImGui::Text("Material %d:", i);
                        ImGui::ColorEdit3("Albedo", glm::value_ptr(material.albedo));
                        ImGui::ColorEdit3("Emission color", glm::value_ptr(material.emissionColor));
                        ImGui::InputFloat("Emission power", &material.emissionPower);
                        ImGui::InputFloat("Reflectance", &material.reflectance);
                        ImGui::InputFloat("Fuzziness", &material.fuzziness);

                        ImGui::PopID();
                    }
                }
            }

            if (ImGui::RadioButton("Accumulate", accumulate)) {
                accumulate = !accumulate;

                if (accumulate) {
                    renderer.Accumulate();
                } else {
                    renderer.DontAccumulate();
                }
            }

            if (ImGui::InputInt("Used threads", &usedThreads)) {
                renderer.SetUsedThreadCount(usedThreads);
                usedThreads = renderer.GetUsedThreadCount();
            }

            if (ImGui::InputInt("Ray depth", &rayDepth)) {
                renderer.SetMaxRayDepth(rayDepth);
            }

            if (ImGui::InputFloat("Gamma", &gamma)) {
                renderer.SetGamma(gamma);
            }

            if (ImGui::Button("Render", {viewport->WorkSize.x * 0.05f, viewport->WorkSize.y * 0.1f}) || accumulate) {
                camera.OnViewportResize(viewportWidth, viewportHeight);
                renderer.OnResize(viewportWidth, viewportHeight);
                camera.ComputeRayDirections();

                auto t1 = std::chrono::high_resolution_clock::now();
                renderer.Render(camera, scene);
                auto t2 = std::chrono::high_resolution_clock::now();
                
                lastRenderTime = static_cast<std::chrono::duration<float, std::milli>>(t2 - t1).count();
                if (accumulate) {
                    totalRenderTime += lastRenderTime;
                } else {
                    totalRenderTime = 0.f;
                }
            }

            ImGui::InputText("##filename.png", filename, 128);
            ImGui::SameLine();
            if (ImGui::Button("Save image")) {
                renderer.SaveImage(filename);
            }

            ImGui::Text("Last render time: %fms", lastRenderTime);
            ImGui::Text("Average render time: %fms", totalRenderTime / (std::max(renderer.GetFrameIndex() - 1, 1)));
            ImGui::Text("Accumulated frame count: %d", std::max(renderer.GetFrameIndex() - 1, 1));
        }
        ImGui::End();
    });

    return app.Run();
}