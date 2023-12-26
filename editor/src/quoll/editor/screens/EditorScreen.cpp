#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "EditorScreen.h"

#include "quoll/renderer/Renderer.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/renderer/StandardPushConstants.h"
#include "quoll/profiler/ImguiDebugLayer.h"
#include "quoll/scene/SceneUpdater.h"
#include "quoll/scene/SkeletonUpdater.h"
#include "quoll/lua-scripting/LuaScriptingSystem.h"
#include "quoll/renderer/Presenter.h"
#include "quoll/renderer/SceneRenderer.h"
#include "quoll/imgui/ImguiRenderer.h"
#include "quoll/asset/FileTracker.h"
#include "quoll/audio/AudioSystem.h"
#include "quoll/logger/StreamTransport.h"

#include "quoll/physics/PhysicsSystem.h"
#include "quoll/loop/MainLoop.h"
#include "quoll/imgui/ImguiUtils.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"

#include "quoll/editor/ui/AssetLoadStatusDialog.h"
#include "quoll/editor/ui/Theme.h"
#include "quoll/editor/ui/Widgets.h"
#include "quoll/editor/ui/MainMenuBar.h"
#include "quoll/editor/ui/LogViewer.h"
#include "quoll/editor/ui/FontAwesome.h"
#include "quoll/editor/ui/StyleStack.h"

#include "quoll/editor/core/LogMemoryStorage.h"

#include "quoll/editor/asset/AssetManager.h"

#include "quoll/editor/scene/core/EditorCamera.h"
#include "quoll/editor/scene/SceneEditorWorkspace.h"
#include "quoll/editor/workspace/WorkspaceManager.h"
#include "quoll/editor/workspace/WorkspaceTabs.h"

#include "quoll/ui/UICanvasUpdater.h"

#include "ImGuizmo.h"

namespace quoll::editor {

EditorScreen::EditorScreen(Window &window, InputDeviceManager &deviceManager,
                           EventSystem &eventSystem, rhi::RenderDevice *device)
    : mDeviceManager(deviceManager), mWindow(window), mEventSystem(eventSystem),
      mDevice(device) {}

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

  QuollAssert(sceneAsset != SceneAssetHandle::Null,
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

  MainLoop mainLoop(mWindow, fpsCounter);

  ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                             mDevice->getDeviceStats(), fpsCounter);

  IconRegistry::loadIcons(renderStorage,
                          std::filesystem::current_path() / "assets" / "icons");

  SceneRenderer sceneRenderer(assetManager.getAssetRegistry(), renderStorage);
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

  mWindow.addFramebufferResizeHandler([&](auto width, auto height) {
    renderer.setFramebufferSize({width, height});
    mousePicking.setFramebufferSize({width, height});
    presenter.enqueueFramebufferUpdate();
  });

  SceneSimulator simulator(mDeviceManager, mWindow,
                           assetManager.getAssetRegistry(), editorCamera);

  mWindow.maximize();

  // Workspace manager
  WorkspaceManager workspaceManager;
  workspaceManager.add(new SceneEditorWorkspace(
      project, assetManager, sceneAsset,
      project.assetsPath / "scenes" / "main.scene", renderer, sceneRenderer,
      editorRenderer, mousePicking, simulator, workspaceManager));
  mEventSystem.observe(KeyboardEvent::Pressed, [&](const auto &data) {
    workspaceManager.getCurrentWorkspace()->processShortcuts(data.key,
                                                             data.mods);
  });

  mWindow.addFocusHandler([&tracker, &loadStatusDialog, &assetManager,
                           &renderer, &workspaceManager](bool focused) {
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

    workspaceManager.getCurrentWorkspace()->reload();

    if (!messages.empty()) {
      loadStatusDialog.setMessages(messages);
      loadStatusDialog.show();
    }
  });

  mainLoop.setUpdateFn([&workspaceManager, this](f32 dt) mutable {
    mEventSystem.poll();

    workspaceManager.getCurrentWorkspace()->update(dt);
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
    static const f32 IconSize = ImGui::GetFrameHeight() - 2.0f;

    imguiRenderer.beginRendering();
    ImGuizmo::BeginFrame();

    auto *workspace = workspaceManager.getCurrentWorkspace();
    workspace->render();

    if (auto _ = MainMenuBar()) {
      debugLayer.renderMenu();

      WorkspaceTabs::render(workspaceManager);
    }

    debugLayer.render();

    logViewer.render(userLogStorage);

    StatusBar::render(editorCamera);

    loadStatusDialog.render();

    imguiRenderer.endRendering();

    const auto &renderFrame = mDevice->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<u32>::max()) {
      imguiRenderer.updateFrameData(renderFrame.frameIndex);
      workspace->updateFrameData(renderFrame.commandList,
                                 renderFrame.frameIndex);

      renderer.execute(renderFrame.commandList, renderFrame.frameIndex);

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
