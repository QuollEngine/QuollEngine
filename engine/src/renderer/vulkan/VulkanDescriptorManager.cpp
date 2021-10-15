#include "VulkanDescriptorManager.h"
#include "VulkanError.h"
#include "VulkanTextureBinder.h"
#include "core/EngineGlobals.h"

namespace liquid {

constexpr uint32_t MAX_TEXTURE_DESCRIPTORS = 8;

VulkanDescriptorManager::VulkanDescriptorManager(VkDevice device_)
    : device(device_) {
  createSceneDescriptorLayout();
  createMaterialDescriptorLayout();
  createDescriptorPool();
}

VulkanDescriptorManager::~VulkanDescriptorManager() {
  if (sceneLayout) {
    vkDestroyDescriptorSetLayout(device, sceneLayout, nullptr);
    sceneLayout = VK_NULL_HANDLE;
    LOG_DEBUG("[Vulkan] Scene descriptor set layout destroyed");
  }

  if (materialLayout) {
    vkDestroyDescriptorSetLayout(device, materialLayout, nullptr);
    materialLayout = VK_NULL_HANDLE;
    LOG_DEBUG("[Vulkan] Material descriptor set layout destroyed");
  }

  if (descriptorPool) {
    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    LOG_DEBUG("[Vulkan] Descriptor pool destroyed");
  }
}

VkDescriptorSet VulkanDescriptorManager::createSceneDescriptorSet(
    const SharedPtr<VulkanHardwareBuffer> &cameraBuffer,
    const SharedPtr<VulkanHardwareBuffer> &sceneBuffer,
    const SharedPtr<Texture> &shadowmaps,
    const std::array<SharedPtr<Texture>, 3> &iblMaps) {
  VkDescriptorSet sceneDescriptorSet = VK_NULL_HANDLE;

  VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
  descriptorSetAllocateInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptorSetAllocateInfo.pNext = nullptr;
  descriptorSetAllocateInfo.descriptorPool = descriptorPool;
  descriptorSetAllocateInfo.pSetLayouts = &sceneLayout;
  descriptorSetAllocateInfo.descriptorSetCount = 1;

  checkForVulkanError(vkAllocateDescriptorSets(device,
                                               &descriptorSetAllocateInfo,
                                               &sceneDescriptorSet),
                      "Failed to allocate scene descriptor set");
  LOG_DEBUG("[Vulkan] Scene descriptor set allocated");

  std::vector<VkWriteDescriptorSet> writes;

  // Camera Buffer Write
  VkDescriptorBufferInfo cameraBufferInfo{};
  cameraBufferInfo.buffer = cameraBuffer->getBuffer();
  cameraBufferInfo.offset = 0;
  cameraBufferInfo.range = cameraBuffer->getBufferSize();

  VkWriteDescriptorSet setCameraBufferWrite{};
  setCameraBufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  setCameraBufferWrite.pNext = nullptr;
  setCameraBufferWrite.dstBinding = 0;
  setCameraBufferWrite.dstSet = sceneDescriptorSet;
  setCameraBufferWrite.descriptorCount = 1;
  setCameraBufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  setCameraBufferWrite.pBufferInfo = &cameraBufferInfo;

  writes.push_back(setCameraBufferWrite);

  // Scene Buffer Write
  VkDescriptorBufferInfo sceneBufferInfo{};
  sceneBufferInfo.buffer = sceneBuffer->getBuffer();
  sceneBufferInfo.offset = 0;
  sceneBufferInfo.range = sceneBuffer->getBufferSize();

  VkWriteDescriptorSet setSceneBufferWrite{};
  setSceneBufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  setSceneBufferWrite.pNext = nullptr;
  setSceneBufferWrite.dstBinding = 1;
  setSceneBufferWrite.dstSet = sceneDescriptorSet;
  setSceneBufferWrite.descriptorCount = 1;
  setSceneBufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  setSceneBufferWrite.pBufferInfo = &sceneBufferInfo;

  writes.push_back(setSceneBufferWrite);

  const auto &vulkanShadowmap = std::dynamic_pointer_cast<VulkanTextureBinder>(
      shadowmaps->getResourceBinder());

  VkDescriptorImageInfo shadowmapInfo{};
  shadowmapInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
  shadowmapInfo.sampler = vulkanShadowmap->getSampler();
  shadowmapInfo.imageView = vulkanShadowmap->getImageView();

  VkWriteDescriptorSet setShadowmapWrite{};
  setShadowmapWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  setShadowmapWrite.pNext = nullptr;
  setShadowmapWrite.dstBinding = 2;
  setShadowmapWrite.dstSet = sceneDescriptorSet;
  setShadowmapWrite.descriptorCount = 1;
  setShadowmapWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  setShadowmapWrite.pImageInfo = &shadowmapInfo;

  writes.push_back(setShadowmapWrite);

  std::array<VkDescriptorImageInfo, 2> iblInfos{};
  VkDescriptorImageInfo lutInfo{};
  if (iblMaps.at(0) && iblMaps.at(1) && iblMaps.at(2)) {
    for (size_t i = 0; i < 2; ++i) {
      const auto &vulkanBinder = std::dynamic_pointer_cast<VulkanTextureBinder>(
          iblMaps.at(i)->getResourceBinder());

      iblInfos.at(i) = {};
      iblInfos.at(i).imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      iblInfos.at(i).sampler = vulkanBinder->getSampler();
      iblInfos.at(i).imageView = vulkanBinder->getImageView();
    }

    VkWriteDescriptorSet setIblWrite{};
    setIblWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setIblWrite.pNext = nullptr;
    setIblWrite.dstBinding = 3;
    setIblWrite.dstSet = sceneDescriptorSet;
    setIblWrite.descriptorCount = iblInfos.size();
    setIblWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setIblWrite.pImageInfo = iblInfos.data();

    writes.push_back(setIblWrite);

    const auto &vulkanBinder = std::dynamic_pointer_cast<VulkanTextureBinder>(
        iblMaps.at(2)->getResourceBinder());
    lutInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    lutInfo.sampler = vulkanBinder->getSampler();
    lutInfo.imageView = vulkanBinder->getImageView();

    VkWriteDescriptorSet setLutWrite{};
    setLutWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setLutWrite.pNext = nullptr;
    setLutWrite.dstBinding = 4;
    setLutWrite.dstSet = sceneDescriptorSet;
    setLutWrite.descriptorCount = 1;
    setLutWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setLutWrite.pImageInfo = &lutInfo;

    writes.push_back(setLutWrite);
  }

  vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()),
                         writes.data(), 0, nullptr);
  return sceneDescriptorSet;
}

VkDescriptorSet VulkanDescriptorManager::createMaterialDescriptorSet(
    const SharedPtr<VulkanHardwareBuffer> &buffer,
    const std::vector<SharedPtr<Texture>> &textures) {

  if (textures.size() > MAX_TEXTURE_DESCRIPTORS) {
    throw VulkanError("Cannot create material descriptor set with more than " +
                          std::to_string(MAX_TEXTURE_DESCRIPTORS) +
                          " texture descriptors",
                      VK_SUCCESS);
  }

  VkDescriptorSet materialDescriptorSet = VK_NULL_HANDLE;

  VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
  descriptorSetAllocateInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptorSetAllocateInfo.pNext = nullptr;
  descriptorSetAllocateInfo.descriptorPool = descriptorPool;
  descriptorSetAllocateInfo.pSetLayouts = &materialLayout;
  descriptorSetAllocateInfo.descriptorSetCount = 1;

  checkForVulkanError(vkAllocateDescriptorSets(device,
                                               &descriptorSetAllocateInfo,
                                               &materialDescriptorSet),
                      "Failed to allocate material descriptor set");
  LOG_DEBUG("[Vulkan] Material descriptor set allocated");

  std::vector<VkWriteDescriptorSet> writes;

  if (buffer) {
    VkDescriptorBufferInfo materialBufferInfo{};
    materialBufferInfo.buffer = buffer->getBuffer();
    materialBufferInfo.offset = 0;
    materialBufferInfo.range = buffer->getBufferSize();

    VkWriteDescriptorSet setBufferWrite{};
    setBufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setBufferWrite.pNext = nullptr;
    setBufferWrite.dstBinding = 0;
    setBufferWrite.dstSet = materialDescriptorSet;
    setBufferWrite.descriptorCount = 1;
    setBufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setBufferWrite.pBufferInfo = &materialBufferInfo;

    writes.push_back(setBufferWrite);
  }

  std::vector<VkDescriptorImageInfo> materialTextureInfos;
  if (!textures.empty()) {
    materialTextureInfos.resize(textures.size());
    for (size_t i = 0; i < textures.size(); ++i) {
      const auto &vulkanBinder = std::dynamic_pointer_cast<VulkanTextureBinder>(
          textures[i]->getResourceBinder());
      materialTextureInfos[i] = {};
      materialTextureInfos[i].sampler = vulkanBinder->getSampler();
      materialTextureInfos[i].imageView = vulkanBinder->getImageView();
      materialTextureInfos[i].imageLayout =
          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    VkWriteDescriptorSet setTexturesWrite{};
    setTexturesWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    setTexturesWrite.pNext = nullptr;
    setTexturesWrite.dstBinding = 1;
    setTexturesWrite.dstArrayElement = 0;
    setTexturesWrite.dstSet = materialDescriptorSet;
    setTexturesWrite.descriptorCount =
        static_cast<uint32_t>(materialTextureInfos.size());
    setTexturesWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    setTexturesWrite.pImageInfo = materialTextureInfos.data();

    writes.push_back(setTexturesWrite);
  }

  vkUpdateDescriptorSets(device, static_cast<uint32_t>(writes.size()),
                         writes.data(), 0, nullptr);

  return materialDescriptorSet;
}

void VulkanDescriptorManager::createSceneDescriptorLayout() {
  VkDescriptorSetLayoutBinding cameraBinding{};
  cameraBinding.binding = 0;
  cameraBinding.descriptorCount = 1;
  cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  cameraBinding.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding sceneBinding{};
  sceneBinding.binding = 1;
  sceneBinding.descriptorCount = 1;
  sceneBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  sceneBinding.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding shadowmapBinding{};
  shadowmapBinding.binding = 2;
  shadowmapBinding.descriptorCount = 1;
  shadowmapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  shadowmapBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding iblBinding{};
  iblBinding.binding = 3;
  iblBinding.descriptorCount = 2;
  iblBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  iblBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding lutBinding{};
  lutBinding.binding = 4;
  lutBinding.descriptorCount = 1;
  lutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  lutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  std::array<VkDescriptorBindingFlags, 5> bindingFlags{
      0, 0, 0, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT,
      VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT};

  VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsCreateInfo{};
  bindingFlagsCreateInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
  bindingFlagsCreateInfo.pNext = nullptr;
  bindingFlagsCreateInfo.pBindingFlags = bindingFlags.data();
  bindingFlagsCreateInfo.bindingCount = bindingFlags.size();

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  std::array<VkDescriptorSetLayoutBinding, 5> bindings{
      cameraBinding, sceneBinding, shadowmapBinding, iblBinding, lutBinding};

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.pNext = &bindingFlagsCreateInfo;
  descriptorSetLayoutInfo.flags = 0;
  descriptorSetLayoutInfo.bindingCount = bindings.size();
  descriptorSetLayoutInfo.pBindings = bindings.data();

  checkForVulkanError(vkCreateDescriptorSetLayout(device,
                                                  &descriptorSetLayoutInfo,
                                                  nullptr, &sceneLayout),
                      "Failed to create scene descriptor set layout");

  LOG_DEBUG("[Vulkan] Scene descriptor set layout created");
}

void VulkanDescriptorManager::createMaterialDescriptorLayout() {
  VkDescriptorSetLayoutBinding bufferBinding{};
  bufferBinding.binding = 0;
  bufferBinding.descriptorCount = 1;
  bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  bufferBinding.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding textureBinding{};
  textureBinding.binding = 1;
  textureBinding.descriptorCount = MAX_TEXTURE_DESCRIPTORS;
  textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings{bufferBinding,
                                                       textureBinding};

  std::array<VkDescriptorBindingFlags, 2> bindingFlags{
      0, VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT};

  VkDescriptorSetLayoutBindingFlagsCreateInfoEXT bindingFlagsCreateInfo{};
  bindingFlagsCreateInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
  bindingFlagsCreateInfo.pNext = nullptr;
  bindingFlagsCreateInfo.pBindingFlags = bindingFlags.data();
  bindingFlagsCreateInfo.bindingCount = bindingFlags.size();

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
  descriptorSetLayoutInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  descriptorSetLayoutInfo.pNext = &bindingFlagsCreateInfo;
  descriptorSetLayoutInfo.flags = 0;
  descriptorSetLayoutInfo.bindingCount = bindings.size();
  descriptorSetLayoutInfo.pBindings = bindings.data();
  checkForVulkanError(vkCreateDescriptorSetLayout(device,
                                                  &descriptorSetLayoutInfo,
                                                  nullptr, &materialLayout),
                      "Failed to create material descriptor set layout");

  LOG_DEBUG("[Vulkan] Material descriptor set layout created");
}

void VulkanDescriptorManager::createDescriptorPool() {
  const uint32_t NUM_UNIFORM_BUFFERS = 500;
  const uint32_t NUM_SAMPLERS = 100;
  const uint32_t NUM_DESCRIPTORS = 100;
  std::array<VkDescriptorPoolSize, 2> poolSizes{
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                           NUM_UNIFORM_BUFFERS},
      VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                           MAX_TEXTURE_DESCRIPTORS * NUM_SAMPLERS}};

  VkDescriptorPoolCreateInfo descriptorPoolInfo{};
  descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptorPoolInfo.pNext = nullptr;
  descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  descriptorPoolInfo.maxSets = NUM_DESCRIPTORS;
  descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  descriptorPoolInfo.pPoolSizes = poolSizes.data();

  checkForVulkanError(vkCreateDescriptorPool(device, &descriptorPoolInfo,
                                             nullptr, &descriptorPool),
                      "Failed to create descriptor pool");

  LOG_DEBUG("[Vulkan] Descriptor pool created");
}

} // namespace liquid
