#include <imgui.h>
#include <Application.h>
#include <Renderer.h>
#include <chrono>
#include <algorithm>

#include <glm/gtc/type_ptr.hpp>

#define dbg(msg) fprintf(stdout, (msg));

int main() {
    Application app(1580, 720, "With the lights out");

    Scene scene;
    Camera camera(1580, 720, {0.f, -0.5f, 2.f});
    Renderer renderer(1580, 720);

    Material redWithLight;
    redWithLight.albedo = glm::vec3(1.f, 0.647f, 0.851f);

    Material purpleWithLight;
    purpleWithLight.albedo = glm::vec3(0.941f, 0.757f, 0.027f);
    purpleWithLight.emissionColor = glm::vec3(0.941f, 0.757f, 0.027f);
    purpleWithLight.emissionPower = 1.2f;

    Material blueColor;
    blueColor.albedo = glm::vec3(0.1f, 0.5f, 0.7f);

    scene.spheres.emplace_back(glm::vec3(-0.55f, -0.5f, 0.f), 0.5f, 0);
    scene.spheres.emplace_back(glm::vec3(0.55f, -0.5f, 0.f), 0.5f, 1);
    scene.spheres.emplace_back(glm::vec3(0.f, -1001.f, 0.f), 1000.f, 2);

    scene.materials.assign({redWithLight, purpleWithLight, blueColor});

    bool accumulate = false;
    float lastRenderTime = 0.f;
    float totalRenderTime = 0.f;

    char *filename = new char[128];
    memset(filename, 0, sizeof(char) * sizeof(filename));

    int rayDepth = renderer.GetMaxRayDepth();
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