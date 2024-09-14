#include "quoll/core/Base.h"
#include "quoll/asset/AssetRegistry.h"
#include "quoll/imgui/ImguiRenderer.h"
#include "quoll/imgui/ImguiUtils.h"
#include "quoll/loop/MainLoop.h"
#include "quoll/profiler/FPSCounter.h"
#include "quoll/profiler/ImguiDebugLayer.h"
#include "quoll/renderer/Presenter.h"
#include "quoll/renderer/RenderStorage.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/RendererAssetRegistry.h"
#include "quoll/editor/ui/FontAwesome.h"
#include "quoll/editor/ui/MainMenuBar.h"
#include "quoll/editor/ui/StyleStack.h"
#include "quoll/editor/ui/Theme.h"
#include "quoll/editor/ui/Widgets.h"
#include "ProjectSelectorWindow.h"

namespace quoll::editor {

ProjectSelectorWindow::ProjectSelectorWindow(Window &window,
                                             rhi::RenderDevice *device)
    : mWindow(window), mDevice(device) {}

std::optional<Project> ProjectSelectorWindow::start() {
  EntityDatabase entityDatabase;
  AssetRegistry assetRegistry;
  MetricsCollector metricsCollector;
  RenderStorage renderStorage(mDevice, metricsCollector);

  quoll::RendererOptions initialOptions{};
  initialOptions.framebufferSize = mWindow.getFramebufferSize();

  Renderer renderer(renderStorage, initialOptions);

  quoll::RendererAssetRegistry rendererAssetRegistry(renderStorage);
  ImguiRenderer imguiRenderer(mWindow, renderStorage, rendererAssetRegistry);
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

  auto framebufferResizeSlot =
      mWindow.getSignals().onFramebufferResize().connect(
          [&](auto width, auto height) {
            renderer.setFramebufferSize({width, height});
            presenter.enqueueFramebufferUpdate();
          });

  mainLoop.setUpdateFn([&project, this](f32 dt) {});

  ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                             mDevice->getDeviceStats(), fpsCounter,
                             metricsCollector, nullptr);

  mainLoop.setRenderFn([&]() mutable {
    if (presenter.requiresFramebufferUpdate()) {
      mDevice->recreateSwapchain();
      presenter.updateFramebuffers(mDevice->getSwapchain());
      return;
    }

    renderer.rebuildIfSettingsChanged();

    auto &imgui = imguiRenderer;

    imgui.beginRendering();

    if (auto _ = MainMenuBar()) {
      debugLayer.renderMenu();
    }
    debugLayer.render();

    static constexpr ImVec2 CenterWindowPivot(0.5f, 0.5f);
    static constexpr f32 ActionButtonWidth = 240.0f;
    static constexpr f32 ActionButtonHeight = 40.0f;
    static constexpr f32 WindowPadding = 20.0f;
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
      if (widgets::Button(CreateProjectLabel.c_str(), ActionButtonSize)) {
        if (projectManager.createProjectInPath()) {
          project = projectManager.getProject();
        }
      }

      static const auto OpenProjectLabel =
          String(fa::FolderOpen) + "  Open project";
      if (widgets::Button(OpenProjectLabel.c_str(), ActionButtonSize)) {
        if (projectManager.openProjectInPath()) {
          project = projectManager.getProject();
        }
      }
    }

    if (project.has_value()) {
      mainLoop.stop();
    }

    ImGui::End();

    imgui.endRendering();

    const auto &renderFrame = mDevice->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<u32>::max()) {
      imgui.updateFrameData(renderFrame.frameIndex);
      renderer.execute(renderFrame.commandList, renderFrame.frameIndex);

      presenter.present(renderFrame.commandList, renderer.getFinalTexture(),
                        renderFrame.swapchainImageIndex);
      mDevice->endFrame(renderFrame);
      metricsCollector.getResults(mDevice);
    } else {
      presenter.updateFramebuffers(mDevice->getSwapchain());
    }
  });

  mainLoop.setStatsFn([this, &metricsCollector](u32 frames) {
    metricsCollector.markForCollection();
  });

  mainLoop.run();

  framebufferResizeSlot.disconnect();
  mDevice->waitForIdle();

  return project;
}

} // namespace quoll::editor
