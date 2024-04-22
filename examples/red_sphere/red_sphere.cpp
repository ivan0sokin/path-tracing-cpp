#include <imgui.h>
#include <Application.h>
#include <Renderer.h>
#include <chrono>
#include <algorithm>
#include <hittable/Sphere.h>

#define dbg(msg) fprintf(stdout, (msg));

int main() {
    Application app(1580, 720, "Red sphere");

    Scene scene;
    Camera camera(1580, 720, {0.f, -0.5f, 10.f}, {0.f, -0.5f, -1.f});
    Renderer renderer(1580, 720);
    renderer.OnRayMiss([](const auto &ray){ return Math::Vector3f(0.2f, 0.4f, 0.6f); });

    Material redColor;
    redColor.albedo = Math::Vector3f(1.f, 0.1f, 0.2f);

    Material greenColor;
    greenColor.albedo = Math::Vector3f(0.1f, 0.5f, 0.7f);

    std::vector<Shapes::Sphere*> spheres;

    spheres.push_back(new Shapes::Sphere(Math::Vector3f(0.f, -0.5f, 0.f), 0.5f, 0));
    spheres.push_back(new Shapes::Sphere(Math::Vector3f(0.f, -1001.f, 0.f), 1000.f, 1));
    
    scene.objects.insert(scene.objects.cend(), spheres.cbegin(), spheres.cend());
    scene.materials.assign({redColor, greenColor});

    bool accumulate = false;
    float lastRenderTime = 0.f;
    float totalRenderTime = 0.f;

    char *filename = new char[128];
    memset(filename, 0, sizeof(char) * sizeof(filename));

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
                    for (int i = 0; i < (int)spheres.size(); ++i) {
                        ImGui::PushID(i);

                        Shapes::Sphere &sphere = *spheres[i];
                        ImGui::Text("Sphere %d:", i);
                        if (ImGui::InputFloat("Radius", &sphere.radius)) {
                            sphere.radiusSquared = sphere.radius * sphere.radius;
                            sphere.inverseRadius = 1.f / sphere.inverseRadius;
                        }
                        ImGui::InputFloat3("Position", Math::ValuePointer(sphere.center));
                        ImGui::InputInt("Material index", &sphere.materialIndex);

                        ImGui::PopID();
                    }
                }

                if (ImGui::CollapsingHeader("Materials", nullptr)) {
                    for (int i = 0; i < (int)scene.materials.size(); ++i) {
                        ImGui::PushID(i);

                        Material &material = scene.materials[i];
                        ImGui::Text("Material %d:", i);
                        ImGui::ColorEdit3("Albedo", Math::ValuePointer(material.albedo));
                        ImGui::ColorEdit3("Emission color", Math::ValuePointer(material.emissionColor));
                        ImGui::InputFloat("Emission power", &material.emissionPower);

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