#include "liquid/core/Base.h"
#include "EditorScreen.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/renderer/StandardPushConstants.h"
#include "liquid/profiler/ImguiDebugLayer.h"
#include "liquid/scene/SceneUpdater.h"
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
  liquidator::EditorGrid editorGrid(renderer.getRegistry());
  liquidator::EditorManager editorManager(editorCamera, editorGrid,
                                          entityManager);

  editorManager.loadOrCreateScene();
  editorManager.loadEditorState(statePath);

  liquid::MainLoop mainLoop(mWindow, fpsCounter);
  liquidator::GLTFImporter gltfImporter(assetManager, renderer.getRegistry());

  liquid::AnimationSystem animationSystem(assetManager.getRegistry());
  liquid::PhysicsSystem physicsSystem(mEventSystem);
  liquid::SceneUpdater sceneUpdater;

  liquid::ImguiDebugLayer debugLayer(
      mDevice->getDeviceInformation(), mDevice->getDeviceStats(),
      renderer.getRegistry(), fpsCounter, debugManager);

  renderer.getShaderLibrary().addShader(
      "editor-grid.vert", renderer.getRegistry().setShader(
                              {"assets/shaders/editor-grid.vert.spv"}));
  renderer.getShaderLibrary().addShader(
      "editor-grid.frag", renderer.getRegistry().setShader(
                              {"assets/shaders/editor-grid.frag.spv"}));

  renderer.getShaderLibrary().addShader(
      "skeleton-lines.vert", renderer.getRegistry().setShader(
                                 {"assets/shaders/skeleton-lines.vert.spv"}));
  renderer.getShaderLibrary().addShader(
      "skeleton-lines.frag", renderer.getRegistry().setShader(
                                 {"assets/shaders/skeleton-lines.frag.spv"}));

  renderer.getShaderLibrary().addShader(
      "object-icons.vert", renderer.getRegistry().setShader(
                               {"assets/shaders/object-icons.vert.spv"}));
  renderer.getShaderLibrary().addShader(
      "object-icons.frag", renderer.getRegistry().setShader(
                               {"assets/shaders/object-icons.frag.spv"}));

  liquidator::UIRoot ui(entityManager, gltfImporter);
  ui.getIconRegistry().loadIcons(renderer.getRegistry(),
                                 std::filesystem::current_path() / "assets" /
                                     "icons");

  auto graph = renderer.createRenderGraph(false);

  graph.first.setFramebufferExtent(mWindow.getFramebufferSize());

  mWindow.addResizeHandler([&graph](auto width, auto height) {
    graph.first.setFramebufferExtent({width, height});
  });

  // Editor renderer
  {
    auto &pass = graph.first.addPass("editorDebug");
    pass.write(graph.second.mainColor, graph.second.defaultColor);
    pass.write(graph.second.depthBuffer,
               liquid::rhi::DepthStencilClear{1.0f, 0});

    auto editorGridPipeline = renderer.getRegistry().setPipeline(
        {renderer.getShaderLibrary().getShader("editor-grid.vert"),
         renderer.getShaderLibrary().getShader("editor-grid.frag"),
         {},
         liquid::rhi::PipelineInputAssembly{},
         liquid::rhi::PipelineRasterizer{liquid::rhi::PolygonMode::Fill,
                                         liquid::rhi::CullMode::None,
                                         liquid::rhi::FrontFace::Clockwise},
         liquid::rhi::PipelineColorBlend{
             {liquid::rhi::PipelineColorBlendAttachment{
                 true, liquid::rhi::BlendFactor::SrcAlpha,
                 liquid::rhi::BlendFactor::DstAlpha, liquid::rhi::BlendOp::Add,
                 liquid::rhi::BlendFactor::SrcAlpha,
                 liquid::rhi::BlendFactor::DstAlpha,
                 liquid::rhi::BlendOp::Add}}}});

    auto skeletonLinesPipeline = renderer.getRegistry().setPipeline(
        {renderer.getShaderLibrary().getShader("skeleton-lines.vert"),
         renderer.getShaderLibrary().getShader("skeleton-lines.frag"),
         {},
         liquid::rhi::PipelineInputAssembly{
             liquid::rhi::PrimitiveTopology::LineList},
         liquid::rhi::PipelineRasterizer{liquid::rhi::PolygonMode::Line,
                                         liquid::rhi::CullMode::None,
                                         liquid::rhi::FrontFace::Clockwise},
         liquid::rhi::PipelineColorBlend{
             {liquid::rhi::PipelineColorBlendAttachment{
                 true, liquid::rhi::BlendFactor::SrcAlpha,
                 liquid::rhi::BlendFactor::DstAlpha, liquid::rhi::BlendOp::Add,
                 liquid::rhi::BlendFactor::SrcAlpha,
                 liquid::rhi::BlendFactor::DstAlpha,
                 liquid::rhi::BlendOp::Add}}}});

    auto objectIconsPipeline = renderer.getRegistry().setPipeline(
        {renderer.getShaderLibrary().getShader("object-icons.vert"),
         renderer.getShaderLibrary().getShader("object-icons.frag"),
         {},
         liquid::rhi::PipelineInputAssembly{
             liquid::rhi::PrimitiveTopology::TriangleStrip},
         liquid::rhi::PipelineRasterizer{liquid::rhi::PolygonMode::Fill,
                                         liquid::rhi::CullMode::None,
                                         liquid::rhi::FrontFace::Clockwise},
         liquid::rhi::PipelineColorBlend{
             {liquid::rhi::PipelineColorBlendAttachment{
                 true, liquid::rhi::BlendFactor::SrcAlpha,
                 liquid::rhi::BlendFactor::DstAlpha, liquid::rhi::BlendOp::Add,
                 liquid::rhi::BlendFactor::SrcAlpha,
                 liquid::rhi::BlendFactor::DstAlpha,
                 liquid::rhi::BlendOp::Add}}}});

    pass.addPipeline(editorGridPipeline);
    pass.addPipeline(skeletonLinesPipeline);
    pass.addPipeline(objectIconsPipeline);

    pass.setExecutor([editorGridPipeline, skeletonLinesPipeline,
                      &objectIconsPipeline, &renderer, &editorCamera,
                      &editorGrid, &entityManager, &editorManager,
                      &ui](liquid::rhi::RenderCommandList &commandList) {
      liquid::rhi::Descriptor sceneDescriptor;
      sceneDescriptor.bind(0,
                           renderer.getRenderStorage().getActiveCameraBuffer(),
                           liquid::rhi::DescriptorType::UniformBuffer);

      liquid::rhi::Descriptor gridDescriptor;
      gridDescriptor.bind(0, editorGrid.getBuffer(),
                          liquid::rhi::DescriptorType::UniformBuffer);

      commandList.bindPipeline(editorGridPipeline);
      commandList.bindDescriptor(editorGridPipeline, 0, sceneDescriptor);
      commandList.bindDescriptor(editorGridPipeline, 1, gridDescriptor);

      constexpr uint32_t PLANE_VERTICES = 6;
      commandList.draw(PLANE_VERTICES, 0);

      commandList.bindPipeline(skeletonLinesPipeline);

      entityManager.getActiveEntityContext()
          .iterateEntities<liquid::WorldTransformComponent,
                           liquid::SkeletonComponent, liquid::DebugComponent>(
              [&commandList, &skeletonLinesPipeline,
               &renderer](auto entity, auto &world,
                          const liquid::SkeletonComponent &skeleton,
                          const liquid::DebugComponent &debug) {
                if (!debug.showBones)
                  return;

                liquid::rhi::Descriptor sceneDescriptor;
                sceneDescriptor.bind(
                    0, renderer.getRenderStorage().getActiveCameraBuffer(),
                    liquid::rhi::DescriptorType::UniformBuffer);

                liquid::rhi::Descriptor skeletonDescriptor;
                skeletonDescriptor.bind(
                    0, skeleton.skeleton.getDebugBuffer(),
                    liquid::rhi::DescriptorType::UniformBuffer);

                liquid::StandardPushConstants transformConstant{};
                transformConstant.modelMatrix = world.worldTransform;

                commandList.bindDescriptor(skeletonLinesPipeline, 0,
                                           sceneDescriptor);
                commandList.bindDescriptor(skeletonLinesPipeline, 1,
                                           skeletonDescriptor);

                commandList.pushConstants(
                    skeletonLinesPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                    sizeof(liquid::StandardPushConstants), &transformConstant);

                commandList.draw(skeleton.skeleton.getNumDebugBones(), 0);
              });

      commandList.bindPipeline(objectIconsPipeline);

      liquid::rhi::Descriptor objectListSceneDescriptor;
      objectListSceneDescriptor.bind(
          0, renderer.getRenderStorage().getActiveCameraBuffer(),
          liquid::rhi::DescriptorType::UniformBuffer);
      commandList.bindDescriptor(objectIconsPipeline, 0,
                                 objectListSceneDescriptor);

      entityManager.getActiveEntityContext()
          .iterateEntities<liquid::WorldTransformComponent,
                           liquid::DirectionalLightComponent>(
              [&objectIconsPipeline, &commandList, &ui, &entityManager](
                  auto entity, const auto &world, const auto &light) {
                liquid::rhi::Descriptor sunDescriptor;
                sunDescriptor.bind(
                    0,
                    {ui.getIconRegistry().getIcon(liquidator::EditorIcon::Sun)},
                    liquid::rhi::DescriptorType::CombinedImageSampler);
                commandList.bindDescriptor(objectIconsPipeline, 1,
                                           sunDescriptor);

                liquid::StandardPushConstants transformConstant{};
                transformConstant.modelMatrix = world.worldTransform;

                commandList.pushConstants(
                    objectIconsPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                    sizeof(liquid::StandardPushConstants), &transformConstant);

                commandList.draw(4, 0);

                if (entityManager.getActiveEntityContext()
                        .hasComponent<liquid::DebugComponent>(entity) &&
                    entityManager.getActiveEntityContext()
                        .getComponent<liquid::DebugComponent>(entity)
                        .showDirection) {
                  liquid::rhi::Descriptor directionDescriptor;
                  directionDescriptor.bind(
                      0,
                      {ui.getIconRegistry().getIcon(
                          liquidator::EditorIcon::Direction)},
                      liquid::rhi::DescriptorType::CombinedImageSampler);
                  commandList.bindDescriptor(objectIconsPipeline, 1,
                                             directionDescriptor);

                  liquid::StandardPushConstants pcDirection{};
                  static constexpr glm::vec3 LIGHT_DIR_ICON_POSITION{0.0f, 2.0f,
                                                                     0.0f};
                  pcDirection.modelMatrix = glm::translate(
                      world.worldTransform, LIGHT_DIR_ICON_POSITION);

                  commandList.pushConstants(
                      objectIconsPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                      sizeof(liquid::StandardPushConstants), &pcDirection);

                  commandList.draw(4, 0);
                }
              });

      entityManager.getActiveEntityContext()
          .iterateEntities<liquid::WorldTransformComponent,
                           liquid::PerspectiveLensComponent>(
              [&objectIconsPipeline, &commandList,
               &ui](auto entity, const auto &world, const auto &camera) {
                liquid::rhi::Descriptor sunDescriptor;
                sunDescriptor.bind(
                    0,
                    {ui.getIconRegistry().getIcon(
                        liquidator::EditorIcon::Camera)},
                    liquid::rhi::DescriptorType::CombinedImageSampler);
                commandList.bindDescriptor(objectIconsPipeline, 1,
                                           sunDescriptor);

                static constexpr float NINETY_DEGREES_IN_RADIANS =
                    glm::pi<float>() / 2.0f;

                liquid::StandardPushConstants transformConstant{};
                transformConstant.modelMatrix =
                    glm::rotate(world.worldTransform, NINETY_DEGREES_IN_RADIANS,
                                glm::vec3(0, 1, 0));

                commandList.pushConstants(
                    objectIconsPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                    sizeof(liquid::StandardPushConstants), &transformConstant);

                commandList.draw(4, 0);
              });
    });
  }

  mainLoop.setUpdateFn([&editorCamera, &animationSystem, &physicsSystem,
                        &entityManager, &aspectRatioUpdater, &sceneUpdater,
                        this](float dt) mutable {
    bool isPlaying = entityManager.isUsingSimulationContext();

    auto &entityContext = entityManager.getActiveEntityContext();

    mEventSystem.poll();
    aspectRatioUpdater.update(entityContext);
    editorCamera.update();

    if (isPlaying) {
      animationSystem.update(dt, entityContext);
    }

    entityContext.iterateEntities<liquid::SkeletonComponent>(
        [](auto entity, auto &component) { component.skeleton.update(); });

    entityContext.iterateEntities<liquid::SkeletonComponent>(
        [](auto entity, auto &component) { component.skeleton.updateDebug(); });

    sceneUpdater.update(entityContext);

    if (isPlaying) {
      physicsSystem.update(dt, entityContext);
    }
    return true;
  });

  mainLoop.setRenderFn([&renderer, &editorManager, &animationSystem,
                        &entityManager, &assetManager, &graph, &physicsSystem,
                        &ui, &debugLayer, &preloadStatusDialog, &presenter,
                        this]() {
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
