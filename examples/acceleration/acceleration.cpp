#include <imgui.h>
#include <Application.h>
#include <Renderer.h>
#include <math/Math.h>
#include <hittable/Triangle.h>
#include <hittable/Box.h>
#include <hittable/Sphere.h>

#include <AccelerationStructure.h>

#include <chrono>
#include <algorithm>

#define dbg(msg) fprintf(stdout, (msg));

int main() {
    Application app(1580, 720, "Cornell box");

    Math::Vector3f position(-277.5f, 277.5f, 800.f);
    Math::Vector3f target(-277.5f, 277.5f, 0.f);
    float verticalFovInDegrees = 40.f;
    Math::Vector3f up(0.f, 1.f, 0.f);
    Camera camera(1580, 720, position, target, Math::ToRadians(verticalFovInDegrees), up);
    
    Renderer renderer(1580, 720);
    // renderer.OnRayMiss([](const Ray&){ return Math::Vector3f(0.1f, 0.2f, 0.5f);});

    Scene scene;

    std::vector<Shapes::Sphere*> spheres;
    spheres.push_back(new Shapes::Sphere(Math::Vector3f{-100.f, 300.f, -455.f}, 100.f, 4));

    scene.objects.insert(scene.objects.cend(), spheres.cbegin(), spheres.cend());

    std::vector<Shapes::Triangle*> triangles;
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{-555.f, 0.f, 0.f}, Math::Vector3f{-555.f, 555.f, 0.f}, Math::Vector3f{-555.f, 0.f, -555.f}, 0));
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{-555.f, 555.f, 0.f}, Math::Vector3f{-555.f, 555.f, -555.f}, Math::Vector3f{-555.f, 0.f, -555.f}, 0)); // left
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{-555.f, 0.f, 0.f}, Math::Vector3f{-555.f, 0.f, -555.f}, Math::Vector3f{0.f, 0.f, -555.f}, 1));
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{0.f, 0.f, -555.f}, Math::Vector3f{0.f, 0.f, 0.f}, Math::Vector3f{-555.f, 0.f, 0.f}, 1)); // bottom
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{-555.f, 0.f, -555.f}, Math::Vector3f{-555.f, 555.f, -555.f}, Math::Vector3f{0.f, 555.f, -555.f}, 1));
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{0.f, 555.f, -555.f}, Math::Vector3f{0.f, 0.f, -555.f}, Math::Vector3f{-555.f, 0.f, -555.f}, 1)); // forward
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{-555.f, 555.f, 0.f}, Math::Vector3f{-555.f, 555.f, -555.f}, Math::Vector3f{0.f, 555.f, -555.f}, 1));
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{0.f, 555.f, -555.f}, Math::Vector3f{0.f, 555.f, 0.f}, Math::Vector3f{-555.f, 555.f, 0.f}, 1)); // up
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{0.f, 0.f, 0.f}, Math::Vector3f{0.f, 0.f, -555.f}, Math::Vector3f{0.f, 555.f, -555.f}, 2));
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{0.f, 555.f, -555.f}, Math::Vector3f{0.f, 555.f, 0.f}, Math::Vector3f{0.f, 0.f, 0.f}, 2)); // right
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{-343.f - 100.f, 554.f, -343.f + 130.f + 100.f}, Math::Vector3f{-343.f - 100.f, 554.f, -343.f - 100.f}, Math::Vector3f{-343.f + 130.f + 100.f, 554.f, -343.f -100.f}, 3));
    triangles.push_back(new Shapes::Triangle(Math::Vector3f{-343.f + 130.f + 100.f, 554.f, -343.f - 100.f}, Math::Vector3f{-343.f + 130.f + 100.f, 554.f, -343.f + 130.f + 100.f}, Math::Vector3f{-343.f - 100.f, 554.f, -343.f + 130.f + 100.f}, 3)); // top light

    scene.objects.insert(scene.objects.cend(), triangles.cbegin(), triangles.cend());

    std::vector<Shapes::Box*> boxes;

    boxes.push_back(new Shapes::Box(Math::Vector3f{-130.f, 0.f, -65.f}, Math::Vector3f{-295.f, 165.f, -230.f}, 1));
    boxes.push_back(new Shapes::Box(Math::Vector3f{-265.f, 0.f, -295.f}, Math::Vector3f{-430.f, 330.f, -460.f}, 1));

    scene.objects.insert(scene.objects.cend(), boxes.cbegin(), boxes.cend());

    AccelerationStructure accelerationStructure(scene.objects);

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
    reflective.metallic = 1.f;
    scene.materials.push_back(reflective);

    bool accumulate = false;
    float lastRenderTime = 0.f;
    float totalRenderTime = 0.f;

    char *filename = new char[128];
    memset(filename, 0, sizeof(char) * sizeof(filename));

    int rayDepth = renderer.GetMaxRayDepth();
    int usedThreads = renderer.GetUsedThreadCount();
    float gamma = renderer.GetGamma();

    bool accelerate = false;

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
                if (ImGui::InputFloat3("Position", Math::ValuePointer(position))) {
                    camera.SetPosition(position);
                }
                if (ImGui::InputFloat3("Target", Math::ValuePointer(target))) {
                    camera.SetTarget(target);
                }
                if (ImGui::InputFloat("Vertical FOV", &verticalFovInDegrees)) {
                    camera.SetVerticalFovInRadians(Math::ToRadians(verticalFovInDegrees));
                }
                if (ImGui::InputFloat3("Up", Math::ValuePointer(up))) {
                    camera.SetUp(up);
                }
            }

            if (ImGui::CollapsingHeader("Scene", nullptr)) {
                if (ImGui::CollapsingHeader("Spheres", nullptr)) {
                    for (int i = 0; i < (int)spheres.size(); ++i) {
                        ImGui::PushID(i);

                        Shapes::Sphere *sphere = spheres[i];
                        ImGui::Text("Sphere %d:", i);
                        if (ImGui::InputFloat("Radius", &sphere->radius)) {
                            sphere->radiusSquared = sphere->radius * sphere->radius;
                            sphere->inverseRadius = 1.f / sphere->inverseRadius;
                        }
                        ImGui::InputFloat3("Position", Math::ValuePointer(sphere->center));
                        ImGui::InputInt("Material index", &sphere->materialIndex);

                        ImGui::PopID();
                    }
                }

                if (ImGui::CollapsingHeader("Triangles", nullptr)) {
                    for (int i = 0; i < (int)triangles.size(); ++i) {
                        ImGui::PushID(i);

                        Shapes::Triangle *triangle = triangles[i];
                        ImGui::Text("Triangle %d:", i);
                        ImGui::InputFloat3("Vertex 0", Math::ValuePointer(triangle->vertices[0]));
                        ImGui::InputFloat3("Vertex 1", Math::ValuePointer(triangle->vertices[1]));
                        ImGui::InputFloat3("Vertex 2", Math::ValuePointer(triangle->vertices[2]));
                        ImGui::InputInt("Material index", &triangle->materialIndex);

                        ImGui::PopID();
                    }
                }

                if (ImGui::CollapsingHeader("Boxes", nullptr)) {
                    for (int i = 0; i < (int)boxes.size(); ++i) {
                        ImGui::PushID(i);

                        Shapes::Box *box = boxes[i];
                        ImGui::InputInt("Material index", &box->materialIndex);

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
                        ImGui::InputFloat("Metallic", &material.metallic);
                        ImGui::InputFloat("Roughness", &material.roughness);
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

            if (ImGui::RadioButton("Accelerate", accelerate)) {
                accelerate = !accelerate;
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
                if (accelerate) {
                    renderer.Render(camera, accelerationStructure, scene.materials);
                } else {
                    renderer.Render(camera, scene);
                }
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