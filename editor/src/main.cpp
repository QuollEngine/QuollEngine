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

  liquid::AssetManager assetManager(std::filesystem::current_path());
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

  liquid::MainLoop mainLoop(window, fpsCounter);
  liquidator::GLTFImporter gltfImporter(assetManager, renderer.getRegistry());
  liquidator::EditorCamera editorCamera(entityContext, renderer, window);
  liquidator::EditorGrid editorGrid(renderer.getRegistry());
  liquidator::SceneManager sceneManager(entityContext, editorCamera,
                                        editorGrid);

  liquidator::UIRoot ui(entityContext, gltfImporter);

  while (sceneManager.hasNewScene()) {
    sceneManager.createNewScene();

    debugLayer.getAssetBrowser().setOnLoadToScene([&entityContext, &renderer,
                                                   &sceneManager,
                                                   &animationSystem,
                                                   &assetManager](
                                                      liquid::AssetType type,
                                                      uint32_t handle) {
      if (type != liquid::AssetType::Mesh &&
          type != liquid::AssetType::SkinnedMesh &&
          type != liquid::AssetType::Prefab) {
        return;
      }

      constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
      const auto &invViewMatrix = glm::inverse(
          sceneManager.getActiveScene()->getActiveCamera()->getViewMatrix());
      const auto &orientation = invViewMatrix * glm::translate(distanceFromEye);

      liquid::TransformComponent transform;
      transform.localPosition = orientation[3];

      auto entity = entityContext.createEntity();
      entityContext.setComponent<liquid::DebugComponent>(entity, {});
      auto *parent = sceneManager.getActiveScene()->getRootNode()->addChild(
          entity, transform);

      if (type == liquid::AssetType::Prefab) {
        auto &asset = assetManager.getRegistry().getPrefabs().getAsset(
            static_cast<liquid::PrefabAssetHandle>(handle));
        entityContext.setComponent<liquid::NameComponent>(entity, {asset.name});

        std::map<uint32_t, liquid::Entity> entityMap;

        auto getOrCreateEntity = [&entityMap, &entityContext, &parent,
                                  &transform](uint32_t localId) mutable {
          if (entityMap.find(localId) == entityMap.end()) {
            auto entity = entityContext.createEntity();
            entityMap.insert_or_assign(localId, entity);
            entityContext.setComponent<liquid::DebugComponent>(entity, {});

            parent->addChild(entity);
          }

          return entityMap.at(localId);
        };

        for (auto &item : asset.data.meshes) {
          auto entity = getOrCreateEntity(item.entity);
          entityContext.setComponent<liquid::MeshComponent>(
              entity, {renderer.createMeshInstance(
                          item.value, assetManager.getRegistry())});
        }

        for (auto &item : asset.data.skinnedMeshes) {
          auto entity = getOrCreateEntity(item.entity);
          entityContext.setComponent<liquid::SkinnedMeshComponent>(
              entity, {renderer.createMeshInstance(
                          item.value, assetManager.getRegistry())});
        }

        for (auto &item : asset.data.skeletons) {
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

      if (type == liquid::AssetType::Mesh) {
        entityContext.setComponent<liquid::MeshComponent>(
            entity, {renderer.createMeshInstance(
                        static_cast<liquid::MeshAssetHandle>(handle),
                        assetManager.getRegistry())});
      } else if (type == liquid::AssetType::SkinnedMesh) {
        auto skinnedMeshHandle =
            static_cast<liquid::SkinnedMeshAssetHandle>(handle);
        entityContext.setComponent<liquid::SkinnedMeshComponent>(
            entity, {renderer.createMeshInstance(skinnedMeshHandle,
                                                 assetManager.getRegistry())});
        auto skeletonHandle = assetManager.getRegistry()
                                  .getSkinnedMeshes()
                                  .getAsset(skinnedMeshHandle)
                                  .data.skeleton;
        if (skeletonHandle != liquid::SkeletonAssetHandle::Invalid) {
          const auto &skeleton = assetManager.getRegistry()
                                     .getSkeletons()
                                     .getAsset(skeletonHandle)
                                     .data;
          liquid::Skeleton skeletonInstance(
              skeleton.jointLocalPositions, skeleton.jointLocalRotations,
              skeleton.jointLocalScales, skeleton.jointParents,
              skeleton.jointInverseBindMatrices, skeleton.jointNames,
              &renderer.getRegistry());

          entityContext.setComponent<liquid::SkeletonComponent>(
              entity, {std::move(skeletonInstance)});

          entityContext.getComponent<liquid::SkeletonComponent>(entity)
              .skeleton.update();
        }
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

      pass.addPipeline(editorGridPipeline);
      pass.addPipeline(skeletonLinesPipeline);

      pass.setExecutor([editorGridPipeline, skeletonLinesPipeline, &renderer,
                        &cameraObj, &editorCamera, &editorGrid, &entityContext](
                           liquid::rhi::RenderCommandList &commandList) {
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
                          &debugLayer]() {
      auto &imgui = renderer.getImguiRenderer();

      imgui.beginRendering();
      ui.render(sceneManager, assetManager.getRegistry(), physicsSystem);

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

      debugLayer.render();
      imgui.endRendering();

      return renderer.render(graph.first);
    });

    mainLoop.run();
  }

  renderer.wait();
  return 0;
}
