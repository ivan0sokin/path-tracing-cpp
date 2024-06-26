#include <imgui.h>
#include <Application.h>
#include <Renderer.h>
#include <chrono>
#include <algorithm>
#include <hittable/Sphere.h>
#include <hittable/Triangle.h>

#define dbg(msg) fprintf(stdout, (msg));

int main() {
    Application app(1580, 720, "With the lights out");

    Scene scene;
    Camera camera(1580, 720, {0.f, -0.5f, 2.f});
    Renderer renderer(1580, 720);

    Material redWithLight;
    redWithLight.albedo = Math::Vector3f(1.f, 0.647f, 0.851f);

    Material purpleWithLight;
    purpleWithLight.albedo = Math::Vector3f(0.941f, 0.757f, 0.027f);
    purpleWithLight.emissionColor = Math::Vector3f(0.941f, 0.757f, 0.027f);
    purpleWithLight.emissionPower = 1.2f;

    Material blueColor;
    blueColor.albedo = Math::Vector3f(0.1f, 0.5f, 0.7f);

    std::vector<Shapes::Sphere*> spheres;

    spheres.push_back(new Shapes::Sphere(Math::Vector3f(-0.55f, -0.5f, 0.f), 0.5f, 0));
    spheres.push_back(new Shapes::Sphere(Math::Vector3f(0.55f, -0.5f, 0.f), 0.5f, 1));
    spheres.push_back(new Shapes::Sphere(Math::Vector3f(0.f, -1001.f, 0.f), 1000.f, 2));

    scene.objects.insert(scene.objects.cend(), spheres.cbegin(), spheres.cend());

    std::vector<Shapes::Triangle*> triangles;

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

                if (ImGui::CollapsingHeader("Triangles", nullptr)) {
                    for (int i = 0; i < (int)triangles.size(); ++i) {
                        ImGui::PushID(i);

                        Shapes::Triangle &triangle = *triangles[i];
                        ImGui::Text("Triangle %d:", i);
                        ImGui::InputFloat3("Vertex 0", Math::ValuePointer(triangle.vertices[0]));
                        ImGui::InputFloat3("Vertex 1", Math::ValuePointer(triangle.vertices[1]));
                        ImGui::InputFloat3("Vertex 2", Math::ValuePointer(triangle.vertices[2]));
                        ImGui::InputInt("Material index", &triangle.materialIndex);

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
                        ImGui::InputFloat("Reflectance", &material.metallic);
                        ImGui::InputFloat("Fuzziness", &material.roughness);
                        ImGui::InputFloat("Specular", &material.specular);

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