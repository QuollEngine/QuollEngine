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
#include "liquid/audio/AudioSystem.h"

#include "liquid/physics/PhysicsSystem.h"
#include "liquid/loop/MainLoop.h"
#include "liquid/imgui/ImguiUtils.h"
#include "liquid/scene/CameraAspectRatioUpdater.h"

#include "liquidator/editor-scene/EditorCamera.h"
#include "liquidator/editor-scene/EditorManager.h"
#include "liquidator/editor-scene/EditorGrid.h"
#include "liquidator/ui/UIRoot.h"
#include "liquidator/ui/AssetLoadStatusDialog.h"
#include "liquidator/ui/Theme.h"
#include "liquidator/ui/Widgets.h"
#include "liquidator/ui/TransformOperationControl.h"

#include "liquidator/core/EditorRenderer.h"
#include "liquidator/core/EditorSimulator.h"
#include "liquidator/core/MousePickingGraph.h"

#include "liquidator/asset/AssetManager.h"

#include "ImGuizmo.h"

namespace liquidator {

/**
 * @brief Get imguizmo operation
 *
 * @param transformOperation Transform operation
 * @return Imguizmo operation
 */
static ImGuizmo::OPERATION
getImguizmoOperation(TransformOperation transformOperation) {
  switch (transformOperation) {
  case TransformOperation::Scale:
    return ImGuizmo::SCALE;
  case TransformOperation::Rotate:
    return ImGuizmo::ROTATE;
  case TransformOperation::Move:
  default:
    return ImGuizmo::TRANSLATE;
  }
}

EditorScreen::EditorScreen(liquid::Window &window,
                           liquid::EventSystem &eventSystem,
                           liquid::rhi::RenderDevice *device)
    : mWindow(window), mEventSystem(eventSystem), mDevice(device) {}

void EditorScreen::start(const Project &project) {
  liquid::FPSCounter fpsCounter;

  auto layoutPath = (project.settingsPath / "layout.ini").string();
  auto statePath = project.settingsPath / "state.lqstate";

  AssetManager assetManager(project.assetsPath, project.assetsCachePath);

  auto &assetsCache = assetManager.getAssetsCache();

  liquid::Renderer renderer(assetsCache.getRegistry(), mWindow, mDevice);

  liquid::Presenter presenter(renderer.getShaderLibrary(), mDevice);

  presenter.updateFramebuffers(mDevice->getSwapchain());

  auto res = assetManager.validateAndPreloadAssets(mDevice);
  liquidator::AssetLoadStatusDialog preloadStatusDialog("Loaded with warnings");

  if (res.hasWarnings()) {
    preloadStatusDialog.setMessages(res.getWarnings());
    preloadStatusDialog.show();
  }

  Theme::apply();

  renderer.getImguiRenderer().useConfigPath(layoutPath);
  renderer.getImguiRenderer().setClearColor(
      Theme::getColor(ThemeColor::BackgroundColor));
  renderer.getImguiRenderer().buildFonts();

  renderer.getSceneRenderer().setClearColor(
      Theme::getColor(ThemeColor::SceneBackgroundColor));

  liquid::FileTracker tracker(project.assetsPath);
  tracker.trackForChanges();

  liquidator::EntityManager entityManager(assetsCache, renderer,
                                          project.scenesPath);
  liquidator::EditorCamera editorCamera(entityManager.getActiveEntityDatabase(),
                                        mEventSystem, mWindow);
  liquidator::EditorGrid editorGrid;
  liquidator::EditorManager editorManager(editorCamera, editorGrid,
                                          entityManager, project);

  editorManager.loadOrCreateScene();
  editorManager.loadEditorState(statePath);

  liquid::MainLoop mainLoop(mWindow, fpsCounter);
  liquidator::AssetLoader assetLoader(assetManager, mDevice);

  liquid::ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                                     mDevice->getDeviceStats(), fpsCounter);

  liquidator::UIRoot ui(entityManager, assetLoader);
  ui.getIconRegistry().loadIcons(mDevice, std::filesystem::current_path() /
                                              "assets" / "icons");

  liquidator::EditorRenderer editorRenderer(renderer.getShaderLibrary(),
                                            ui.getIconRegistry(), mDevice);

  liquid::rhi::RenderGraph graph;

  auto scenePassGroup = renderer.getSceneRenderer().attach(graph);
  auto imguiPassGroup = renderer.getImguiRenderer().attach(graph);
  imguiPassGroup.pass.read(scenePassGroup.sceneColor);

  {
    static constexpr glm::vec4 BlueishClearValue{0.52f, 0.54f, 0.89f, 1.0f};
    auto &pass = editorRenderer.attach(graph);
    pass.write(scenePassGroup.sceneColor, BlueishClearValue);
    pass.write(scenePassGroup.depthBuffer,
               liquid::rhi::DepthStencilClear{1.0f, 0});
  }

  renderer.getSceneRenderer().attachText(graph, scenePassGroup);

  MousePickingGraph mousePicking(renderer.getShaderLibrary(),
                                 renderer.getSceneRenderer().getFrameData(),
                                 assetsCache.getRegistry(), mDevice);

  mousePicking.setFramebufferSize(mWindow);
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
          assetManager.loadOriginalIfChanged(change.path);
        }

        ui.getAssetBrowser().reload();
      });

  liquidator::EditorSimulator simulator(
      mEventSystem, mWindow, assetsCache.getRegistry(), editorCamera);

  mWindow.maximize();

  mainLoop.setUpdateFn(
      [&editorCamera, &entityManager, &simulator, this](float dt) mutable {
        auto &entityDatabase = entityManager.getActiveEntityDatabase();

        mEventSystem.poll();
        simulator.update(dt, entityManager.getActiveScene());
        return true;
      });

  mainLoop.setRenderFn([&renderer, &editorManager, &entityManager, &assetsCache,
                        &assetManager, &graph, &scenePassGroup, &imguiPassGroup,
                        &ui, &debugLayer, &preloadStatusDialog, &presenter,
                        &editorRenderer, &simulator, &mousePicking, this]() {
    auto &entityDatabase = entityManager.getActiveEntityDatabase();

    // TODO: Why is -2.0f needed here
    static const float IconSize = ImGui::GetFrameHeight() - 2.0f;

    auto &imgui = renderer.getImguiRenderer();
    auto &sceneRenderer = renderer.getSceneRenderer();

    imgui.beginRendering();
    ImGuizmo::BeginFrame();

    if (auto _ = widgets::MainMenuBar()) {
      liquidator::MenuBar::render(editorManager, entityManager);
      debugLayer.renderMenu();
    }

    debugLayer.render();

    if (auto _ = Toolbar()) {
      auto icon =
          entityManager.isUsingSimulationDatabase()
              ? ui.getIconRegistry().getIcon(liquidator::EditorIcon::Stop)
              : ui.getIconRegistry().getIcon(liquidator::EditorIcon::Play);

      if (liquid::imgui::imageButton(icon, ImVec2(IconSize, IconSize))) {
        if (entityManager.isUsingSimulationDatabase()) {
          simulator.cleanupSimulationDatabase(
              entityManager.getActiveEntityDatabase());
          simulator.useEditorUpdate();
          entityManager.useEditingDatabase();
        } else {
          simulator.useSimulationUpdate();
          entityManager.useSimulationDatabase();
        }
      }

      ImGui::SameLine();

      TransformOperationControl(ui.getIconRegistry(), editorManager, IconSize);

      ImGui::SameLine();
    }

    bool mouseClicked = false;

    ui.render(editorManager, renderer, assetManager,
              simulator.getPhysicsSystem(), entityManager);

    if (auto _ = SceneView(scenePassGroup.sceneColor)) {
      const auto &pos = ImGui::GetItemRectMin();
      const auto &size = ImGui::GetItemRectSize();

      editorManager.getEditorCamera().setViewport(pos.x, pos.y, size.x, size.y,
                                                  ImGui::IsItemHovered());

      mouseClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);

      const auto &editorCamera = editorManager.getEditorCamera();

      ImGuizmo::SetDrawlist();
      ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

      if (ui.getSceneHierarchyPanel().isEntitySelected()) {
        auto selected = ui.getSceneHierarchyPanel().getSelectedEntity();
        const auto &world =
            entityDatabase.get<liquid::WorldTransformComponent>(selected);

        auto worldTransform = world.worldTransform;

        const auto &camera = entityDatabase.get<liquid::CameraComponent>(
            editorManager.getEditorCamera().getCamera());

        auto gizmoPerspective = camera.projectionMatrix;

        if (ImGuizmo::Manipulate(
                glm::value_ptr(camera.viewMatrix),
                glm::value_ptr(gizmoPerspective),
                getImguizmoOperation(editorManager.getTransformOperation()),
                ImGuizmo::LOCAL, glm::value_ptr(worldTransform), nullptr,
                nullptr, nullptr)) {
          entityManager.updateLocalTransformUsingWorld(selected,
                                                       worldTransform);
        }

        if (ImGuizmo::IsOver()) {
          mouseClicked = false;
        }
      }
    }

    StatusBar::render(editorManager);

    preloadStatusDialog.render();

    imgui.endRendering();

    const auto &renderFrame = mDevice->beginFrame();

    auto camera = entityManager.isUsingSimulationDatabase()
                      ? entityManager.getActiveSimulationCamera()
                      : editorManager.getEditorCamera().getCamera();

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      imgui.updateFrameData(renderFrame.frameIndex);
      sceneRenderer.updateFrameData(entityManager.getActiveEntityDatabase(),
                                    camera, renderFrame.frameIndex);
      editorRenderer.updateFrameData(
          entityManager.getActiveEntityDatabase(), camera,
          editorManager.getEditorGrid(),
          ui.getSceneHierarchyPanel().getSelectedEntity(),
          renderFrame.frameIndex);

      if (mousePicking.isSelectionPerformedInFrame(renderFrame.frameIndex)) {
        auto entity = mousePicking.getSelectedEntity();
        ui.getSceneHierarchyPanel().setSelectedEntity(entity);
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
  editorManager.saveEditorState(statePath);

  mDevice->waitForIdle();
}

} // namespace liquidator
