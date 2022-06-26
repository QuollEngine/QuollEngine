#include "liquid/core/Base.h"
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
#include "liquid/core/EntityDeleter.h"
#include "liquid/audio/AudioSystem.h"

#include "liquid/physics/PhysicsSystem.h"
#include "liquid/loop/MainLoop.h"
#include "liquid/imgui/ImguiUtils.h"
#include "liquid/scene/CameraAspectRatioUpdater.h"

#include "../editor-scene/EditorCamera.h"
#include "../editor-scene/EditorManager.h"
#include "../editor-scene/EditorGrid.h"
#include "../ui/UIRoot.h"
#include "../ui/AssetLoadStatusDialog.h"
#include "../core/EditorRenderer.h"

void randomSpawn(liquidator::EntityManager &entityManager,
                 liquidator::EditorManager &editorManager,
                 liquid::AssetManager &assetManager) {

  constexpr float DISTRIBUTION_EDGE = 500.0f;
  std::random_device device;
  std::mt19937 mt(device());
  std::uniform_real_distribution<float> dist(-DISTRIBUTION_EDGE,
                                             DISTRIBUTION_EDGE);
  constexpr size_t NUM_SPAWNS = 1000;

  for (auto &[handle, _] :
       assetManager.getRegistry().getPrefabs().getAssets()) {
    for (size_t i = 0; i < NUM_SPAWNS; ++i) {
      auto parent = entityManager.spawnEntity(
          editorManager.getEditorCamera(), liquid::EntityNull,
          static_cast<uint32_t>(handle), liquid::AssetType::Prefab, false);
      entityManager.getActiveEntityDatabase()
          .getComponent<liquid::LocalTransformComponent>(parent)
          .localPosition = glm::vec3(dist(mt), dist(mt), dist(mt));
    }
  }
}

namespace liquidator {

EditorScreen::EditorScreen(liquid::Window &window,
                           liquid::EventSystem &eventSystem,
                           liquid::rhi::RenderDevice *device)
    : mWindow(window), mEventSystem(eventSystem), mDevice(device) {}

void EditorScreen::start(const Project &project) {
  liquid::FPSCounter fpsCounter;

  liquid::CameraAspectRatioUpdater aspectRatioUpdater(mWindow);

  auto layoutPath = (project.settingsPath / "layout.ini").string();
  auto statePath = project.settingsPath / "state.lqstate";

  liquid::AssetManager assetManager(project.assetsPath);
  liquid::Renderer renderer(assetManager.getRegistry(), mWindow, mDevice);

  liquid::Presenter presenter(renderer.getShaderLibrary(),
                              renderer.getRegistry());

  presenter.updateFramebuffers(mDevice->getSwapchain());

  auto res = assetManager.preloadAssets(renderer.getRegistry());
  liquidator::AssetLoadStatusDialog preloadStatusDialog("Loaded with warnings");
  preloadStatusDialog.setMessages(res.getWarnings());

  renderer.getImguiRenderer().useConfigPath(layoutPath);

  if (res.hasWarnings()) {
    preloadStatusDialog.show();
  }

  liquid::FileTracker tracker(project.assetsPath);
  tracker.trackForChanges();

  liquidator::EntityManager entityManager(assetManager, renderer,
                                          project.scenePath);
  liquidator::EditorCamera editorCamera(entityManager.getActiveEntityDatabase(),
                                        mEventSystem, renderer, mWindow);
  liquidator::EditorGrid editorGrid;
  liquidator::EditorManager editorManager(editorCamera, editorGrid,
                                          entityManager);

  editorManager.loadOrCreateScene();
  editorManager.loadEditorState(statePath);

  liquid::MainLoop mainLoop(mWindow, fpsCounter);
  liquidator::GLTFImporter gltfImporter(assetManager, renderer.getRegistry());

  liquid::AnimationSystem animationSystem(assetManager.getRegistry());
  liquid::PhysicsSystem physicsSystem(mEventSystem);
  liquid::SceneUpdater sceneUpdater;
  liquid::SkeletonUpdater skeletonUpdater;
  liquid::AudioSystem audioSystem(assetManager.getRegistry());
  liquid::ScriptingSystem scriptingSystem(mEventSystem,
                                          assetManager.getRegistry());
  liquid::EntityDeleter entityDeleter;

  liquid::ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                                     mDevice->getDeviceStats(),
                                     renderer.getRegistry(), fpsCounter);

  liquidator::UIRoot ui(entityManager, gltfImporter);
  ui.getIconRegistry().loadIcons(renderer.getRegistry(),
                                 std::filesystem::current_path() / "assets" /
                                     "icons");

  mDevice->synchronize(renderer.getRegistry());

  liquidator::EditorRenderer editorRenderer(renderer.getRegistry(),
                                            renderer.getShaderLibrary(),
                                            ui.getIconRegistry());

  liquid::rhi::RenderGraph graph;

  auto scenePassGroup = renderer.getSceneRenderer().attach(graph);
  auto imguiPassGroup = renderer.getImguiRenderer().attach(graph);

  imguiPassGroup.pass.read(scenePassGroup.sceneColor);

  graph.setFramebufferExtent(mWindow.getFramebufferSize());

  mWindow.addResizeHandler([&graph](auto width, auto height) {
    graph.setFramebufferExtent({width, height});
  });

  mWindow.addFocusHandler(
      [&tracker, &assetManager, &renderer, &ui](bool focused) {
        if (!focused)
          return;

        const auto &changes = tracker.trackForChanges();
        for (auto &change : changes) {
          assetManager.loadAsset(change.path);
        }

        ui.getAssetBrowser().reload();
      });

  {
    constexpr glm::vec4 BLUEISH_CLEAR_VALUE{0.19f, 0.21f, 0.26f, 1.0f};
    auto &pass = editorRenderer.attach(graph);
    pass.write(scenePassGroup.sceneColor, BLUEISH_CLEAR_VALUE);
    pass.write(scenePassGroup.depthBuffer,
               liquid::rhi::DepthStencilClear{1.0f, 0});
  }

  mainLoop.setUpdateFn([&editorCamera, &animationSystem, &physicsSystem,
                        &entityManager, &aspectRatioUpdater, &skeletonUpdater,
                        &scriptingSystem, &sceneUpdater, &entityDeleter,
                        &audioSystem, this](float dt) mutable {
    bool isPlaying = entityManager.isUsingSimulationDatabase();

    auto &entityDatabase = entityManager.getActiveEntityDatabase();

    mEventSystem.poll();
    aspectRatioUpdater.update(entityDatabase);
    editorCamera.update();

    if (isPlaying) {
      scriptingSystem.start(entityDatabase);
      scriptingSystem.update(dt, entityDatabase);
      animationSystem.update(dt, entityDatabase);
    }

    skeletonUpdater.update(entityDatabase);
    sceneUpdater.update(entityDatabase);

    if (isPlaying) {
      physicsSystem.update(dt, entityDatabase);
      audioSystem.output(entityDatabase);
    }

    entityDeleter.update(entityDatabase);
    return true;
  });

  mainLoop.setRenderFn([&renderer, &editorManager, &animationSystem,
                        &entityManager, &assetManager, &graph, &scenePassGroup,
                        &imguiPassGroup, &physicsSystem, &ui, &debugLayer,
                        &preloadStatusDialog, &presenter, &editorRenderer,
                        &scriptingSystem, &audioSystem, this]() {
    auto &imgui = renderer.getImguiRenderer();
    auto &sceneRenderer = renderer.getSceneRenderer();

    imgui.beginRendering();
    ui.render(editorManager, renderer, assetManager, physicsSystem,
              entityManager);

    ImVec2 pos{};
    ImVec2 size{};
    static constexpr float ICON_SIZE = 20.0f;
    if (ImGui::Begin("View", nullptr)) {
      pos = ImGui::GetWindowPos();
      size = ImGui::GetContentRegionAvail();

      auto icon =
          entityManager.isUsingSimulationDatabase()
              ? ui.getIconRegistry().getIcon(liquidator::EditorIcon::Stop)
              : ui.getIconRegistry().getIcon(liquidator::EditorIcon::Play);

      if (liquid::imgui::imageButton(icon, ImVec2(ICON_SIZE, ICON_SIZE))) {
        if (entityManager.isUsingSimulationDatabase()) {
          physicsSystem.cleanup(entityManager.getActiveEntityDatabase());
          scriptingSystem.cleanup(entityManager.getActiveEntityDatabase());
          audioSystem.cleanup(entityManager.getActiveEntityDatabase());
          entityManager.useEditingDatabase();
        } else {
          entityManager.useSimulationDatabase();
        }
      }

      ImVec2 viewportSize(size.x, size.y - ICON_SIZE - ImGui::GetFrameHeight());
      editorManager.getEditorCamera().setViewport(
          pos.x, pos.y + ICON_SIZE, viewportSize.x, viewportSize.y);

      liquid::imgui::image(scenePassGroup.sceneColor, viewportSize);
      ImGui::End();
    }

    if (!editorManager.isUsingEditorCamera()) {
      ImGui::SetNextWindowPos(ImVec2(pos.x + size.x, pos.y + size.y), 0,
                              ImVec2(1.0f, 1.0f));

      if (ImGui::Begin("Reset editor button", nullptr,
                       ImGuiWindowFlags_NoDecoration |
                           ImGuiWindowFlags_NoSavedSettings |
                           ImGuiWindowFlags_NoDocking)) {
        if (ImGui::Button("Reset to editor camera")) {
          editorManager.switchToEditorCamera();
        }
        ImGui::End();
      }
    }

#ifdef LIQUID_PROFILER
    if (ImGui::Begin("Benchmark")) {
      if (ImGui::Button("Spawn prefabs")) {
        randomSpawn(entityManager, editorManager, assetManager);
      }

      ImGui::End();
    }
#endif

    preloadStatusDialog.render();
    debugLayer.render();

    imgui.endRendering();

    const auto &renderFrame = mDevice->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      imgui.updateFrameData(renderFrame.frameIndex);
      sceneRenderer.updateFrameData(entityManager.getActiveEntityDatabase(),
                                    editorManager.getCamera());
      editorRenderer.updateFrameData(entityManager.getActiveEntityDatabase(),
                                     editorManager.getCamera(),
                                     editorManager.getEditorGrid());

      renderer.render(graph, renderFrame.commandList);

      presenter.present(renderFrame.commandList, imguiPassGroup.imguiColor,
                        renderFrame.swapchainImageIndex);

      mDevice->endFrame(renderFrame);
    } else {
      presenter.updateFramebuffers(mDevice->getSwapchain());
    }
  });

  mainLoop.run();
  editorManager.saveEditorState(statePath);
}

} // namespace liquidator
