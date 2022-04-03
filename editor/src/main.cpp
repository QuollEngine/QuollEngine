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
      std::filesystem::path("../../../engine/bin/Debug/assets").string());
  liquid::EntityContext entityContext;
  liquid::Window window("Liquidator", INITIAL_WIDTH, INITIAL_HEIGHT);

  liquid::rhi::VulkanRenderBackend backend(window);
  liquid::DebugManager debugManager;
  liquid::FPSCounter fpsCounter;

  auto *device = backend.createDefaultDevice();

  liquid::AssetRegistry assetRegistry;
  liquid::EventSystem eventSystem;
  liquid::Renderer renderer(entityContext, window, device);
  liquid::AnimationSystem animationSystem(entityContext);
  liquid::PhysicsSystem physicsSystem(entityContext, eventSystem);

  liquid::ImguiDebugLayer debugLayer(
      device->getDeviceInformation(), device->getDeviceStats(),
      renderer.getRegistry(), assetRegistry, fpsCounter, debugManager);

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
  liquid::GLTFLoader loader(entityContext, renderer, animationSystem, true);
  liquidator::GLTFImporter gltfImporter(assetRegistry, renderer.getRegistry());
  liquidator::EditorCamera editorCamera(entityContext, renderer, window);
  liquidator::EditorGrid editorGrid(renderer.getRegistry());
  liquidator::SceneManager sceneManager(entityContext, editorCamera,
                                        editorGrid);

  liquidator::UIRoot ui(entityContext, loader, gltfImporter);

  while (sceneManager.hasNewScene()) {
    sceneManager.createNewScene();

    debugLayer.getAssetBrowser().setOnLoadToScene([&entityContext, &renderer,
                                                   &sceneManager,
                                                   &assetRegistry](
                                                      liquid::AssetType type,
                                                      uint32_t handle) {
      if (type != liquid::AssetType::Mesh &&
          type != liquid::AssetType::SkinnedMesh) {
        return;
      }

      constexpr glm::vec3 distanceFromEye = {0.0f, 0.0f, -10.0f};
      const auto &invViewMatrix = glm::inverse(
          sceneManager.getActiveScene()->getActiveCamera()->getViewMatrix());
      const auto &orientation = invViewMatrix * glm::translate(distanceFromEye);

      liquid::TransformComponent transform;
      transform.localPosition = orientation[3];

      auto entity = entityContext.createEntity();
      sceneManager.getActiveScene()->getRootNode()->addChild(entity, transform);
      entityContext.setComponent<liquid::DebugComponent>(entity, {});

      if (type == liquid::AssetType::Mesh) {
        entityContext.setComponent<liquid::MeshComponent>(
            entity,
            {renderer.createMeshInstance(
                static_cast<liquid::MeshAssetHandle>(handle), assetRegistry)});
      } else if (type == liquid::AssetType::SkinnedMesh) {
        auto skinnedMeshHandle =
            static_cast<liquid::SkinnedMeshAssetHandle>(handle);
        entityContext.setComponent<liquid::SkinnedMeshComponent>(
            entity,
            {renderer.createMeshInstance(skinnedMeshHandle, assetRegistry)});
        auto skeletonHandle = assetRegistry.getSkinnedMeshes()
                                  .getAsset(skinnedMeshHandle)
                                  .data.skeleton;
        if (skeletonHandle != liquid::SkeletonAssetHandle::Invalid) {
          const auto &skeleton =
              assetRegistry.getSkeletons().getAsset(skeletonHandle).data;
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
          [](auto entity, auto &component) {
            component.skeleton.updateDebug();
          });

      sceneManager.getActiveScene()->update();
      physicsSystem.update(static_cast<float>(dt));
      return !sceneManager.hasNewScene();
    });

    mainLoop.setRenderFn([&renderer, &graph, &ui, &sceneManager,
                          &animationSystem, &physicsSystem, &debugLayer]() {
      auto &imgui = renderer.getImguiRenderer();

      imgui.beginRendering();
      ui.render(sceneManager, animationSystem, physicsSystem);

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
