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
  Engine::getLogger().setTransport(systemLogStorage.createTransport());
  Engine::getUserLogger().setTransport(userLogStorage.createTransport());

  FPSCounter fpsCounter;

  auto layoutPath = (project.settingsPath / "layout.ini").string();
  auto statePath = project.settingsPath / "state.lqstate";

  AssetManager assetManager(project.assetsPath, project.assetsCachePath,
                            mDevice, true, true);

  Renderer renderer(assetManager.getAssetRegistry(), mWindow, mDevice);

  Presenter presenter(renderer.getShaderLibrary(), mDevice);

  presenter.updateFramebuffers(mDevice->getSwapchain());

  auto res = assetManager.validateAndPreloadAssets(renderer.getRenderStorage());
  AssetLoadStatusDialog preloadStatusDialog("Loaded with warnings");

  if (res.hasWarnings()) {
    preloadStatusDialog.setMessages(res.getWarnings());
    preloadStatusDialog.show();
  }

  Theme::apply();

  renderer.getImguiRenderer().useConfigPath(layoutPath);
  renderer.getImguiRenderer().setClearColor(
      Theme::getColor(ThemeColor::BackgroundColor));
  renderer.getImguiRenderer().buildFonts();

  FileTracker tracker(project.assetsPath);
  tracker.trackForChanges();

  WorkspaceState state{project, assetManager.getAssetRegistry()};
  state.scene.entityDatabase.reg<CameraLookAt>();

  ActionExecutor actionExecutor(state, project.scenesPath / "main.lqscene");
  actionExecutor.getSceneIO().loadScene(project.scenesPath / "main.lqscene");

  EditorCamera editorCamera(state.scene.entityDatabase, mEventSystem, mWindow);

  EditorManager editorManager(editorCamera, project);
  editorManager.loadWorkspaceState(statePath, state);

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

  auto scenePassGroup = renderer.getSceneRenderer().attach(graph);
  auto imguiPassGroup = renderer.getImguiRenderer().attach(graph);
  imguiPassGroup.pass.read(scenePassGroup.sceneColor);

  {
    static constexpr glm::vec4 BlueishClearValue{0.52f, 0.54f, 0.89f, 1.0f};
    auto &pass = editorRenderer.attach(graph);
    pass.write(scenePassGroup.sceneColor, BlueishClearValue);
    pass.write(scenePassGroup.depthBuffer, rhi::DepthStencilClear{1.0f, 0});
  }

  renderer.getSceneRenderer().attachText(graph, scenePassGroup);

  MousePickingGraph mousePicking(
      renderer.getShaderLibrary(), renderer.getSceneRenderer().getFrameData(),
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

  EditorSimulator simulator(mEventSystem, mWindow,
                            assetManager.getAssetRegistry(), editorCamera);

  mWindow.maximize();

  mainLoop.setUpdateFn([&state, &simulator, this](float dt) mutable {
    mEventSystem.poll();
    simulator.update(dt, state);
    return true;
  });

  LogViewer logViewer;
  mainLoop.setRenderFn([&renderer, &editorManager, &editorCamera, &assetManager,
                        &graph, &scenePassGroup, &imguiPassGroup, &ui,
                        &debugLayer, &preloadStatusDialog, &presenter,
                        &editorRenderer, &simulator, &mousePicking,
                        &systemLogStorage, &userLogStorage, &logViewer, &state,
                        &actionExecutor, this]() {
    // TODO: Why is -2.0f needed here
    static const float IconSize = ImGui::GetFrameHeight() - 2.0f;

    auto &imgui = renderer.getImguiRenderer();
    auto &sceneRenderer = renderer.getSceneRenderer();

    imgui.beginRendering();
    ImGuizmo::BeginFrame();

    ui.render(state, editorManager, assetManager);

    if (auto _ = widgets::MainMenuBar()) {
      debugLayer.renderMenu();
    }
    debugLayer.render();

    logViewer.render(systemLogStorage, userLogStorage);

    bool mouseClicked =
        ui.renderSceneView(state, scenePassGroup.sceneColor, editorCamera);

    StatusBar::render(editorManager);

    preloadStatusDialog.render();

    imgui.endRendering();

    if (renderer.getRenderStorage().recreateFramebufferRelativeTextures()) {
      presenter.updateFramebuffers(mDevice->getSwapchain());
      return;
    }

    const auto &renderFrame = mDevice->beginFrame();

    auto camera = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene.activeCamera
                      : state.camera;

    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      imgui.updateFrameData(renderFrame.frameIndex);
      sceneRenderer.updateFrameData(scene.entityDatabase, camera,
                                    renderFrame.frameIndex);
      editorRenderer.updateFrameData(scene.entityDatabase, camera, state,
                                     renderFrame.frameIndex);

      if (mousePicking.isSelectionPerformedInFrame(renderFrame.frameIndex)) {
        auto entity = mousePicking.getSelectedEntity();
        state.selectedEntity = entity;
      }

      mousePicking.compile();

      renderer.render(graph, renderFrame.commandList, renderFrame.frameIndex);

      if (mouseClicked) {
        auto mousePos = mWindow.getCurrentMousePosition();

        if (editorManager.getEditorCamera().isWithinViewport(mousePos)) {
          auto scaledMousePos =
              editorManager.getEditorCamera().scaleToViewport(mousePos);

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
  editorManager.saveWorkspaceState(state, statePath);

  mDevice->waitForIdle();
}

} // namespace liquid::editor
