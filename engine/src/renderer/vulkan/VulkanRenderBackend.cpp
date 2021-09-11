#include "VulkanRenderBackend.h"
#include "VulkanError.h"
#include "core/EngineGlobals.h"

namespace liquid {

VulkanRenderBackend::VulkanRenderBackend(GLFWWindow *window_,
                                         bool enableValidations_,
                                         StatsManager &statsManager)
    : window(window_), vulkanInstance(window_, enableValidations_) {

  renderContext.create(vulkanInstance);
  uploadContext.create(vulkanInstance);

  resourceAllocator = VulkanResourceAllocator::create(
      vulkanInstance, uploadContext, statsManager);

  createSwapchain();

  createSwapchainPass();
  createSwapchainFramebuffers();

  resizeHandler = window->addResizeHandler(
      [this](uint32_t x, uint32_t y) mutable { framebufferResized = true; });
}

VulkanRenderBackend::~VulkanRenderBackend() {
  uploadContext.destroy();
  renderContext.destroy();

  destroySwapchainPass();
  destroySwapchainFramebuffers();

  window->removeResizeHandler(resizeHandler);

  swapchain.destroy();

  if (resourceAllocator) {
    delete resourceAllocator;
  }

  window = nullptr;
}

void VulkanRenderBackend::waitForIdle() {
  vkDeviceWaitIdle(vulkanInstance.getDevice());
}

void VulkanRenderBackend::createSwapchain() {
  swapchain = VulkanSwapchain(window, vulkanInstance,
                              resourceAllocator->getVmaAllocator());

  LOG_DEBUG("[Vulkan] Swapchain created");
}

void VulkanRenderBackend::recreateSwapchain() {
  waitForIdle();
  destroySwapchainPass();
  destroySwapchainFramebuffers();
  createSwapchain();
  createSwapchainPass();
  createSwapchainFramebuffers();

  framebufferResized = false;

  LOG_DEBUG("[Vulkan] Swapchain recreated");
}

void VulkanRenderBackend::createSwapchainPass() {
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

  checkForVulkanError(vkCreateRenderPass(vulkanInstance.getDevice(),
                                         &renderPassInfo, nullptr,
                                         &swapchainPass),
                      "Failed to create main render pass");

  LOG_DEBUG("[Vulkan] Main render pass created");
}

void VulkanRenderBackend::createSwapchainFramebuffers() {
  swapchainFramebuffers.resize(swapchain.getImageViews().size());

  for (size_t i = 0; i < swapchain.getImageViews().size(); i++) {
    std::array<VkImageView, 2> attachments{swapchain.getImageViews()[i],
                                           swapchain.getDepthImageView()};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = swapchainPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = swapchain.getExtent().width;
    framebufferInfo.height = swapchain.getExtent().height;
    framebufferInfo.layers = 1;

    checkForVulkanError(vkCreateFramebuffer(vulkanInstance.getDevice(),
                                            &framebufferInfo, nullptr,
                                            &swapchainFramebuffers[i]),
                        "Failed to create framebuffer");
  }

  LOG_DEBUG("[Vulkan] Framebuffers created");
}

void VulkanRenderBackend::destroySwapchainPass() {
  if (swapchainPass) {
    vkDestroyRenderPass(vulkanInstance.getDevice(), swapchainPass, nullptr);
    swapchainPass = nullptr;
    LOG_DEBUG("[Vulkan] Swapchain render Pass destroyed");
  }
}

void VulkanRenderBackend::destroySwapchainFramebuffers() {
  for (auto &framebuffer : swapchainFramebuffers) {
    vkDestroyFramebuffer(vulkanInstance.getDevice(), framebuffer, nullptr);
  }
  swapchainFramebuffers.clear();
  LOG_DEBUG("[Vulkan] Main framebuffers destroyed");
}

} // namespace liquid
