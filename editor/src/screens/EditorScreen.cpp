#include "liquid/core/Base.h"
#include "EditorScreen.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/renderer/StandardPushConstants.h"
#include "liquid/profiler/ImguiDebugLayer.h"
#include "liquid/scene/SceneUpdater.h"
#include "liquid/scene/SkeletonUpdater.h"
#include "liquid/renderer/Presenter.h"

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
      entityManager.getActiveEntityContext()
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
  liquid::DebugManager debugManager;
  liquid::FPSCounter fpsCounter;

  liquid::CameraAspectRatioUpdater aspectRatioUpdater(mWindow);

  auto layoutPath = (project.settingsPath / "layout.ini").string();
  auto statePath = project.settingsPath / "state.lqpath";

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

  liquidator::EntityManager entityManager(assetManager, renderer,
                                          project.scenePath);
  liquidator::EditorCamera editorCamera(entityManager.getActiveEntityContext(),
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

  liquid::ImguiDebugLayer debugLayer(
      mDevice->getDeviceInformation(), mDevice->getDeviceStats(),
      renderer.getRegistry(), fpsCounter, debugManager);

  liquidator::UIRoot ui(entityManager, gltfImporter);
  ui.getIconRegistry().loadIcons(renderer.getRegistry(),
                                 std::filesystem::current_path() / "assets" /
                                     "icons");

  mDevice->synchronize(renderer.getRegistry());

  liquidator::EditorRenderer editorRenderer(renderer.getRegistry(),
                                            renderer.getShaderLibrary(),
                                            ui.getIconRegistry());

  auto graph = renderer.createRenderGraph(false);

  graph.first.setFramebufferExtent(mWindow.getFramebufferSize());

  mWindow.addResizeHandler([&graph](auto width, auto height) {
    graph.first.setFramebufferExtent({width, height});
  });

  auto &pass = editorRenderer.attach(graph.first);
  pass.read(graph.second.mainColor);
  pass.write(graph.second.mainColor, graph.second.defaultColor);
  pass.write(graph.second.depthBuffer, liquid::rhi::DepthStencilClear{1.0f, 0});

  mainLoop.setUpdateFn([&editorCamera, &animationSystem, &physicsSystem,
                        &entityManager, &aspectRatioUpdater, &skeletonUpdater,
                        &sceneUpdater, this](float dt) mutable {
    bool isPlaying = entityManager.isUsingSimulationContext();

    auto &entityContext = entityManager.getActiveEntityContext();

    mEventSystem.poll();
    aspectRatioUpdater.update(entityContext);
    editorCamera.update();

    if (isPlaying) {
      animationSystem.update(dt, entityContext);
    }

    skeletonUpdater.update(entityContext);
    sceneUpdater.update(entityContext);

    if (isPlaying) {
      physicsSystem.update(dt, entityContext);
    }
    return true;
  });

  mainLoop.setRenderFn([&renderer, &editorManager, &animationSystem,
                        &entityManager, &assetManager, &graph, &physicsSystem,
                        &ui, &debugLayer, &preloadStatusDialog, &presenter,
                        &editorRenderer, this]() {
    auto &imgui = renderer.getImguiRenderer();

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
          entityManager.isUsingSimulationContext()
              ? ui.getIconRegistry().getIcon(liquidator::EditorIcon::Stop)
              : ui.getIconRegistry().getIcon(liquidator::EditorIcon::Play);

      if (liquid::imgui::imageButton(icon, ImVec2(ICON_SIZE, ICON_SIZE))) {
        if (entityManager.isUsingSimulationContext()) {
          physicsSystem.cleanup(entityManager.getActiveEntityContext());
          entityManager.useEditingContext();
        } else {
          entityManager.useSimulationContext();
        }
      }

      ImVec2 viewportSize(size.x, size.y - ICON_SIZE - ImGui::GetFrameHeight());
      editorManager.getEditorCamera().setViewport(
          pos.x, pos.y + ICON_SIZE, viewportSize.x, viewportSize.y);

      liquid::imgui::image(graph.second.mainColor, viewportSize);
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
      editorRenderer.update(entityManager.getActiveEntityContext(),
                            editorManager.getCamera(),
                            editorManager.getEditorGrid());

      renderer.render(graph.first, renderFrame.commandList,
                      editorManager.getCamera(),
                      entityManager.getActiveEntityContext());

      presenter.present(renderFrame.commandList, graph.second.imguiColor,
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
