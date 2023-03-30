#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "EditorScreen.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/renderer/StandardPushConstants.h"
#include "liquid/profiler/ImguiDebugLayer.h"
#include "liquid/scene/SceneUpdater.h"
#include "liquid/scene/SkeletonUpdater.h"
#include "liquid/scripting/ScriptingSystem.h"
#include "liquid/renderer/Presenter.h"
#include "liquid/renderer/SceneRenderer.h"
#include "liquid/imgui/ImguiRenderer.h"
#include "liquid/asset/FileTracker.h"
#include "liquid/audio/AudioSystem.h"
#include "liquid/logger/StreamTransport.h"

#include "liquid/physics/PhysicsSystem.h"
#include "liquid/loop/MainLoop.h"
#include "liquid/imgui/ImguiUtils.h"
#include "liquid/scene/CameraAspectRatioUpdater.h"

#include "liquidator/editor-scene/EditorCamera.h"
#include "liquidator/editor-scene/EditorManager.h"
#include "liquidator/ui/UIRoot.h"
#include "liquidator/ui/AssetLoadStatusDialog.h"
#include "liquidator/ui/Theme.h"
#include "liquidator/ui/Widgets.h"
#include "liquidator/ui/LogViewer.h"
#include "liquidator/ui/FontAwesome.h"

#include "liquidator/core/LogMemoryStorage.h"
#include "liquidator/core/EditorRenderer.h"
#include "liquidator/core/EditorSimulator.h"
#include "liquidator/core/MousePickingGraph.h"

#include "liquidator/asset/AssetManager.h"
#include "liquidator/state/WorkspaceState.h"

#include "liquidator/actions/ActionExecutor.h"

#include "ImGuizmo.h"

namespace liquid::editor {

EditorScreen::EditorScreen(Window &window, EventSystem &eventSystem,
                           rhi::RenderDevice *device)
    : mWindow(window), mEventSystem(eventSystem), mDevice(device) {}

void EditorScreen::start(const Project &project) {
  LogMemoryStorage systemLogStorage, userLogStorage;
  // Engine::getLogger().setTransport(systemLogStorage.createTransport());
  Engine::getUserLogger().setTransport(userLogStorage.createTransport());

  FPSCounter fpsCounter;

  auto layoutPath = (project.settingsPath / "layout.ini").string();
  auto statePath = project.settingsPath / "state.lqstate";

  Renderer renderer(mWindow, mDevice);
  AssetManager assetManager(project.assetsPath, project.assetsCachePath,
                            renderer.getRenderStorage(), true, true);

  SceneRenderer sceneRenderer(renderer.getShaderLibrary(),
                              assetManager.getAssetRegistry(),
                              renderer.getRenderStorage());

  ImguiRenderer imguiRenderer(mWindow, renderer.getShaderLibrary(),
                              renderer.getRenderStorage());

  Presenter presenter(renderer.getShaderLibrary(), renderer.getRenderStorage());

  presenter.updateFramebuffers(mDevice->getSwapchain());

  auto res = assetManager.validateAndPreloadAssets(renderer.getRenderStorage());
  AssetLoadStatusDialog preloadStatusDialog("Loaded with warnings");

  if (res.hasWarnings()) {
    preloadStatusDialog.setMessages(res.getWarnings());
    preloadStatusDialog.show();
  }

  Theme::apply();

  imguiRenderer.useConfigPath(layoutPath);
  imguiRenderer.setClearColor(Theme::getColor(ThemeColor::BackgroundColor));
  imguiRenderer.buildFonts();

  FileTracker tracker(project.assetsPath);
  tracker.trackForChanges();

  EditorCamera editorCamera(mEventSystem, mWindow);

  WorkspaceState state{project, assetManager.getAssetRegistry()};
  state.scene.entityDatabase.reg<CameraLookAt>();
  state.camera = editorCamera.createDefaultCamera(state.scene.entityDatabase);
  state.activeCamera = state.camera;

  ActionExecutor actionExecutor(state, project.scenesPath / "main.lqscene");
  actionExecutor.getSceneIO().loadScene(project.scenesPath / "main.lqscene");

  EditorManager::loadWorkspaceState(statePath, state);

  MainLoop mainLoop(mWindow, fpsCounter);
  AssetLoader assetLoader(assetManager, renderer.getRenderStorage());

  ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                             mDevice->getDeviceStats(), fpsCounter);

  UIRoot ui(actionExecutor, assetLoader);
  ui.getIconRegistry().loadIcons(renderer.getRenderStorage(),
                                 std::filesystem::current_path() / "assets" /
                                     "icons");

  EditorRenderer editorRenderer(renderer.getShaderLibrary(),
                                ui.getIconRegistry(),
                                renderer.getRenderStorage(), mDevice);

  RenderGraph graph("Main");

  auto scenePassGroup = sceneRenderer.attach(graph);
  auto imguiPassGroup = imguiRenderer.attach(graph);
  imguiPassGroup.pass.read(scenePassGroup.finalColor);

  {
    static constexpr glm::vec4 ClearColor{0.52f, 0.54f, 0.89f, 1.0f};
    auto &pass = editorRenderer.attach(graph);
    pass.write(scenePassGroup.sceneColor, AttachmentType::Color, ClearColor);
    pass.write(scenePassGroup.depthBuffer, AttachmentType::Depth,
               rhi::DepthStencilClear{1.0f, 0});
    pass.write(scenePassGroup.sceneColorResolved, AttachmentType::Resolve,
               ClearColor);
  }

  sceneRenderer.attachText(graph, scenePassGroup);

  MousePickingGraph mousePicking(
      renderer.getShaderLibrary(), sceneRenderer.getFrameData(),
      assetManager.getAssetRegistry(), renderer.getRenderStorage(), mDevice);

  mousePicking.setFramebufferSize(mWindow);
  graph.setFramebufferExtent(mWindow.getFramebufferSize());

  mWindow.addResizeHandler([&graph, &renderer](auto width, auto height) {
    graph.setFramebufferExtent({width, height});
    renderer.getRenderStorage().setFramebufferSize(width, height);
  });

  mWindow.addFocusHandler(
      [&tracker, &assetManager, &renderer, &ui](bool focused) {
        if (!focused)
          return;

        const auto &changes = tracker.trackForChanges();
        for (auto &change : changes) {
          assetManager.loadOriginalIfChanged(change.path);
        }

        ui.getAssetBrowser().reload();
      });

  ui.processShortcuts(mEventSystem);

  EditorSimulator simulator(mEventSystem, mWindow,
                            assetManager.getAssetRegistry(), editorCamera);

  mWindow.maximize();

  mainLoop.setUpdateFn([&state, &simulator, this](float dt) mutable {
    mEventSystem.poll();
    simulator.update(dt, state);
    return true;
  });

  LogViewer logViewer;
  mainLoop.setRenderFn([&renderer, &editorCamera, &assetManager, &graph,
                        &sceneRenderer, &imguiRenderer, &scenePassGroup,
                        &imguiPassGroup, &ui, &debugLayer, &preloadStatusDialog,
                        &presenter, &editorRenderer, &simulator, &mousePicking,
                        &systemLogStorage, &userLogStorage, &logViewer, &state,
                        &actionExecutor, this]() {
    // TODO: Why is -2.0f needed here
    static const float IconSize = ImGui::GetFrameHeight() - 2.0f;

    imguiRenderer.beginRendering();
    ImGuizmo::BeginFrame();

    ui.render(state, assetManager);

    if (auto _ = widgets::MainMenuBar()) {
      debugLayer.renderMenu();
    }
    debugLayer.render();

    logViewer.render(systemLogStorage, userLogStorage);

    bool mouseClicked =
        ui.renderSceneView(state, scenePassGroup.finalColor, editorCamera);

    StatusBar::render(editorCamera);

    preloadStatusDialog.render();

    imguiRenderer.endRendering();

    if (renderer.getRenderStorage().recreateFramebufferRelativeTextures()) {
      presenter.updateFramebuffers(mDevice->getSwapchain());
      return;
    }

    const auto &renderFrame = mDevice->beginFrame();

    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      imguiRenderer.updateFrameData(renderFrame.frameIndex);
      sceneRenderer.updateFrameData(scene.entityDatabase, state.activeCamera,
                                    renderFrame.frameIndex);
      editorRenderer.updateFrameData(scene.entityDatabase, state.activeCamera,
                                     state, renderFrame.frameIndex);

      if (mousePicking.isSelectionPerformedInFrame(renderFrame.frameIndex)) {
        auto entity = mousePicking.getSelectedEntity();
        state.selectedEntity = entity;
      }

      mousePicking.compile();

      renderer.render(graph, renderFrame.commandList, renderFrame.frameIndex);

      if (mouseClicked) {
        auto mousePos = mWindow.getCurrentMousePosition();

        if (editorCamera.isWithinViewport(mousePos)) {
          auto scaledMousePos = editorCamera.scaleToViewport(mousePos);

          mousePicking.execute(renderFrame.commandList, scaledMousePos,
                               renderFrame.frameIndex);
        }
        mouseClicked = false;
      }

      presenter.present(renderFrame.commandList, imguiPassGroup.imguiColor,
                        renderFrame.swapchainImageIndex);

      mDevice->endFrame(renderFrame);

    } else {
      presenter.updateFramebuffers(mDevice->getSwapchain());
    }
  });

  mainLoop.run();
  Engine::resetLoggers();
  EditorManager::saveWorkspaceState(state, statePath);

  mDevice->waitForIdle();
}

} // namespace liquid::editor
