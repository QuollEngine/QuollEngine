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

#include "liquidator/workspace/Workspace.h"

#include "ImGuizmo.h"

namespace quoll::editor {

EditorScreen::EditorScreen(Window &window, EventSystem &eventSystem,
                           rhi::RenderDevice *device)
    : mWindow(window), mEventSystem(eventSystem), mDevice(device) {}

void EditorScreen::start(const Project &rawProject) {
  auto project = rawProject;

  LogMemoryStorage userLogStorage;
  Engine::getUserLogger().setTransport(userLogStorage.createTransport());

  FPSCounter fpsCounter;

  RenderStorage renderStorage(mDevice);

  quoll::RendererOptions initialOptions{};
  initialOptions.size = mWindow.getFramebufferSize();
  Renderer renderer(renderStorage, initialOptions);

  AssetManager assetManager(project.assetsPath, project.assetsCachePath,
                            renderStorage, true, true);

  SceneRenderer sceneRenderer(assetManager.getAssetRegistry(), renderStorage);

  ImguiRenderer imguiRenderer(mWindow, renderStorage);

  Presenter presenter(renderStorage);

  presenter.updateFramebuffers(mDevice->getSwapchain());

  auto res = assetManager.validateAndPreloadAssets(renderStorage);

  SceneAssetHandle sceneAsset = SceneAssetHandle::Null;
  for (auto [handle, data] :
       assetManager.getAssetRegistry().getScenes().getAssets()) {
    sceneAsset = handle;
    project.startingScene = data.uuid;
  }

  LIQUID_ASSERT(sceneAsset != SceneAssetHandle::Null,
                "Scene asset does not exist");
  if (sceneAsset == SceneAssetHandle::Null) {
    return;
  }

  AssetLoadStatusDialog loadStatusDialog("Loaded with warnings");

  if (res.hasWarnings()) {
    for (const auto &warning : res.getWarnings()) {
      Engine::getUserLogger().warning() << warning;
    }

    loadStatusDialog.setMessages(res.getWarnings());
    loadStatusDialog.show();
  }

  Theme::apply();
  imguiRenderer.setClearColor(Theme::getClearColor());
  imguiRenderer.buildFonts();

  FileTracker tracker(project.assetsPath);
  tracker.trackForChanges();

  EditorCamera editorCamera(mEventSystem, mWindow);

  Workspace workspace(project, assetManager, sceneAsset,
                      project.assetsPath / "main.scene");

  auto context = workspace.getContext();

  auto &state = context.state;

  UIRoot ui(assetManager);

  MainLoop mainLoop(mWindow, fpsCounter);

  ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                             mDevice->getDeviceStats(), fpsCounter);

  IconRegistry::loadIcons(renderStorage,
                          std::filesystem::current_path() / "assets" / "icons");

  EditorRenderer editorRenderer(renderStorage, mDevice);

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
      auto res = assetManager.loadSourceIfChanged(change.path);
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

  ui.processShortcuts(context, mEventSystem);

  EditorSimulator simulator(mEventSystem, mWindow,
                            assetManager.getAssetRegistry(), editorCamera);

  mWindow.maximize();

  mainLoop.setUpdateFn(
      [&state, &workspace, &simulator, this](float dt) mutable {
        workspace.update();
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

    workspace.renderLayout();

    ui.render(context);

    if (auto _ = widgets::MainMenuBar()) {
      debugLayer.renderMenu();
    }
    debugLayer.render();

    logViewer.render(userLogStorage);

    bool mouseClicked =
        ui.renderSceneView(context, renderer.getSceneTexture(), editorCamera);

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
        context.state.selectedEntity = entity;
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

  mDevice->waitForIdle();
}

} // namespace quoll::editor
