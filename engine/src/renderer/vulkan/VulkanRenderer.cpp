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

#include <glm/gtx/string_cast.hpp>

namespace liquid {

constexpr uint32_t SHADOWMAP_DIMENSIONS = 2048;

VulkanRenderer::VulkanRenderer(EntityContext &entityContext_,
                               GLFWWindow *window, bool enableValidations)
    : entityContext(entityContext_), renderBackend(window, enableValidations),
      statsManager(new StatsManager), debugManager(new DebugManager),
      shaderLibrary(new ShaderLibrary) {

  descriptorManager = new VulkanDescriptorManager(
      renderBackend.getVulkanInstance().getDevice());

  createPipelineBuilder();

  mainResourceManager = new VulkanResourceManager(
      descriptorManager, pipelineBuilder, renderBackend.getSwapchainPass(), 0);

  loadShaders();

  createShadowPass();

  createImgui();
}

VulkanRenderer::~VulkanRenderer() {
  entityContext.destroyComponents<MeshComponent>();

  destroyShadowPass();

  if (imguiRenderer) {
    delete imguiRenderer;
    imguiRenderer = nullptr;
  }

  if (shaderLibrary) {
    delete shaderLibrary;
  }

  if (pipelineBuilder) {
    delete pipelineBuilder;
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

void VulkanRenderer::createShadowPass() {
  shadowPass = std::make_shared<VulkanShadowPass>(
      SHADOWMAP_DIMENSIONS, renderBackend.getVulkanInstance().getDevice(),
      pipelineBuilder, renderBackend.getResourceAllocator(), descriptorManager,
      statsManager);
}

void VulkanRenderer::destroyShadowPass() {
  shadowPass = nullptr;
  shadowMaterials.clear();
}

void VulkanRenderer::createPipelineBuilder() {
  pipelineBuilder = new VulkanPipelineBuilder(
      renderBackend.getVulkanInstance().getDevice(), descriptorManager);
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
  return std::make_shared<Material>(
      vertexShader, fragmentShader, textures, properties, cullMode,
      renderBackend.getResourceAllocator(), mainResourceManager);
}

SharedPtr<Material>
VulkanRenderer::createMaterialPBR(const MaterialPBR::Properties &properties,
                                  const CullMode &cullMode) {
  return std::make_shared<MaterialPBR>(properties, shaderLibrary, cullMode,
                                       renderBackend.getResourceAllocator(),
                                       mainResourceManager);
}

void VulkanRenderer::setClearColor(glm::vec4 clearColor_) {
  clearColor = clearColor_;
}

void VulkanRenderer::createImgui() {
  imguiRenderer = new ImguiRenderer(
      renderBackend.getWindow(), renderBackend.getVulkanInstance(),
      renderBackend.getSwapchainPass(), shaderLibrary,
      renderBackend.getResourceAllocator());
}

void VulkanRenderer::setViewportAndScissor(RenderCommandList &commandList,
                                           glm::vec2 extent) {
  commandList.setViewport({0.0f, 0.0f}, extent, {0.0f, 1.0f});
  commandList.setScissor({0.0f, 0.0f}, extent);
}

SharedPtr<VulkanRenderData> VulkanRenderer::prepareScene(Scene *scene) {
  shadowMaterials.reserve(
      entityContext.getEntityCountForComponent<LightComponent>());

  size_t i = 0;
  entityContext.iterateEntities<LightComponent>(
      [&i, this](Entity entity, const LightComponent &light) {
        shadowMaterials.push_back(SharedPtr<Material>(new Material(
            shaderLibrary->getShader("__engine.default.shadowmap.vertex"),
            shaderLibrary->getShader("__engine.default.shadowmap.fragment"), {},
            {{"lightMatrix", glm::mat4{1.0f}},
             {"lightIndex", static_cast<int>(i)}},
            CullMode::Front, renderBackend.getResourceAllocator(),
            shadowPass->getResourceManager().get())));

        i++;
      });

  return std::make_shared<VulkanRenderData>(
      entityContext, scene, descriptorManager,
      renderBackend.getResourceAllocator(), shadowPass->getShadowmap(),
      shadowMaterials);
}

void VulkanRenderer::drawRenderables(RenderCommandList &commandList,
                                     Camera *camera, bool useForShadowMapping) {
  entityContext.iterateEntities<MeshComponent, TransformComponent>(
      [&commandList, camera, useForShadowMapping,
       this](Entity entity, const MeshComponent &mesh,
             const TransformComponent &transform) {
        const auto &instance = mesh.instance;

        if (useForShadowMapping &&
            entityContext.hasComponent<EnvironmentComponent>(entity)) {
          return;
        }

        auto *transformConstant = new VulkanStandardPushConstants;
        transformConstant->modelMatrix = transform.transformWorld;

        commandList.pushConstants(
            pipelineBuilder->getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0,
            sizeof(VulkanStandardPushConstants), transformConstant);

        for (size_t i = 0; i < instance->getVertexBuffers().size(); ++i) {
          if (instance->getMaterials().at(i) && !useForShadowMapping) {
            const auto &materialBinder =
                std::dynamic_pointer_cast<VulkanMaterialResourceBinder>(
                    instance->getMaterials().at(i)->getResourceBinder());

            VkPipeline graphicsPipeline = materialBinder->getGraphicsPipeline(
                debugManager->getWireframeMode());

            commandList.bindPipeline(graphicsPipeline,
                                     VK_PIPELINE_BIND_POINT_GRAPHICS);

            commandList.bindDescriptorSets(pipelineBuilder->getPipelineLayout(),
                                           VK_PIPELINE_BIND_POINT_GRAPHICS, 1,
                                           {materialBinder->getDescriptorSet()},
                                           {});
          }

          commandList.bindVertexBuffer(instance->getVertexBuffers().at(i));
          commandList.bindIndexBuffer(instance->getIndexBuffers().at(i),
                                      VK_INDEX_TYPE_UINT32);
          commandList.drawIndexed(
              instance->getIndexBuffers().at(i)->getItemSize(), 0, 0);

          if (statsManager) {
            statsManager->addDrawCall(
                instance->getIndexBuffers().at(i)->getItemSize() / 3);
          }
        }
      });
}

void VulkanRenderer::draw(const SharedPtr<VulkanRenderData> &renderData) {
  if (statsManager) {
    statsManager->resetDrawCalls();
  }

  uint32_t imageIdx = renderBackend.getSwapchain().acquireNextImage(
      renderBackend.getRenderContext().getImageAvailableSemaphore());
  if (imageIdx == std::numeric_limits<uint32_t>::max()) {
    renderBackend.recreateSwapchain();
    return;
  }

  RenderCommandList commandList;

  shadowPass->render(commandList, [this, &renderData,
                                   imageIdx](RenderCommandList &commandList) {
    setViewportAndScissor(commandList, {shadowPass->getExtent().width,
                                        shadowPass->getExtent().height});
    for (auto &shadowMaterial : shadowMaterials) {
      const auto &materialBinder =
          std::dynamic_pointer_cast<VulkanMaterialResourceBinder>(
              shadowMaterial->getResourceBinder());

      VkPipeline graphicsPipeline =
          materialBinder->getGraphicsPipeline(debugManager->getWireframeMode());

      commandList.bindPipeline(graphicsPipeline,
                               VK_PIPELINE_BIND_POINT_GRAPHICS);
      commandList.bindDescriptorSets(pipelineBuilder->getPipelineLayout(),
                                     VK_PIPELINE_BIND_POINT_GRAPHICS, 1,
                                     {materialBinder->getDescriptorSet()}, {});

      auto *scene = renderData->getScene();
      drawRenderables(commandList, scene->getActiveCamera(), true);
    }
  });

  {
    setViewportAndScissor(commandList,
                          {renderBackend.getSwapchain().getExtent().width,
                           renderBackend.getSwapchain().getExtent().height});

    VkClearValue clearColorValue;
    clearColorValue.color.float32[0] = clearColor.x;
    clearColorValue.color.float32[1] = clearColor.y;
    clearColorValue.color.float32[2] = clearColor.z;
    clearColorValue.color.float32[3] = clearColor.w;

    VkClearValue clearDepthValue;
    clearDepthValue.depthStencil.depth = 1.0f;
    clearDepthValue.depthStencil.stencil = 0;

    commandList.beginRenderPass(
        renderBackend.getSwapchainPass(),
        renderBackend.getSwapchainFramebuffers().at(imageIdx), {0, 0},
        {renderBackend.getSwapchain().getExtent().width,
         renderBackend.getSwapchain().getExtent().height},
        {clearColorValue, clearDepthValue});

    commandList.bindDescriptorSets(pipelineBuilder->getPipelineLayout(),
                                   VK_PIPELINE_BIND_POINT_GRAPHICS, 0,
                                   {renderData->getSceneDescriptorSet()}, {});

    auto *scene = renderData->getScene();
    drawRenderables(commandList, scene->getActiveCamera(), false);

    imguiRenderer->draw(commandList);

    commandList.endRenderPass();
  }

  renderBackend.getRenderContext().render(commandList);

  auto queuePresentResult = renderBackend.getRenderContext().present(
      renderBackend.getSwapchain(), imageIdx);

  if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
      queuePresentResult == VK_SUBOPTIMAL_KHR ||
      renderBackend.isFramebufferResized()) {
    renderBackend.recreateSwapchain();
  }
}

void VulkanRenderer::waitForIdle() { renderBackend.waitForIdle(); }

} // namespace liquid
