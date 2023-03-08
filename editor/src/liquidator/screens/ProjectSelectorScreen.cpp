#include "liquid/core/Base.h"
#include "liquid/renderer/Renderer.h"
#include "liquid/renderer/Presenter.h"
#include "liquid/asset/AssetRegistry.h"
#include "liquid/loop/MainLoop.h"
#include "liquid/profiler/FPSCounter.h"
#include "liquid/profiler/ImguiDebugLayer.h"
#include "liquid/imgui/ImguiUtils.h"

#include "liquidator/editor-scene/EditorCamera.h"
#include "liquidator/ui/Theme.h"
#include "liquidator/ui/FontAwesome.h"
#include "liquidator/ui/Widgets.h"
#include "liquidator/ui/StyleStack.h"

#include "ProjectSelectorScreen.h"

namespace liquid::editor {

ProjectSelectorScreen::ProjectSelectorScreen(Window &window,
                                             EventSystem &eventSystem,
                                             rhi::RenderDevice *device)
    : mWindow(window), mEventSystem(eventSystem), mDevice(device) {}

std::optional<Project> ProjectSelectorScreen::start() {
  EntityDatabase entityDatabase;
  AssetRegistry assetRegistry;
  ShaderLibrary shaderLibrary;
  RenderStorage renderStorage(mDevice);
  RenderGraphEvaluator graphEvaluator(renderStorage);

  ImguiRenderer imguiRenderer(mWindow, shaderLibrary, renderStorage, mDevice);
  Presenter presenter(shaderLibrary, mDevice);

  ProjectManager projectManager;

  FPSCounter fpsCounter;
  MainLoop mainLoop(mWindow, fpsCounter);
  EditorCamera editorCamera(entityDatabase, mEventSystem, mWindow);
  std::optional<Project> project;

  presenter.updateFramebuffers(mDevice->getSwapchain());

  editorCamera.reset();

  Theme::apply();

  imguiRenderer.setClearColor(Theme::getColor(ThemeColor::BackgroundColor));
  imguiRenderer.buildFonts();

  RenderGraph graph("Main");
  auto imguiPassData = imguiRenderer.attach(graph);

  graph.setFramebufferExtent(mWindow.getFramebufferSize());

  auto resizeHandler = mWindow.addResizeHandler(
      [&graph, this, &renderStorage, &presenter](auto width, auto height) {
        graph.setFramebufferExtent({width, height});
        renderStorage.setFramebufferSize(width, height);
      });

  mainLoop.setUpdateFn([&project, this](float dt) {
    mEventSystem.poll();
    return !project.has_value();
  });

  ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                             mDevice->getDeviceStats(), fpsCounter);

  mainLoop.setRenderFn([&imguiRenderer, &graphEvaluator, &graph, &imguiPassData,
                        &renderStorage, &project, &projectManager,
                        &entityDatabase, &presenter, &debugLayer,
                        this]() mutable {
    auto &imgui = imguiRenderer;

    imgui.beginRendering();

    ImGui::BeginMainMenuBar();
    debugLayer.renderMenu();
    ImGui::EndMainMenuBar();
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

    if (ImGui::Begin("Liquidator", nullptr,
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

    if (renderStorage.recreateFramebufferRelativeTextures()) {
      presenter.updateFramebuffers(mDevice->getSwapchain());
      return;
    }

    const auto &renderFrame = mDevice->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      imgui.updateFrameData(renderFrame.frameIndex);
      graph.compile(mDevice);
      graphEvaluator.build(graph);
      graphEvaluator.execute(renderFrame.commandList, graph,
                             renderFrame.frameIndex);

      presenter.present(renderFrame.commandList, imguiPassData.imguiColor,
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

} // namespace liquid::editor
