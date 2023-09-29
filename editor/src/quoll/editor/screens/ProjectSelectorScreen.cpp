#include "quoll/core/Base.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/Presenter.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/loop/MainLoop.h"
#include "quoll/profiler/FPSCounter.h"
#include "quoll/profiler/ImguiDebugLayer.h"
#include "quoll/imgui/ImguiUtils.h"
#include "quoll/imgui/ImguiRenderer.h"
#include "quoll/renderer/Renderer.h"

#include "quoll/editor/ui/Theme.h"
#include "quoll/editor/ui/FontAwesome.h"
#include "quoll/editor/ui/Widgets.h"
#include "quoll/editor/ui/StyleStack.h"

#include "ProjectSelectorScreen.h"

namespace quoll::editor {

ProjectSelectorScreen::ProjectSelectorScreen(Window &window,
                                             EventSystem &eventSystem,
                                             rhi::RenderDevice *device)
    : mWindow(window), mEventSystem(eventSystem), mDevice(device) {}

std::optional<Project> ProjectSelectorScreen::start() {
  EntityDatabase entityDatabase;
  AssetRegistry assetRegistry;
  RenderStorage renderStorage(mDevice);

  quoll::RendererOptions initialOptions{};
  initialOptions.size = mWindow.getFramebufferSize();

  Renderer renderer(renderStorage, initialOptions);

  ImguiRenderer imguiRenderer(mWindow, renderStorage);
  Presenter presenter(renderStorage);

  ProjectManager projectManager;

  FPSCounter fpsCounter;
  MainLoop mainLoop(mWindow, fpsCounter);
  std::optional<Project> project;

  presenter.updateFramebuffers(mDevice->getSwapchain());

  Theme::apply();

  imguiRenderer.setClearColor(Theme::getClearColor());
  imguiRenderer.buildFonts();

  renderer.setGraphBuilder([&](RenderGraph &graph,
                               const RendererOptions &options) {
    auto imguiPassData = imguiRenderer.attach(graph, options);
    return RendererTextures{imguiPassData.imguiColor, imguiPassData.imguiColor};
  });

  auto resizeHandler =
      mWindow.addFramebufferResizeHandler([&](auto width, auto height) {
        renderer.setFramebufferSize({width, height});
        presenter.enqueueFramebufferUpdate();
      });

  mainLoop.setUpdateFn([&project, this](float dt) {
    mEventSystem.poll();
    return !project.has_value();
  });

  ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                             mDevice->getDeviceStats(), fpsCounter);

  mainLoop.setRenderFn([&]() mutable {
    if (presenter.requiresFramebufferUpdate()) {
      mDevice->recreateSwapchain();
      presenter.updateFramebuffers(mDevice->getSwapchain());
      return;
    }

    renderer.rebuildIfSettingsChanged();

    auto &imgui = imguiRenderer;

    imgui.beginRendering();

    if (auto _ = widgets::MainMenuBar()) {
      debugLayer.renderMenu();
    }
    debugLayer.render();

    static constexpr ImVec2 CenterWindowPivot(0.5f, 0.5f);
    static constexpr float ActionButtonWidth = 240.0f;
    static constexpr float ActionButtonHeight = 40.0f;
    static constexpr float WindowPadding = 20.0f;
    static constexpr ImVec2 ActionButtonSize{ActionButtonWidth,
                                             ActionButtonHeight};

    const auto &fbSize = glm::vec2(mWindow.getFramebufferSize());
    const auto actionBarPos =
        ImVec2(fbSize.x - ActionButtonWidth - WindowPadding,
               fbSize.y * 0.5f - WindowPadding);

    ImGui::SetNextWindowPos(actionBarPos, 0, CenterWindowPivot);

    if (ImGui::Begin("Quoll Engine", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoTitleBar)) {
      StyleStack styles;
      styles.pushStyle(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));

      static const auto CreateProjectLabel =
          String(fa::FolderPlus) + "  Create project";
      if (ImGui::Button(CreateProjectLabel.c_str(), ActionButtonSize)) {
        if (projectManager.createProjectInPath()) {
          project = projectManager.getProject();
        }
      }

      static const auto OpenProjectLabel =
          String(fa::FolderOpen) + "  Open project";
      if (ImGui::Button(OpenProjectLabel.c_str(), ActionButtonSize)) {
        if (projectManager.openProjectInPath()) {
          project = projectManager.getProject();
        }
      }
    }
    ImGui::End();

    imgui.endRendering();

    const auto &renderFrame = mDevice->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      imgui.updateFrameData(renderFrame.frameIndex);
      renderer.execute(renderFrame.commandList, renderFrame.frameIndex);

      presenter.present(renderFrame.commandList, renderer.getFinalTexture(),
                        renderFrame.swapchainImageIndex);
      mDevice->endFrame(renderFrame);
    } else {
      presenter.updateFramebuffers(mDevice->getSwapchain());
    }
  });

  mainLoop.run();

  mWindow.removeResizeHandler(resizeHandler);
  mDevice->waitForIdle();

  return project;
}

} // namespace quoll::editor
