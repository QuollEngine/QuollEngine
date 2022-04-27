#include "liquid/core/Base.h"
#include "ProjectSelectorScreen.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/asset/AssetRegistry.h"
#include "liquid/loop/MainLoop.h"
#include "liquid/profiler/FPSCounter.h"

#include "liquid/imgui/ImguiUtils.h"

#include "../editor-scene/EditorCamera.h"
#include "../ui/IconRegistry.h"

namespace liquidator {

ProjectSelectorScreen::ProjectSelectorScreen(liquid::Window &window,
                                             liquid::EventSystem &eventSystem,
                                             liquid::rhi::RenderDevice *device)
    : mWindow(window), mEventSystem(eventSystem), mDevice(device) {}

std::optional<Project> ProjectSelectorScreen::start() {
  liquid::EntityContext entityContext;
  liquid::AssetRegistry assetRegistry;
  liquid::Renderer renderer(entityContext, assetRegistry, mWindow, mDevice);
  liquidator::ProjectManager projectManager;

  liquid::FPSCounter fpsCounter;
  liquid::MainLoop mainLoop(mWindow, fpsCounter);
  liquidator::EditorCamera editorCamera(entityContext, mEventSystem, renderer,
                                        mWindow);
  liquidator::IconRegistry iconRegistry;
  std::optional<liquidator::Project> project;

  static constexpr float HALF = 0.5f;
  static constexpr float ICON_SIZE = 80.0f;

  editorCamera.reset();
  auto graph = renderer.createRenderGraph(false);

  iconRegistry.loadIcons(renderer.getRegistry(),
                         std::filesystem::current_path() / "assets" / "icons");

  mainLoop.setUpdateFn([&project, this](float dt) {
    mEventSystem.poll();
    return !project.has_value();
  });

  mainLoop.setRenderFn([&renderer, &editorCamera, &graph, &project,
                        &projectManager, &iconRegistry, this]() mutable {
    auto &imgui = renderer.getImguiRenderer();

    imgui.beginRendering();

    const auto &fbSize = glm::vec2(mWindow.getFramebufferSize());
    auto center = fbSize * HALF;

    ImGui::SetNextWindowPos(ImVec2(center.x, center.y), 0, ImVec2(HALF, HALF));

    if (ImGui::Begin("Liquidator", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar)) {

      if (ImGui::BeginTable("project-selector", 2)) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (liquid::imgui::imageButton(
                iconRegistry.getIcon(liquidator::EditorIcon::CreateDirectory),
                ImVec2(ICON_SIZE, ICON_SIZE))) {
          if (projectManager.createProjectInPath()) {
            project = projectManager.getProject();
          }
        }
        ImGui::Text("Create");

        ImGui::TableNextColumn();
        if (liquid::imgui::imageButton(
                iconRegistry.getIcon(liquidator::EditorIcon::Directory),
                ImVec2(ICON_SIZE, ICON_SIZE))) {
          if (projectManager.openProjectInPath()) {
            project = projectManager.getProject();
          }
        }

        ImGui::Text("Open");
      }
      ImGui::EndTable();
    }
    ImGui::End();

    imgui.endRendering();
    renderer.render(graph.first, editorCamera.getCamera());
  });

  mainLoop.run();

  return project;
}

} // namespace liquidator
