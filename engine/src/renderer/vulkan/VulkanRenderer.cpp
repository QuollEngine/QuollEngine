#include "core/Base.h"
#include "core/Engine.h"
#include "core/EngineGlobals.h"
#include "window/glfw/GLFWWindow.h"

#include "VulkanError.h"
#include "VulkanValidator.h"
#include "VulkanRenderer.h"
#include "VulkanHardwareBuffer.h"
#include "VulkanStandardPushConstants.h"
#include "VulkanMaterialResourceBinder.h"
#include "VulkanPipeline.h"

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

  mainResourceManager = new VulkanResourceManager(descriptorManager);

  loadShaders();
  createImgui();
  createMainPipelineLayout();
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

  if (mainPipelineLayout) {
    vkDestroyPipelineLayout(renderBackend.getVulkanInstance().getDevice(),
                            mainPipelineLayout, nullptr);
    LOG_DEBUG("[Vulkan] Main pipeline layout destroyed");
    mainPipelineLayout = VK_NULL_HANDLE;
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
            mainPipelineLayout,
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

        commandList.bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);

        for (auto &shadowMaterial : shadowMaterials) {
          const auto &materialBinder =
              std::dynamic_pointer_cast<VulkanMaterialResourceBinder>(
                  shadowMaterial->getResourceBinder());

          commandList.bindDescriptorSets(
              mainPipelineLayout, VK_PIPELINE_BIND_POINT_GRAPHICS, 1,
              {materialBinder->getDescriptorSet()}, {});

          entityContext.iterateEntities<MeshComponent, TransformComponent>(
              [&commandList, &renderData,
               this](Entity entity, const MeshComponent &mesh,
                     const TransformComponent &transform) {
                const auto &instance = mesh.instance;

                auto *transformConstant = new VulkanStandardPushConstants;
                transformConstant->modelMatrix = transform.transformWorld;

                commandList.pushConstants(
                    mainPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                    sizeof(VulkanStandardPushConstants), transformConstant);

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
    GraphResourceId imguiPipeline;
    GraphResourceId shadowmapTexture;
    VkDescriptorSet sceneDescriptorSet;
  };

  graph.addPass<MainPassScope>(
      "mainPass",
      [this](RenderGraphBuilder &builder, MainPassScope &scope) {
        builder.writeSwapchainColor();
        builder.writeSwapchainDepth();
        scope.shadowmapTexture = builder.read("shadowmap");
        scope.pipeline = builder.create(PipelineDescriptor{
            mainPipelineLayout,
            shaderLibrary->getShader("__engine.default.pbr.vertex"),
            shaderLibrary->getShader("__engine.default.pbr.fragment"),
            PipelineVertexInputLayout::create<Vertex>(),
            PipelineInputAssembly{PrimitiveTopology::TriangleList},
            PipelineRasterizer{PolygonMode::Fill, CullMode::None,
                               FrontFace::Clockwise},
            PipelineColorBlend{{PipelineColorBlendAttachment{}}}});

        scope.imguiPipeline = builder.create(PipelineDescriptor{
            imguiRenderer->getPipelineLayout(),
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
      [this, &renderData](auto &commandList, MainPassScope &scope,
                          RenderGraphRegistry &registry) {
        if (!scope.sceneDescriptorSet) {
          const auto &cameraBuffer =
              std::static_pointer_cast<VulkanHardwareBuffer>(
                  renderData->getScene()
                      ->getActiveCamera()
                      ->getUniformBuffer());

          scope.sceneDescriptorSet =
              descriptorManager->createSceneDescriptorSet(
                  cameraBuffer, renderData->getSceneBuffer(),
                  registry.getTexture(scope.shadowmapTexture), {});
        }
        const auto &pipeline = registry.getPipeline(scope.pipeline);

        commandList.bindPipeline(pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);

        commandList.bindDescriptorSets(mainPipelineLayout,
                                       VK_PIPELINE_BIND_POINT_GRAPHICS, 0,
                                       {scope.sceneDescriptorSet}, {});

        entityContext.iterateEntities<MeshComponent, TransformComponent>(
            [&commandList, &renderData,
             this](Entity entity, const MeshComponent &mesh,
                   const TransformComponent &transform) {
              const auto &instance = mesh.instance;

              auto *transformConstant = new VulkanStandardPushConstants;
              transformConstant->modelMatrix = transform.transformWorld;

              commandList.pushConstants(
                  mainPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                  sizeof(VulkanStandardPushConstants), transformConstant);

              for (size_t i = 0; i < instance->getVertexBuffers().size(); ++i) {
                if (instance->getMaterials().at(i)) {
                  const auto &materialBinder =
                      std::dynamic_pointer_cast<VulkanMaterialResourceBinder>(
                          instance->getMaterials().at(i)->getResourceBinder());

                  commandList.bindDescriptorSets(
                      mainPipelineLayout, VK_PIPELINE_BIND_POINT_GRAPHICS, 1,
                      {materialBinder->getDescriptorSet()}, {});
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

        auto &imguiPipeline = registry.getPipeline(scope.imguiPipeline);
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
                                    mainResourceManager);
}

SharedPtr<Material>
VulkanRenderer::createMaterialPBR(const MaterialPBR::Properties &properties,
                                  const CullMode &cullMode) {
  return std::make_shared<MaterialPBR>(
      properties, renderBackend.getResourceAllocator(), mainResourceManager);
}

void VulkanRenderer::setClearColor(glm::vec4 clearColor_) {
  clearColor = clearColor_;
}

void VulkanRenderer::createImgui() {
  imguiRenderer = new ImguiRenderer(renderBackend.getWindow(),
                                    renderBackend.getVulkanInstance(),
                                    renderBackend.getResourceAllocator());
}

void VulkanRenderer::createMainPipelineLayout() {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.offset = 0;
  pushConstantRange.size = sizeof(VulkanStandardPushConstants);
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  auto descriptorSetLayouts = descriptorManager->getDescriptorLayouts();
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

  checkForVulkanError(
      vkCreatePipelineLayout(renderBackend.getVulkanInstance().getDevice(),
                             &pipelineLayoutInfo, nullptr, &mainPipelineLayout),
      "Failed to create main pipeline layout");
  LOG_DEBUG("[Vulkan] Main pipeline layout created");
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
            renderBackend.getResourceAllocator(), mainResourceManager)));

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
