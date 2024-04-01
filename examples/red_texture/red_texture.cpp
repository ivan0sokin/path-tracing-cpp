#include <imgui.h>
#include <Application.h>
#include <Renderer.h>

int main() {
    Application app(1280, 720, "Red texture");
    Renderer renderer(1280, 720);

    app.SetOnUpdate([&](){
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGuiWindowFlags frameFlags = ImGuiViewportFlags_IsPlatformWindow
                                    | ImGuiViewportFlags_NoDecoration
                                    | ImGuiViewportFlags_NoTaskBarIcon
                                    | ImGuiViewportFlags_NoAutoMerge
                                    | ImGuiWindowFlags_NoCollapse
                                    | ImGuiWindowFlags_NoResize;

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize({viewport->WorkSize.x * 0.8f, viewport->WorkSize.y});
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::Begin("Frame", nullptr, frameFlags);
        {
            int width = (int)ImGui::GetContentRegionAvail().x;
            int height = (int)ImGui::GetContentRegionAvail().y;
            renderer.OnResize(width, height);
            ImGui::Image((void*)renderer.Render()->GetDescriptor(), ImGui::GetContentRegionAvail(), {.0f, 1.f}, {1.f, 0.f});
        }
        ImGui::End();
        
        ImGui::SetNextWindowPos({viewport->WorkPos.x + viewport->WorkSize.x * 0.8f, viewport->WorkPos.y});
        ImGui::SetNextWindowSize({viewport->WorkSize.x * 0.2f, viewport->WorkSize.y});
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoTitleBar);
        {
            if (ImGui::CollapsingHeader("Btn", nullptr)) {
                static int firstCounter = 0;
                if (ImGui::Button("First counter")) {
                    firstCounter++;
                }

                ImGui::Text("%d", firstCounter);
            }

            if (ImGui::CollapsingHeader("Second Btn", nullptr)) {
                static int secondCounter = 0;
                if (ImGui::Button("Second Counter")) {
                    secondCounter++;
                }
                ImGui::Text("%d", secondCounter);
            }
        }
        ImGui::End();
    });

    return app.Run();
}