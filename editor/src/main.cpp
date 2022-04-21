#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/renderer/Material.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/scene/Vertex.h"
#include "liquid/scene/Mesh.h"
#include "liquid/scene/MeshInstance.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/events/EventSystem.h"
#include "liquid/window/Window.h"
#include "liquid/renderer/StandardPushConstants.h"
#include "liquid/profiler/ImguiDebugLayer.h"

#include "liquid/rhi/vulkan/VulkanRenderBackend.h"

#include "liquid/loaders/GLTFLoader.h"
#include "liquid/loaders/ImageTextureLoader.h"

#include "liquid/physics/PhysicsSystem.h"

#include "liquid/loop/MainLoop.h"

#include "editor-scene/EditorCamera.h"
#include "editor-scene/SceneManager.h"
#include "editor-scene/EditorGrid.h"
#include "ui/UIRoot.h"
#include "ui/AssetLoadStatusDialog.h"

static const uint32_t INITIAL_WIDTH = 1024;
static const uint32_t INITIAL_HEIGHT = 768;

int main() {
  liquid::Engine::setAssetsPath(
      std::filesystem::path("./engine/assets").string());
  liquid::EntityContext entityContext;
  liquid::Window window("Liquidator", INITIAL_WIDTH, INITIAL_HEIGHT);

  liquid::rhi::VulkanRenderBackend backend(window);
  liquid::DebugManager debugManager;
  liquid::FPSCounter fpsCounter;

  auto *device = backend.createDefaultDevice();

  auto tmpProjectPath = std::filesystem::current_path() / "tmp-project";
  if (!std::filesystem::exists(tmpProjectPath)) {
    std::filesystem::create_directory(tmpProjectPath);
  }

  liquid::AssetManager assetManager(tmpProjectPath);

  auto res = assetManager.preloadAssets();
  liquidator::AssetLoadStatusDialog preloadStatusDialog("Loaded with warnings");
  preloadStatusDialog.setMessages(res.getWarnings());

  if (res.hasWarnings()) {
    preloadStatusDialog.show();
  }

  liquid::EventSystem eventSystem;
  liquid::Renderer renderer(entityContext, window, device);
  liquid::AnimationSystem animationSystem(entityContext,
                                          assetManager.getRegistry());
  liquid::PhysicsSystem physicsSystem(entityContext, eventSystem);

  liquid::ImguiDebugLayer debugLayer(
      device->getDeviceInformation(), device->getDeviceStats(),
      renderer.getRegistry(), assetManager.getRegistry(), fpsCounter,
      debugManager);

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

  liquid::MainLoop mainLoop(window, fpsCounter);
  liquidator::GLTFImporter gltfImporter(assetManager, renderer.getRegistry());
  liquidator::EditorCamera editorCamera(entityContext, renderer, window);
  liquidator::EditorGrid editorGrid(renderer.getRegistry());
  liquidator::SceneManager sceneManager(entityContext, editorCamera,
                                        editorGrid);

  liquidator::UIRoot ui(entityContext, gltfImporter);
  ui.getIconRegistry().loadIcons(renderer.getRegistry(),
                                 std::filesystem::current_path() / "assets" /
                                     "icons");

  while (sceneManager.hasNewScene()) {
    sceneManager.createNewScene();

    ui.getAssetBrowser().setOnItemOpenHandler([&entityContext, &renderer,
                                               &sceneManager, &animationSystem,
                                               &assetManager](
                                                  liquid::AssetType type,
                                                  uint32_t handle) {
      if (type != liquid::AssetType::Prefab) {
        return;
      }

      constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
      const auto &invViewMatrix = glm::inverse(
          sceneManager.getActiveScene()->getActiveCamera()->getViewMatrix());
      const auto &orientation = invViewMatrix * glm::translate(distanceFromEye);

      liquid::TransformComponent parentTransform;
      parentTransform.localPosition = orientation[3];

      auto parentEntity = entityContext.createEntity();
      entityContext.setComponent<liquid::DebugComponent>(parentEntity, {});
      auto *parent = sceneManager.getActiveScene()->getRootNode()->addChild(
          parentEntity, parentTransform);

      if (type == liquid::AssetType::Prefab) {
        auto &asset = assetManager.getRegistry().getPrefabs().getAsset(
            static_cast<liquid::PrefabAssetHandle>(handle));
        entityContext.setComponent<liquid::NameComponent>(parentEntity,
                                                          {asset.name});

        std::map<uint32_t, liquid::Entity> entityMap;

        auto getOrCreateEntity =
            [&entityMap, &entityContext, &parent](
                uint32_t localId,
                const liquid::TransformComponent &transform = {}) mutable {
              if (entityMap.find(localId) == entityMap.end()) {
                auto entity = entityContext.createEntity();
                entityMap.insert_or_assign(localId, entity);
                entityContext.setComponent<liquid::DebugComponent>(entity, {});

                parent->addChild(entity, transform);
              }

              return entityMap.at(localId);
            };

        for (auto &item : asset.data.transforms) {
          liquid::TransformComponent transform{};
          transform.localPosition = item.value.position;
          transform.localRotation = item.value.rotation;
          transform.localScale = item.value.scale;
          getOrCreateEntity(item.entity, transform);
        }

        for (auto &item : asset.data.transforms) {
          auto &transform =
              entityContext.getComponent<liquid::TransformComponent>(
                  entityMap.at(item.entity));
          if (item.value.parent >= 0) {
            transform.parent = getOrCreateEntity(item.value.parent);
          }
        }

        for (auto &item : asset.data.meshes) {
          if (!assetManager.getRegistry().getMeshes().hasAsset(item.value)) {
            continue;
          }

          auto entity = getOrCreateEntity(item.entity);
          entityContext.setComponent<liquid::MeshComponent>(
              entity, {renderer.createMeshInstance(
                          item.value, assetManager.getRegistry())});
        }

        for (auto &item : asset.data.skinnedMeshes) {
          if (!assetManager.getRegistry().getSkinnedMeshes().hasAsset(
                  item.value)) {
            continue;
          }

          auto entity = getOrCreateEntity(item.entity);
          entityContext.setComponent<liquid::SkinnedMeshComponent>(
              entity, {renderer.createMeshInstance(
                          item.value, assetManager.getRegistry())});
        }

        for (auto &item : asset.data.skeletons) {
          if (!assetManager.getRegistry().getSkeletons().hasAsset(item.value)) {
            continue;
          }

          auto entity = getOrCreateEntity(item.entity);

          const auto &skeleton = assetManager.getRegistry()
                                     .getSkeletons()
                                     .getAsset(item.value)
                                     .data;
          liquid::Skeleton skeletonInstance(
              skeleton.jointLocalPositions, skeleton.jointLocalRotations,
              skeleton.jointLocalScales, skeleton.jointParents,
              skeleton.jointInverseBindMatrices, skeleton.jointNames,
              &renderer.getRegistry());

          entityContext.setComponent<liquid::SkeletonComponent>(
              entity, {std::move(skeletonInstance)});
        }

        for (auto &item : asset.data.animators) {
          auto entity = getOrCreateEntity(item.entity);

          entityContext.setComponent(entity, item.value);
        }

        return;
      }
    });

    const auto &cameraObj =
        entityContext
            .getComponent<liquid::CameraComponent>(editorCamera.getCamera())
            .camera;

    const auto &renderData =
        renderer.prepareScene(sceneManager.getActiveScene());

    auto graph = renderer.createRenderGraph(renderData, false);

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
                   liquid::rhi::BlendFactor::DstAlpha,
                   liquid::rhi::BlendOp::Add,
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
                   liquid::rhi::BlendFactor::DstAlpha,
                   liquid::rhi::BlendOp::Add,
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
                   liquid::rhi::BlendFactor::DstAlpha,
                   liquid::rhi::BlendOp::Add,
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

        entityContext
            .iterateEntities<liquid::TransformComponent,
                             liquid::SkeletonComponent, liquid::DebugComponent>(
                [&commandList, &skeletonLinesPipeline,
                 &cameraObj](auto entity, auto &transform,
                             const liquid::SkeletonComponent &skeleton,
                             const liquid::DebugComponent &debug) {
                  if (!debug.showBones)
                    return;

                  liquid::rhi::Descriptor sceneDescriptor;
                  sceneDescriptor.bind(
                      0, cameraObj->getBuffer(),
                      liquid::rhi::DescriptorType::UniformBuffer);

                  liquid::rhi::Descriptor skeletonDescriptor;
                  skeletonDescriptor.bind(
                      0, skeleton.skeleton.getDebugBuffer(),
                      liquid::rhi::DescriptorType::UniformBuffer);

                  auto *transformConstant = new liquid::StandardPushConstants;
                  transformConstant->modelMatrix = transform.worldTransform;

                  commandList.bindDescriptor(skeletonLinesPipeline, 0,
                                             sceneDescriptor);
                  commandList.bindDescriptor(skeletonLinesPipeline, 1,
                                             skeletonDescriptor);

                  commandList.pushConstants(
                      skeletonLinesPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                      sizeof(liquid::StandardPushConstants), transformConstant);

                  commandList.draw(skeleton.skeleton.getNumDebugBones(), 0);
                });

        entityContext.iterateEntities<liquid::TransformComponent,
                                      liquid::LightComponent>(
            [&objectIconsPipeline, &commandList, &cameraObj, &ui,
             &entityContext](auto entity, const auto &transform,
                             const auto &light) {
              commandList.bindPipeline(objectIconsPipeline);

              liquid::rhi::Descriptor sceneDescriptor;
              sceneDescriptor.bind(0, cameraObj->getBuffer(),
                                   liquid::rhi::DescriptorType::UniformBuffer);

              commandList.bindDescriptor(objectIconsPipeline, 0,
                                         sceneDescriptor);

              liquid::rhi::Descriptor sunDescriptor;
              sunDescriptor.bind(
                  0,
                  {ui.getIconRegistry().getIcon(liquidator::EditorIcon::Sun)},
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
                    {ui.getIconRegistry().getIcon(
                        liquidator::EditorIcon::Direction)},
                    liquid::rhi::DescriptorType::CombinedImageSampler);
                commandList.bindDescriptor(objectIconsPipeline, 1,
                                           directionDescriptor);

                liquid::StandardPushConstants pcDirection{};
                static constexpr glm::vec3 LIGHT_DIR_ICON_POSITION{0.0f, 2.0f,
                                                                   0.0f};
                pcDirection.modelMatrix = glm::translate(
                    transform.worldTransform, LIGHT_DIR_ICON_POSITION);

                commandList.pushConstants(
                    objectIconsPipeline, VK_SHADER_STAGE_VERTEX_BIT, 0,
                    sizeof(liquid::StandardPushConstants), &pcDirection);

                commandList.draw(4, 0);
              }
            });
      });
    }

    mainLoop.setUpdateFn([&editorCamera, &sceneManager, &renderData,
                          &animationSystem, &physicsSystem, &entityContext,
                          &eventSystem](double dt) mutable {
      eventSystem.poll();
      editorCamera.update();

      animationSystem.update(static_cast<float>(dt));
      renderData->update();

      entityContext.iterateEntities<liquid::SkeletonComponent>(
          [](auto entity, auto &component) { component.skeleton.update(); });

      entityContext.iterateEntities<liquid::SkeletonComponent>(
          [](auto entity, auto &component) {
            component.skeleton.updateDebug();
          });

      sceneManager.getActiveScene()->update();
      physicsSystem.update(static_cast<float>(dt));
      return !sceneManager.hasNewScene();
    });

    mainLoop.setRenderFn([&renderer, &sceneManager, &animationSystem,
                          &assetManager, &graph, &physicsSystem, &ui,
                          &debugLayer, &preloadStatusDialog]() {
      auto &imgui = renderer.getImguiRenderer();

      imgui.beginRendering();
      ui.render(sceneManager, assetManager, physicsSystem);

      if (ImGui::Begin("View")) {
        const auto &size = ImGui::GetContentRegionAvail();
        const auto &pos = ImGui::GetWindowPos();
        sceneManager.getEditorCamera().setViewport(pos.x, pos.y, size.x,
                                                   size.y);
        ImGui::Image(reinterpret_cast<void *>(
                         static_cast<uintptr_t>(graph.second.mainColor)),
                     size);
        ImGui::End();
      }

      preloadStatusDialog.render();
      debugLayer.render();
      imgui.endRendering();

      return renderer.render(graph.first);
    });

    mainLoop.run();
  }

  renderer.wait();
  return 0;
}
