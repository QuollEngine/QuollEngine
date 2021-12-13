#include "core/Base.h"
#include "core/Engine.h"
#include "core/EngineGlobals.h"
#include "window/glfw/GLFWWindow.h"

#include "VulkanError.h"
#include "VulkanValidator.h"
#include "VulkanRenderer.h"
#include "VulkanHardwareBuffer.h"
#include "VulkanStandardPushConstants.h"
#include "VulkanPipeline.h"
#include "VulkanDeferredMaterialBinder.h"

#include "renderer/render-graph/RenderGraph.h"
#include "VulkanGraphEvaluator.h"

#include <glm/gtx/string_cast.hpp>

namespace liquid {

constexpr uint32_t SHADOWMAP_DIMENSIONS = 2048;
constexpr uint32_t NUM_LIGHTS = 16;

VulkanRenderer::VulkanRenderer(EntityContext &entityContext_,
                               GLFWWindow *window, bool enableValidations)
    : entityContext(entityContext_),
      renderBackend(window, enableValidations, statsManager),
      debugManager(new DebugManager), shaderLibrary(new ShaderLibrary) {

  descriptorManager = new VulkanDescriptorManager(
      renderBackend.getVulkanInstance().getDevice());

  deferredResourceManager =
      new VulkanDeferredResourceManager(descriptorManager);

  loadShaders();
  createImgui();
}

VulkanRenderer::~VulkanRenderer() {
  entityContext.destroyComponents<MeshComponent>();

  shadowMaterials.clear();

  if (imguiRenderer) {
    delete imguiRenderer;
    imguiRenderer = nullptr;
    LOG_DEBUG("[Vulkan] Imgui renderer destroyed");
  }

  if (shaderLibrary) {
    delete shaderLibrary;
    LOG_DEBUG("[Vulkan] Shader library destroyed");
  }

  if (deferredResourceManager) {
    delete deferredResourceManager;
    LOG_DEBUG("[Vulkan] deferred resource manager destroyed");
  }

  if (descriptorManager) {
    delete descriptorManager;
    descriptorManager = nullptr;
    LOG_DEBUG("[Vulkan] Descriptor manager destroyed");
  }
}

void VulkanRenderer::loadShaders() {
  shaderLibrary->addShader(
      "__engine.default.pbr.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/pbr.vert.spv"));
  shaderLibrary->addShader(
      "__engine.default.pbr.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/pbr.frag.spv"));
  shaderLibrary->addShader(
      "__engine.default.skybox.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/skybox.vert.spv"));
  shaderLibrary->addShader(
      "__engine.default.skybox.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/skybox.frag.spv"));
  shaderLibrary->addShader(
      "__engine.default.shadowmap.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/shadowmap.vert.spv"));
  shaderLibrary->addShader(
      "__engine.default.shadowmap.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/shadowmap.frag.spv"));
  shaderLibrary->addShader(
      "__engine.imgui.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/imgui.vert.spv"));
  shaderLibrary->addShader(
      "__engine.imgui.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/imgui.frag.spv"));
}

void VulkanRenderer::createRenderGraph(
    const SharedPtr<VulkanRenderData> &renderData) {
  if (graphCreated)
    return;

  struct ShadowPassScope {
    GraphResourceId pipeline;
    GraphResourceId shadowMap;
  };

  graph.addPass<ShadowPassScope>(
      "shadowPass",
      [this](RenderGraphBuilder &builder, ShadowPassScope &scope) {
        scope.shadowMap = builder.write(
            "shadowmap", RenderPassAttachment{AttachmentType::Depth,
                                              TextureFramebufferData{
                                                  SHADOWMAP_DIMENSIONS,
                                                  SHADOWMAP_DIMENSIONS,
                                                  VK_FORMAT_D16_UNORM,
                                                  NUM_LIGHTS,
                                              },
                                              AttachmentLoadOp::Clear,
                                              AttachmentStoreOp::Store,
                                              DepthStencilClear{1.0f, 0}});

        scope.pipeline = builder.create(PipelineDescriptor{
            shaderLibrary->getShader("__engine.default.shadowmap.vertex"),
            shaderLibrary->getShader("__engine.default.shadowmap.fragment"),
            PipelineVertexInputLayout::create<Vertex>(),
            PipelineInputAssembly{PrimitiveTopology::TriangleList},
            PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                               FrontFace::Clockwise}});
      },
      [this, &renderData](RenderCommandList &commandList,
                          ShadowPassScope &scope,
                          RenderGraphRegistry &registry) {
        const auto &pipeline = registry.getPipeline(scope.pipeline);

        const auto &vulkanPipeline =
            std::dynamic_pointer_cast<VulkanPipeline>(pipeline);

        commandList.bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);

        for (auto &shadowMaterial : shadowMaterials) {
          const auto &materialBinder =
              std::dynamic_pointer_cast<VulkanDeferredMaterialBinder>(
                  shadowMaterial->getResourceBinder());

          commandList.bindDescriptorSets(
              vulkanPipeline->getPipelineLayout(),
              VK_PIPELINE_BIND_POINT_GRAPHICS, 0,
              {materialBinder->getDescriptorSet(
                  vulkanPipeline->getDescriptorLayout(0))},
              {});

          entityContext.iterateEntities<MeshComponent, TransformComponent>(
              [&commandList, &renderData, &vulkanPipeline,
               this](Entity entity, const MeshComponent &mesh,
                     const TransformComponent &transform) {
                const auto &instance = mesh.instance;

                auto *transformConstant = new VulkanStandardPushConstants;
                transformConstant->modelMatrix = transform.transformWorld;

                commandList.pushConstants(vulkanPipeline->getPipelineLayout(),
                                          VK_SHADER_STAGE_VERTEX_BIT, 0,
                                          sizeof(VulkanStandardPushConstants),
                                          transformConstant);

                for (size_t i = 0; i < instance->getVertexBuffers().size();
                     ++i) {
                  commandList.bindVertexBuffer(
                      instance->getVertexBuffers().at(i));

                  if (instance->getIndexBuffers().at(i) != nullptr) {
                    commandList.bindIndexBuffer(
                        instance->getIndexBuffers().at(i),
                        VK_INDEX_TYPE_UINT32);
                    commandList.drawIndexed(instance->getIndexCounts().at(i), 0,
                                            0);
                    statsManager.addDrawCall(instance->getIndexCounts().at(i) /
                                             3);
                  } else {
                    commandList.draw(instance->getVertexCounts().at(i), 0);
                    statsManager.addDrawCall(instance->getVertexCounts().at(i) /
                                             3);
                  }
                }
              });
        }
      });

  struct MainPassScope {
    GraphResourceId pipeline;
    GraphResourceId shadowmapTexture;
    VkDescriptorSet sceneDescriptorSet;
    VkDescriptorSet sceneDescriptorSetFragment;
  };

  constexpr glm::vec4 blueishClearValue{0.19f, 0.21f, 0.26f, 1.0f};

  graph.addPass<MainPassScope>(
      "mainPass",
      [this, blueishClearValue](RenderGraphBuilder &builder,
                                MainPassScope &scope) {
        builder.writeSwapchain(
            "mainColor", RenderPassSwapchainAttachment{
                             AttachmentType::Color, AttachmentLoadOp::Clear,
                             AttachmentStoreOp::Store, blueishClearValue});
        builder.writeSwapchain("mainDepth", RenderPassSwapchainAttachment{
                                                AttachmentType::Depth,
                                                AttachmentLoadOp::Clear,
                                                AttachmentStoreOp::Store,
                                                DepthStencilClear{1.0f, 0}});
        scope.shadowmapTexture = builder.read("shadowmap");
        scope.pipeline = builder.create(PipelineDescriptor{
            shaderLibrary->getShader("__engine.default.pbr.vertex"),
            shaderLibrary->getShader("__engine.default.pbr.fragment"),
            PipelineVertexInputLayout::create<Vertex>(),
            PipelineInputAssembly{PrimitiveTopology::TriangleList},
            PipelineRasterizer{PolygonMode::Fill, CullMode::None,
                               FrontFace::Clockwise},
            PipelineColorBlend{{PipelineColorBlendAttachment{}}}});
      },
      [this, &renderData](auto &commandList, MainPassScope &scope,
                          RenderGraphRegistry &registry) {
        const auto &pipeline = registry.getPipeline(scope.pipeline);

        commandList.bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);

        const auto &vulkanMainPipeline =
            std::dynamic_pointer_cast<VulkanPipeline>(pipeline);

        if (!scope.sceneDescriptorSet) {
          const auto &cameraBuffer =
              std::static_pointer_cast<VulkanHardwareBuffer>(
                  renderData->getScene()
                      ->getActiveCamera()
                      ->getUniformBuffer());

          scope.sceneDescriptorSet =
              descriptorManager->createSceneDescriptorSet(
                  cameraBuffer, nullptr, nullptr, {},
                  vulkanMainPipeline->getDescriptorLayout(0));

          scope.sceneDescriptorSetFragment =
              descriptorManager->createSceneDescriptorSet(
                  cameraBuffer, renderData->getSceneBuffer(),
                  registry.getTexture(scope.shadowmapTexture), {},
                  vulkanMainPipeline->getDescriptorLayout(1));
        }

        commandList.bindDescriptorSets(vulkanMainPipeline->getPipelineLayout(),
                                       VK_PIPELINE_BIND_POINT_GRAPHICS, 0,
                                       {scope.sceneDescriptorSet}, {});

        commandList.bindDescriptorSets(vulkanMainPipeline->getPipelineLayout(),
                                       VK_PIPELINE_BIND_POINT_GRAPHICS, 1,
                                       {scope.sceneDescriptorSetFragment}, {});

        entityContext.iterateEntities<MeshComponent, TransformComponent>(
            [&commandList, &renderData, &vulkanMainPipeline,
             this](Entity entity, const MeshComponent &mesh,
                   const TransformComponent &transform) {
              const auto &instance = mesh.instance;

              auto *transformConstant = new VulkanStandardPushConstants;
              transformConstant->modelMatrix = transform.transformWorld;

              commandList.pushConstants(
                  vulkanMainPipeline->getPipelineLayout(),
                  VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                  sizeof(VulkanStandardPushConstants), transformConstant);

              for (size_t i = 0; i < instance->getVertexBuffers().size(); ++i) {
                if (instance->getMaterials().at(i)) {
                  const auto &materialBinder =
                      std::dynamic_pointer_cast<VulkanDeferredMaterialBinder>(
                          instance->getMaterials().at(i)->getResourceBinder());

                  const auto &desc = materialBinder->getDescriptorSet(
                      vulkanMainPipeline->getDescriptorLayout(2));

                  commandList.bindDescriptorSets(
                      vulkanMainPipeline->getPipelineLayout(),
                      VK_PIPELINE_BIND_POINT_GRAPHICS, 2, {desc}, {});
                }

                commandList.bindVertexBuffer(
                    instance->getVertexBuffers().at(i));

                if (instance->getIndexBuffers().at(i) != nullptr) {
                  commandList.bindIndexBuffer(instance->getIndexBuffers().at(i),
                                              VK_INDEX_TYPE_UINT32);
                  commandList.drawIndexed(instance->getIndexCounts().at(i), 0,
                                          0);
                  statsManager.addDrawCall(instance->getIndexCounts().at(i) /
                                           3);
                } else {
                  commandList.draw(instance->getVertexCounts().at(i), 0);
                  statsManager.addDrawCall(instance->getVertexCounts().at(i) /
                                           3);
                }
              }
            });
      });

  struct ImguiScope {
    GraphResourceId pipeline;
  };

  graph.addPass<ImguiScope>(
      "imgui",
      [this](RenderGraphBuilder &builder, ImguiScope &scope) {
        builder.read("mainColor");
        builder.writeSwapchain("imguiColor", RenderPassSwapchainAttachment{
                                                 AttachmentType::Color,
                                                 AttachmentLoadOp::Load,
                                                 AttachmentStoreOp::Store});

        scope.pipeline = builder.create(PipelineDescriptor{
            shaderLibrary->getShader("__engine.imgui.vertex"),
            shaderLibrary->getShader("__engine.imgui.fragment"),
            PipelineVertexInputLayout{
                {PipelineVertexInputBinding{0, sizeof(ImDrawVert),
                                            VertexInputRate::Vertex}},
                {PipelineVertexInputAttribute{0, 0, VK_FORMAT_R32G32_SFLOAT,
                                              IM_OFFSETOF(ImDrawVert, pos)},
                 PipelineVertexInputAttribute{1, 0, VK_FORMAT_R32G32_SFLOAT,
                                              IM_OFFSETOF(ImDrawVert, uv)},
                 PipelineVertexInputAttribute{2, 0, VK_FORMAT_R8G8B8A8_UNORM,
                                              IM_OFFSETOF(ImDrawVert, col)}}

            },
            PipelineInputAssembly{PrimitiveTopology::TriangleList},
            PipelineRasterizer{PolygonMode::Fill, CullMode::None,
                               FrontFace::CounterClockwise},
            PipelineColorBlend{{PipelineColorBlendAttachment{
                true, BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha,
                BlendOp::Add, BlendFactor::One, BlendFactor::OneMinusSrcAlpha,
                BlendOp::Add}}}});
      },
      [this](RenderCommandList &commandList, ImguiScope &scope,
             RenderGraphRegistry &registry) {
        auto &imguiPipeline = registry.getPipeline(scope.pipeline);
        imguiRenderer->draw(commandList, imguiPipeline);
      });

  graphCreated = true;
}

SharedPtr<VulkanShader> VulkanRenderer::createShader(const String &shaderFile) {
  return std::make_shared<VulkanShader>(
      renderBackend.getVulkanInstance().getDevice(), shaderFile);
}

SharedPtr<Material> VulkanRenderer::createMaterial(
    const SharedPtr<Shader> &vertexShader,
    const SharedPtr<Shader> &fragmentShader,
    const std::vector<SharedPtr<Texture>> &textures,
    const std::vector<std::pair<String, Property>> &properties,
    const CullMode &cullMode) {
  return std::make_shared<Material>(textures, properties,
                                    renderBackend.getResourceAllocator(),
                                    deferredResourceManager);
}

SharedPtr<Material>
VulkanRenderer::createMaterialPBR(const MaterialPBR::Properties &properties,
                                  const CullMode &cullMode) {
  return std::make_shared<MaterialPBR>(properties,
                                       renderBackend.getResourceAllocator(),
                                       deferredResourceManager);
}

void VulkanRenderer::createImgui() {
  imguiRenderer = new ImguiRenderer(renderBackend.getWindow(),
                                    renderBackend.getVulkanInstance(),
                                    renderBackend.getResourceAllocator());
}

SharedPtr<VulkanRenderData> VulkanRenderer::prepareScene(Scene *scene) {
  shadowMaterials.reserve(
      entityContext.getEntityCountForComponent<LightComponent>());

  size_t i = 0;
  entityContext.iterateEntities<LightComponent>(
      [&i, this](Entity entity, const LightComponent &light) {
        shadowMaterials.push_back(SharedPtr<Material>(new Material(
            {},
            {{"lightMatrix", glm::mat4{1.0f}},
             {"lightIndex", static_cast<int>(i)}},
            renderBackend.getResourceAllocator(), deferredResourceManager)));

        i++;
      });

  return std::make_shared<VulkanRenderData>(
      entityContext, scene, descriptorManager,
      renderBackend.getResourceAllocator(), nullptr, shadowMaterials);
}

void VulkanRenderer::draw(const SharedPtr<VulkanRenderData> &renderData) {
  statsManager.resetDrawCalls();
  createRenderGraph(renderData);

  renderBackend.execute(graph);
}

void VulkanRenderer::waitForIdle() { renderBackend.waitForIdle(); }

} // namespace liquid
