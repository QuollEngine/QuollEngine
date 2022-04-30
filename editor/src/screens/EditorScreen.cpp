#include "liquid/core/Base.h"
#include "EditorScreen.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/scene/MeshInstance.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/renderer/StandardPushConstants.h"
#include "liquid/profiler/ImguiDebugLayer.h"

#include "liquid/physics/PhysicsSystem.h"
#include "liquid/loop/MainLoop.h"
#include "liquid/imgui/ImguiUtils.h"

#include "../editor-scene/EditorCamera.h"
#include "../editor-scene/SceneManager.h"
#include "../editor-scene/EditorGrid.h"
#include "../ui/UIRoot.h"
#include "../ui/AssetLoadStatusDialog.h"

void randomSpawn(liquid::EntityContext &entityContext,
                 liquid::Renderer &renderer,
                 liquidator::SceneManager &sceneManager,
                 liquid::AnimationSystem &animationSystem,
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
      auto parent = sceneManager.getEntityManager().spawnAsset(
          sceneManager.getEditorCamera(),
          sceneManager.getActiveScene()->getRootNode(),
          static_cast<uint32_t>(handle), liquid::AssetType::Prefab);
      entityContext.getComponent<liquid::TransformComponent>(parent)
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
  liquid::EntityContext entityContext;
  liquid::DebugManager debugManager;
  liquid::FPSCounter fpsCounter;

  auto layoutPath = (project.settingsPath / "layout.ini").string();
  auto statePath = project.settingsPath / "state.lqpath";

  liquid::AssetManager assetManager(project.assetsPath);
  liquid::Renderer renderer(entityContext, assetManager.getRegistry(), mWindow,
                            mDevice);

  auto res = assetManager.preloadAssets(renderer.getRegistry());
  liquidator::AssetLoadStatusDialog preloadStatusDialog("Loaded with warnings");
  preloadStatusDialog.setMessages(res.getWarnings());

  renderer.getImguiRenderer().useConfigPath(layoutPath);

  if (res.hasWarnings()) {
    preloadStatusDialog.show();
  }

  liquidator::EditorCamera editorCamera(entityContext, mEventSystem, renderer,
                                        mWindow);
  liquidator::EditorGrid editorGrid(renderer.getRegistry());
  liquidator::EntityManager entityManager(entityContext, assetManager, renderer,
                                          project.scenePath);
  liquidator::SceneManager sceneManager(entityContext, editorCamera, editorGrid,
                                        entityManager);

  sceneManager.loadOrCreateScene();
  sceneManager.loadEditorState(statePath);

  liquid::MainLoop mainLoop(mWindow, fpsCounter);
  liquidator::GLTFImporter gltfImporter(assetManager, renderer.getRegistry());

  liquid::AnimationSystem animationSystem(entityContext,
                                          assetManager.getRegistry());
  liquid::PhysicsSystem physicsSystem(entityContext, mEventSystem);

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

  liquidator::UIRoot ui(entityContext, entityManager, gltfImporter);
  ui.getIconRegistry().loadIcons(renderer.getRegistry(),
                                 std::filesystem::current_path() / "assets" /
                                     "icons");

  const auto &cameraObj =
      entityContext
          .getComponent<liquid::CameraComponent>(editorCamera.getCamera())
          .camera;

  auto graph = renderer.createRenderGraph(false);

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
                      &objectIconsPipeline, &renderer, &cameraObj,
                      &editorCamera, &editorGrid, &entityContext,
                      &ui](liquid::rhi::RenderCommandList &commandList) {
      liquid::rhi::Descriptor sceneDescriptor;
      sceneDescriptor.bind(0, cameraObj->getBuffer(),
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

      entityContext.iterateEntities<liquid::TransformComponent,
                                    liquid::SkeletonComponent,
                                    liquid::DebugComponent>(
          [&commandList, &skeletonLinesPipeline,
           &cameraObj](auto entity, auto &transform,
                       const liquid::SkeletonComponent &skeleton,
                       const liquid::DebugComponent &debug) {
            if (!debug.showBones)
              return;

            liquid::rhi::Descriptor sceneDescriptor;
            sceneDescriptor.bind(0, cameraObj->getBuffer(),
                                 liquid::rhi::DescriptorType::UniformBuffer);

            liquid::rhi::Descriptor skeletonDescriptor;
            skeletonDescriptor.bind(0, skeleton.skeleton.getDebugBuffer(),
                                    liquid::rhi::DescriptorType::UniformBuffer);

            liquid::StandardPushConstants transformConstant{};
            transformConstant.modelMatrix = transform.worldTransform;

            commandList.bindDescriptor(skeletonLinesPipeline, 0,
                                       sceneDescriptor);
            commandList.bindDescriptor(skeletonLinesPipeline, 1,
                                       skeletonDescriptor);

            commandList.pushConstants(
                skeletonLinesPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                sizeof(liquid::StandardPushConstants), &transformConstant);

            commandList.draw(skeleton.skeleton.getNumDebugBones(), 0);
          });

      entityContext.iterateEntities<
          liquid::TransformComponent,
          liquid::LightComponent>([&objectIconsPipeline, &commandList,
                                   &cameraObj, &ui, &entityContext](
                                      auto entity, const auto &transform,
                                      const auto &light) {
        commandList.bindPipeline(objectIconsPipeline);

        liquid::rhi::Descriptor sceneDescriptor;
        sceneDescriptor.bind(0, cameraObj->getBuffer(),
                             liquid::rhi::DescriptorType::UniformBuffer);

        commandList.bindDescriptor(objectIconsPipeline, 0, sceneDescriptor);

        liquid::rhi::Descriptor sunDescriptor;
        sunDescriptor.bind(
            0, {ui.getIconRegistry().getIcon(liquidator::EditorIcon::Sun)},
            liquid::rhi::DescriptorType::CombinedImageSampler);
        commandList.bindDescriptor(objectIconsPipeline, 1, sunDescriptor);

        liquid::StandardPushConstants transformConstant{};
        transformConstant.modelMatrix = transform.worldTransform;

        commandList.pushConstants(
            objectIconsPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
            sizeof(liquid::StandardPushConstants), &transformConstant);

        commandList.draw(4, 0);

        if (entityContext.hasComponent<liquid::DebugComponent>(entity) &&
            entityContext.getComponent<liquid::DebugComponent>(entity)
                .showDirection) {
          liquid::rhi::Descriptor directionDescriptor;
          directionDescriptor.bind(
              0,
              {ui.getIconRegistry().getIcon(liquidator::EditorIcon::Direction)},
              liquid::rhi::DescriptorType::CombinedImageSampler);
          commandList.bindDescriptor(objectIconsPipeline, 1,
                                     directionDescriptor);

          liquid::StandardPushConstants pcDirection{};
          static constexpr glm::vec3 LIGHT_DIR_ICON_POSITION{0.0f, 2.0f, 0.0f};
          pcDirection.modelMatrix =
              glm::translate(transform.worldTransform, LIGHT_DIR_ICON_POSITION);

          commandList.pushConstants(
              objectIconsPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
              sizeof(liquid::StandardPushConstants), &pcDirection);

          commandList.draw(4, 0);
        }
      });
    });
  }

  mainLoop.setUpdateFn([&editorCamera, &sceneManager, &animationSystem,
                        &physicsSystem, &entityContext,
                        this](double dt) mutable {
    mEventSystem.poll();
    editorCamera.update();

    animationSystem.update(static_cast<float>(dt));

    entityContext.iterateEntities<liquid::SkeletonComponent>(
        [](auto entity, auto &component) { component.skeleton.update(); });

    entityContext.iterateEntities<liquid::SkeletonComponent>(
        [](auto entity, auto &component) { component.skeleton.updateDebug(); });

    sceneManager.getActiveScene()->update();
    physicsSystem.update(static_cast<float>(dt));
    return true;
  });

  mainLoop.setRenderFn([&renderer, &sceneManager, &animationSystem,
                        &entityContext, &assetManager, &graph, &physicsSystem,
                        &ui, &debugLayer, &preloadStatusDialog]() {
    auto &imgui = renderer.getImguiRenderer();

    imgui.beginRendering();
    ui.render(sceneManager, renderer, assetManager, physicsSystem);

    if (ImGui::Begin("View")) {
      const auto &size = ImGui::GetContentRegionAvail();
      const auto &pos = ImGui::GetWindowPos();
      sceneManager.getEditorCamera().setViewport(pos.x, pos.y, size.x, size.y);
      liquid::imgui::image(graph.second.mainColor, size);
      ImGui::End();
    }

    // if (ImGui::Begin("Benchmark")) {

    //   if (ImGui::Button("Spawn prefabs")) {
    //     randomSpawn(entityContext, renderer, sceneManager,
    //     animationSystem,
    //                 assetManager);
    //   }

    //   ImGui::End();
    // }

    preloadStatusDialog.render();
    debugLayer.render();
    imgui.endRendering();

    return renderer.render(graph.first,
                           sceneManager.getEditorCamera().getCamera());
  });

  mainLoop.run();
  sceneManager.saveEditorState(statePath);
}

} // namespace liquidator
