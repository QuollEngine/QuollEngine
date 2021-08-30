#include "ImguiRenderer.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "ImguiError.h"
#include "core/EngineGlobals.h"

namespace liquid {

ImguiRenderer::ImguiRenderer(GLFWWindow *window,
                             const VulkanContext &vulkanContext_,
                             const VulkanSwapchain &swapchain,
                             VkRenderPass renderPass,
                             const VulkanUploadContext &uploadContext)
    : vulkanContext(vulkanContext_) {
  ImGui::CreateContext();

  descriptorManager = new VulkanDescriptorManager(vulkanContext.getDevice());
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForVulkan(window->getInstance(), true);
  ImGui_ImplVulkan_InitInfo initInfo{};
  initInfo.Instance = vulkanContext.getInstance();
  initInfo.PhysicalDevice = vulkanContext.getPhysicalDevice().getVulkanDevice();
  initInfo.Device = vulkanContext.getDevice();
  initInfo.QueueFamily = vulkanContext.getPhysicalDevice()
                             .getQueueFamilyIndices()
                             .graphicsFamily.value();
  initInfo.Queue = vulkanContext.getGraphicsQueue();
  initInfo.DescriptorPool = descriptorManager->getDescriptorPool();
  initInfo.PipelineCache = nullptr;
  initInfo.Allocator = nullptr;
  initInfo.MinImageCount = 2;
  initInfo.ImageCount = static_cast<uint32_t>(swapchain.getImageViews().size());
  initInfo.CheckVkResultFn = [](VkResult res) {
    if (res != VK_SUCCESS) {
      throw ImguiError("Failed to initialize Vulkan device objects");
    }
  };

  ImGui_ImplVulkan_Init(&initInfo, renderPass);
  LOG_DEBUG("[ImGui] ImGui initialized with Vulkan backend");

  uploadFonts(uploadContext);
}

ImguiRenderer::~ImguiRenderer() {
  delete descriptorManager;

  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void ImguiRenderer::beginRendering() {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void ImguiRenderer::endRendering() { ImGui::Render(); }

void ImguiRenderer::draw(VkCommandBuffer commandBuffer) {
  auto *data = ImGui::GetDrawData();
  if (data) {
    ImGui_ImplVulkan_RenderDrawData(data, commandBuffer);
  }
}

void ImguiRenderer::uploadFonts(const VulkanUploadContext &uploadContext) {
  uploadContext.submit([](VkCommandBuffer commandBuffer) {
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
  });

  ImGui_ImplVulkan_DestroyFontUploadObjects();

  LOG_DEBUG("[ImGui] Fonts uploaded for usage");
}

} // namespace liquid
