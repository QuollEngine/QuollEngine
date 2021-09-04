#include "core/Base.h"
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
                               GLFWWindow *window_, bool enableValidations_)
    : entityContext(entityContext_), window(window_),
      context(window_, enableValidations_), statsManager(new StatsManager),
      debugManager(new DebugManager), shaderLibrary(new ShaderLibrary) {

  descriptorManager = new VulkanDescriptorManager(context.getDevice());

  createAllocator();
  createSwapchain();
  createPipelineBuilder();

  createMainPass();
  createShadowPass();

  renderContext.create(context);
  uploadContext.create(context);

  createImgui();

  resizeHandler = window->addResizeHandler(
      [this](uint32_t x, uint32_t y) mutable { framebufferResized = true; });
}

VulkanRenderer::~VulkanRenderer() {
  entityContext.destroyComponents<MeshComponent>();

  destroyMainPass();
  destroyShadowPass();

  uploadContext.destroy();
  renderContext.destroy();
  swapchain.destroy();

  if (imguiRenderer) {
    delete imguiRenderer;
    imguiRenderer = nullptr;
  }

  if (allocator) {
    vmaDestroyAllocator(allocator);
    LOG_DEBUG("[Vulkan] Allocator destroyed");
  }

  if (resourceAllocator) {
    delete resourceAllocator;
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

  window->removeResizeHandler(resizeHandler);

  window = nullptr;
}

void VulkanRenderer::recreateSwapchain() {
  waitForIdle();
  destroyMainFramebuffers();
  createSwapchain();
  createMainFramebuffers();

  LOG_DEBUG("[Vulkan] Swapchain recreated");
}

void VulkanRenderer::createMainPass() {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = swapchain.getSurfaceFormat().format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentDescription depthAttachment{};
  depthAttachment.flags = 0;
  depthAttachment.format = swapchain.getDepthFormat();
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef{};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  std::array<VkAttachmentDescription, 2> attachments{colorAttachment,
                                                     depthAttachment};

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  checkForVulkanError(vkCreateRenderPass(context.getDevice(), &renderPassInfo,
                                         nullptr, &mainRenderPass),
                      "Failed to create main render pass");

  LOG_DEBUG("[Vulkan] Main render pass created");

  mainResourceManager = new VulkanResourceManager(
      descriptorManager, pipelineBuilder, mainRenderPass, 0);

  createMainFramebuffers();
}

void VulkanRenderer::destroyMainFramebuffers() {
  for (auto &x : mainFramebuffers) {
    vkDestroyFramebuffer(context.getDevice(), x, nullptr);
  }
  mainFramebuffers.clear();
  LOG_DEBUG("[Vulkan] Main framebuffers destroyed");
}

void VulkanRenderer::destroyMainPass() {
  if (mainResourceManager) {
    delete mainResourceManager;
    mainResourceManager = nullptr;
  }

  destroyMainFramebuffers();

  if (mainRenderPass) {
    vkDestroyRenderPass(context.getDevice(), mainRenderPass, nullptr);
    mainRenderPass = nullptr;
    LOG_DEBUG("[Vulkan] Main render Pass destroyed");
  }
}

void VulkanRenderer::createShadowPass() {
  shadowPass = std::make_shared<VulkanShadowPass>(
      SHADOWMAP_DIMENSIONS, context.getDevice(), allocator, pipelineBuilder,
      resourceAllocator, descriptorManager, statsManager);
}

void VulkanRenderer::destroyShadowPass() {
  shadowPass = nullptr;
  shadowMaterials.clear();
}

void VulkanRenderer::createAllocator() {
  VmaAllocatorCreateInfo createInfo{};
  createInfo.instance = context.getInstance();
  createInfo.physicalDevice = context.getPhysicalDevice().getVulkanDevice();
  createInfo.device = context.getDevice();

  checkForVulkanError(vmaCreateAllocator(&createInfo, &allocator),
                      "Failed to create allocator");

  LOG_DEBUG("[Vulkan] Allocator created");

  resourceAllocator = new VulkanResourceAllocator(
      uploadContext, allocator, context.getDevice(), statsManager);
}

void VulkanRenderer::createSwapchain() {
  swapchain = VulkanSwapchain(window, context, allocator);
}

void VulkanRenderer::createPipelineBuilder() {
  pipelineBuilder =
      new VulkanPipelineBuilder(context.getDevice(), descriptorManager);
}

SharedPtr<VulkanShader> VulkanRenderer::createShader(const String &shaderFile) {
  return std::make_shared<VulkanShader>(context.getDevice(), shaderFile);
}

SharedPtr<Material> VulkanRenderer::createMaterial(
    const SharedPtr<Shader> &vertexShader,
    const SharedPtr<Shader> &fragmentShader,
    const std::vector<SharedPtr<Texture>> &textures,
    const std::vector<std::pair<String, Property>> &properties,
    const CullMode &cullMode) {
  return std::make_shared<Material>(vertexShader, fragmentShader, textures,
                                    properties, cullMode, resourceAllocator,
                                    mainResourceManager);
}

SharedPtr<Material>
VulkanRenderer::createMaterialPBR(const MaterialPBR::Properties &properties,
                                  const CullMode &cullMode) {
  return std::make_shared<MaterialPBR>(properties, shaderLibrary, cullMode,
                                       resourceAllocator, mainResourceManager);
}

void VulkanRenderer::createMainFramebuffers() {
  mainFramebuffers.resize(swapchain.getImageViews().size());

  for (size_t i = 0; i < swapchain.getImageViews().size(); i++) {
    std::array<VkImageView, 2> attachments{swapchain.getImageViews()[i],
                                           swapchain.getDepthImageView()};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = mainRenderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapchain.getExtent().width;
    framebufferInfo.height = swapchain.getExtent().height;
    framebufferInfo.layers = 1;

    checkForVulkanError(vkCreateFramebuffer(context.getDevice(),
                                            &framebufferInfo, nullptr,
                                            &mainFramebuffers[i]),
                        "Failed to create framebuffer");
  }

  LOG_DEBUG("[Vulkan] Framebuffers created");
}

void VulkanRenderer::setClearColor(glm::vec4 clearColor_) {
  clearColor = clearColor_;
}

void VulkanRenderer::createImgui() {
  imguiRenderer =
      new ImguiRenderer(window, context, swapchain, mainRenderPass,
                        uploadContext, shaderLibrary, resourceAllocator);
}

void VulkanRenderer::setViewportAndScissor(VkCommandBuffer commandBuffer,
                                           VkExtent2D extent) {
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
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
            CullMode::Front, resourceAllocator,
            shadowPass->getResourceManager().get())));

        i++;
      });

  return std::make_shared<VulkanRenderData>(
      entityContext, scene, descriptorManager, resourceAllocator,
      shadowPass->getShadowmap(), shadowMaterials);
}

void VulkanRenderer::drawRenderables(VkCommandBuffer commandBuffer,
                                     Camera *camera, bool useForShadowMapping) {
  entityContext.iterateEntities<MeshComponent, TransformComponent>(
      [commandBuffer, camera, useForShadowMapping,
       this](Entity entity, const MeshComponent &mesh,
             const TransformComponent &transform) {
        const auto &instance = mesh.instance;

        if (useForShadowMapping &&
            entityContext.hasComponent<EnvironmentComponent>(entity)) {
          return;
        }

        // 5. Bind object transforms
        VulkanStandardPushConstants pushContants{};
        pushContants.modelMatrix = transform.transformWorld;

        vkCmdPushConstants(commandBuffer, pipelineBuilder->getPipelineLayout(),
                           VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(VulkanStandardPushConstants), &pushContants);

        for (size_t i = 0; i < instance->getVertexBuffers().size(); ++i) {
          if (instance->getMaterials().at(i) && !useForShadowMapping) {
            const auto &materialBinder =
                std::dynamic_pointer_cast<VulkanMaterialResourceBinder>(
                    instance->getMaterials().at(i)->getResourceBinder());

            VkPipeline graphicsPipeline = materialBinder->getGraphicsPipeline(
                debugManager->getWireframeMode());

            // 2. Bind pipeline
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              graphicsPipeline);

            vkCmdBindDescriptorSets(
                commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineBuilder->getPipelineLayout(), 1, 1,
                materialBinder->getDescriptorSet(), 0, nullptr);
          }

          auto *vertexBuffer = dynamic_cast<VulkanHardwareBuffer *>(
              instance->getVertexBuffers().at(i));
          auto *indexBuffer = dynamic_cast<VulkanHardwareBuffer *>(
              instance->getIndexBuffers().at(i));

          // 5. Draw object
          VkDeviceSize offset = 0;
          std::array<VkBuffer, 1> vulkanVertexBuffers{
              vertexBuffer->getBuffer()};

          vkCmdBindVertexBuffers(
              commandBuffer, 0,
              static_cast<uint32_t>(vulkanVertexBuffers.size()),
              vulkanVertexBuffers.data(), &offset);
          vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0,
                               VK_INDEX_TYPE_UINT32);

          vkCmdDrawIndexed(commandBuffer,
                           static_cast<uint32_t>(indexBuffer->getItemSize()), 1,
                           0, 0, 0);
          if (statsManager) {
            statsManager->addDrawCall(indexBuffer->getItemSize() / 3);
          }
        }
      });
}

void VulkanRenderer::draw(const SharedPtr<VulkanRenderData> &renderData) {
  if (statsManager) {
    statsManager->resetDrawCalls();
  }

  uint32_t imageIdx =
      swapchain.acquireNextImage(renderContext.getImageAvailableSemaphore());
  if (imageIdx == std::numeric_limits<uint32_t>::max()) {
    recreateSwapchain();
    return;
  }

  renderContext.render([this, &renderData,
                        imageIdx](VkCommandBuffer commandBuffer) {
    {
      shadowPass->render(
          commandBuffer, [this, &renderData](VkCommandBuffer commandBuffer) {
            setViewportAndScissor(commandBuffer, shadowPass->getExtent());

            for (auto &shadowMaterial : shadowMaterials) {
              const auto &materialBinder =
                  std::dynamic_pointer_cast<VulkanMaterialResourceBinder>(
                      shadowMaterial->getResourceBinder());

              VkPipeline graphicsPipeline = materialBinder->getGraphicsPipeline(
                  debugManager->getWireframeMode());

              vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                graphicsPipeline);

              vkCmdBindDescriptorSets(
                  commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                  pipelineBuilder->getPipelineLayout(), 1, 1,
                  materialBinder->getDescriptorSet(), 0, nullptr);

              auto *scene = renderData->getScene();
              drawRenderables(commandBuffer, scene->getActiveCamera(), true);
            }
          });
    }

    {
      setViewportAndScissor(commandBuffer, swapchain.getExtent());

      VkClearValue clearValue;
      clearValue.color = {clearColor[0], clearColor[1], clearColor[2],
                          clearColor[3]};
      VkClearValue depthClear;
      depthClear.depthStencil.depth = 1.0f;

      std::array<VkClearValue, 2> clearValues{clearValue, depthClear};

      VkRenderPassBeginInfo renderPassInfo{};
      renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      renderPassInfo.renderPass = mainRenderPass;
      renderPassInfo.framebuffer = mainFramebuffers[imageIdx];
      renderPassInfo.renderArea.offset = {0, 0};
      renderPassInfo.renderArea.extent = swapchain.getExtent();
      renderPassInfo.clearValueCount =
          static_cast<uint32_t>(clearValues.size());
      renderPassInfo.pClearValues = clearValues.data();

      vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                           VK_SUBPASS_CONTENTS_INLINE);

      vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                              pipelineBuilder->getPipelineLayout(), 0, 1,
                              renderData->getSceneDescriptorSet(), 0, nullptr);

      auto *scene = renderData->getScene();
      drawRenderables(commandBuffer, scene->getActiveCamera(), false);

      imguiRenderer->draw(commandBuffer);

      vkCmdEndRenderPass(commandBuffer);
    }
  });

  auto queuePresentResult = renderContext.present(swapchain, imageIdx);

  if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR ||
      queuePresentResult == VK_SUBOPTIMAL_KHR || framebufferResized) {
    recreateSwapchain();
    framebufferResized = false;
  }
}

void VulkanRenderer::waitForIdle() { vkDeviceWaitIdle(context.getDevice()); }

} // namespace liquid
