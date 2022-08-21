#include "liquid/core/Base.h"
#include "liquid/renderer/Renderer.h"
#include "liquid/renderer/Presenter.h"
#include "liquid/asset/AssetRegistry.h"
#include "liquid/loop/MainLoop.h"
#include "liquid/profiler/FPSCounter.h"
#include "liquid/imgui/ImguiUtils.h"

#include "liquidator/editor-scene/EditorCamera.h"
#include "liquidator/ui/IconRegistry.h"
#include "liquidator/ui/Theme.h"

#include "ProjectSelectorScreen.h"

namespace liquidator {

ProjectSelectorScreen::ProjectSelectorScreen(liquid::Window &window,
                                             liquid::EventSystem &eventSystem,
                                             liquid::rhi::RenderDevice *device)
    : mWindow(window), mEventSystem(eventSystem), mDevice(device) {}

std::optional<Project> ProjectSelectorScreen::start() {
  liquid::EntityDatabase entityDatabase;
  liquid::AssetRegistry assetRegistry;

  liquid::Renderer renderer(assetRegistry, mWindow, mDevice);
  liquid::Presenter presenter(renderer.getShaderLibrary(),
                              renderer.getRegistry());

  liquidator::ProjectManager projectManager;

  liquid::FPSCounter fpsCounter;
  liquid::MainLoop mainLoop(mWindow, fpsCounter);
  liquidator::EditorCamera editorCamera(entityDatabase, mEventSystem, renderer,
                                        mWindow);
  liquidator::IconRegistry iconRegistry;
  std::optional<liquidator::Project> project;

  static constexpr float IconSize = 80.0f;

  presenter.updateFramebuffers(mDevice->getSwapchain());

  editorCamera.reset();

  Theme::apply();

  renderer.getImguiRenderer().setClearColor(
      Theme::getColor(ThemeColor::BackgroundColor));
  renderer.getImguiRenderer().buildFonts();

  liquid::rhi::RenderGraph graph;
  auto imguiPassData = renderer.getImguiRenderer().attach(graph);

  graph.setFramebufferExtent(mWindow.getFramebufferSize());

  auto resizeHandler = mWindow.addResizeHandler(
      [&graph, this, &presenter](auto width, auto height) {
        graph.setFramebufferExtent({width, height});
      });

  iconRegistry.loadIcons(mDevice,
                         std::filesystem::current_path() / "assets" / "icons");

  mainLoop.setUpdateFn([&project, this](float dt) {
    mEventSystem.poll();
    return !project.has_value();
  });

  mainLoop.setRenderFn([&renderer, &editorCamera, &graph, &imguiPassData,
                        &project, &projectManager, &iconRegistry,
                        &entityDatabase, &presenter, this]() mutable {
    auto &imgui = renderer.getImguiRenderer();

    imgui.beginRendering();

    const auto &fbSize = glm::vec2(mWindow.getFramebufferSize());
    const auto center = fbSize * 0.5f;

    static constexpr ImVec2 CenterWindowPivot(0.5f, 0.5f);

    ImGui::SetNextWindowPos(ImVec2(center.x, center.y), 0, CenterWindowPivot);

    if (ImGui::Begin("Liquidator", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar)) {

      if (ImGui::BeginTable("project-selector", 2)) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (liquid::imgui::imageButton(
                iconRegistry.getIcon(liquidator::EditorIcon::CreateDirectory),
                ImVec2(IconSize, IconSize))) {
          if (projectManager.createProjectInPath()) {
            project = projectManager.getProject();
          }
        }
        ImGui::Text("Create");

        ImGui::TableNextColumn();
        if (liquid::imgui::imageButton(
                iconRegistry.getIcon(liquidator::EditorIcon::Directory),
                ImVec2(IconSize, IconSize))) {
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

    const auto &renderFrame = mDevice->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      imgui.updateFrameData(renderFrame.frameIndex);
      renderer.render(graph, renderFrame.commandList);

      presenter.present(renderFrame.commandList, imguiPassData.imguiColor,
                        renderFrame.swapchainImageIndex);
      mDevice->endFrame(renderFrame);
    } else {
      presenter.updateFramebuffers(mDevice->getSwapchain());
    }
  });

  mainLoop.run();

  mWindow.removeResizeHandler(resizeHandler);
  return project;
}

} // namespace liquidator
