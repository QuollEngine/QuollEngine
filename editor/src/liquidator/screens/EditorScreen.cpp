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
  LogMemoryStorage userLogStorage;
  Engine::getUserLogger().setTransport(userLogStorage.createTransport());

  FPSCounter fpsCounter;

  auto layoutPath = (project.settingsPath / "layout.ini").string();
  auto statePath = project.settingsPath / "state.lqstate";

  RenderStorage renderStorage(mDevice);

  liquid::RendererOptions initialOptions{};
  initialOptions.size = mWindow.getFramebufferSize();
  Renderer renderer(renderStorage, initialOptions);

  AssetManager assetManager(project.assetsPath, project.assetsCachePath,
                            renderStorage, true, true);

  SceneRenderer sceneRenderer(assetManager.getAssetRegistry(), renderStorage);

  ImguiRenderer imguiRenderer(mWindow, renderStorage);

  Presenter presenter(renderStorage);

  presenter.updateFramebuffers(mDevice->getSwapchain());

  auto res = assetManager.validateAndPreloadAssets(renderStorage);
  AssetLoadStatusDialog loadStatusDialog("Loaded with warnings");

  if (res.hasWarnings()) {
    for (const auto &warning : res.getWarnings()) {
      Engine::getUserLogger().warning() << warning;
    }

    loadStatusDialog.setMessages(res.getWarnings());
    loadStatusDialog.show();
  }

  Theme::apply();

  imguiRenderer.useConfigPath(layoutPath);
  imguiRenderer.setClearColor(Theme::getColor(ThemeColor::BackgroundColor));
  imguiRenderer.buildFonts();

  FileTracker tracker(project.assetsPath);
  tracker.trackForChanges();

  EditorCamera editorCamera(mEventSystem, mWindow);

  WorkspaceState state{project};

  ActionExecutor actionExecutor(state, assetManager.getAssetRegistry(),
                                project.scenesPath / "main.lqscene");
  actionExecutor.getSceneIO().loadScene(project.scenesPath / "main.lqscene");

  state.scene.entityDatabase.reg<CameraLookAt>();
  state.camera = editorCamera.createDefaultCamera(state.scene.entityDatabase);
  state.activeCamera = state.camera;

  EditorManager::loadWorkspaceState(statePath, state);

  MainLoop mainLoop(mWindow, fpsCounter);
  AssetLoader assetLoader(assetManager, renderStorage);

  ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                             mDevice->getDeviceStats(), fpsCounter);

  UIRoot ui(actionExecutor, assetLoader);
  ui.getIconRegistry().loadIcons(
      renderStorage, std::filesystem::current_path() / "assets" / "icons");

  EditorRenderer editorRenderer(ui.getIconRegistry(), renderStorage, mDevice);

  renderer.setGraphBuilder([&](auto &graph, const auto &options) {
    auto scenePassGroup = sceneRenderer.attach(graph, options);
    auto imguiPassGroup = imguiRenderer.attach(graph, options);
    imguiPassGroup.pass.read(scenePassGroup.finalColor);
    editorRenderer.attach(graph, scenePassGroup, options);
    sceneRenderer.attachText(graph, scenePassGroup);

    return RendererTextures{imguiPassGroup.imguiColor,
                            scenePassGroup.finalColor};
  });

  MousePickingGraph mousePicking(sceneRenderer.getFrameData(),
                                 assetManager.getAssetRegistry(),
                                 renderStorage);

  mousePicking.setFramebufferSize(mWindow.getFramebufferSize());

  mWindow.addResizeHandler([&](auto width, auto height) {
    renderer.setFramebufferSize({width, height});
    mousePicking.setFramebufferSize({width, height});
    presenter.enqueueFramebufferUpdate();
  });

  mWindow.addFocusHandler([&tracker, &loadStatusDialog, &assetManager,
                           &renderer, &ui](bool focused) {
    if (!focused)
      return;

    const auto &changes = tracker.trackForChanges();
    std::vector<String> messages;
    for (auto &change : changes) {
      auto res = assetManager.loadOriginalIfChanged(change.path);
      if (res.hasError()) {
        messages.push_back(res.getError());

        Engine::getUserLogger().error() << res.getError();
      } else {
        messages.insert(messages.end(), res.getWarnings().begin(),
                        res.getWarnings().end());

        for (const auto &warning : res.getWarnings()) {
          Engine::getUserLogger().warning() << warning;
        }
      }
    }

    ui.getAssetBrowser().reload();

    if (!messages.empty()) {
      loadStatusDialog.setMessages(messages);
      loadStatusDialog.show();
    }
  });

  ui.processShortcuts(mEventSystem);

  EditorSimulator simulator(mEventSystem, mWindow,
                            assetManager.getAssetRegistry(), editorCamera);

  mWindow.maximize();

  mainLoop.setUpdateFn(
      [&state, &actionExecutor, &simulator, this](float dt) mutable {
        actionExecutor.process();
        mEventSystem.poll();
        simulator.update(dt, state);
        return true;
      });

  LogViewer logViewer;
  mainLoop.setRenderFn([&]() {
    if (presenter.requiresFramebufferUpdate()) {
      mDevice->recreateSwapchain();
      presenter.updateFramebuffers(mDevice->getSwapchain());
      return;
    }

    renderer.rebuildIfSettingsChanged();

    // TODO: Why is -2.0f needed here
    static const float IconSize = ImGui::GetFrameHeight() - 2.0f;

    imguiRenderer.beginRendering();
    ImGuizmo::BeginFrame();

    ui.render(state, assetManager);

    if (auto _ = widgets::MainMenuBar()) {
      debugLayer.renderMenu();
    }
    debugLayer.render();

    logViewer.render(userLogStorage);

    bool mouseClicked =
        ui.renderSceneView(state, renderer.getSceneTexture(), editorCamera);

    StatusBar::render(editorCamera);

    loadStatusDialog.render();

    imguiRenderer.endRendering();

    const auto &renderFrame = mDevice->beginFrame();

    auto &scene = state.mode == WorkspaceMode::Simulation
                      ? state.simulationScene
                      : state.scene;

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      imguiRenderer.updateFrameData(renderFrame.frameIndex);
      sceneRenderer.updateFrameData(scene.entityDatabase, state.activeCamera,
                                    renderFrame.frameIndex);
      editorRenderer.updateFrameData(scene.entityDatabase, state.activeCamera,
                                     state, assetManager.getAssetRegistry(),
                                     renderFrame.frameIndex);

      if (mousePicking.isSelectionPerformedInFrame(renderFrame.frameIndex)) {
        auto entity = mousePicking.getSelectedEntity();
        state.selectedEntity = entity;
      }

      renderer.execute(renderFrame.commandList, renderFrame.frameIndex);

      if (mouseClicked) {
        auto mousePos = mWindow.getCurrentMousePosition();

        if (editorCamera.isWithinViewport(mousePos)) {
          auto scaledMousePos = editorCamera.scaleToViewport(mousePos);
          mousePicking.execute(renderFrame.commandList, scaledMousePos,
                               renderFrame.frameIndex);
        }
        mouseClicked = false;
      }

      presenter.present(renderFrame.commandList, renderer.getFinalTexture(),
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
