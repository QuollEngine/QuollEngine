#include "core/Base.h"
#include "VulkanDeferredMaterialBinder.h"

#include "VulkanHardwareBuffer.h"

namespace liquid {

VulkanDeferredMaterialBinder::VulkanDeferredMaterialBinder(
    Material *material_, VulkanDescriptorManager *descriptorManager_)
    : descriptorManager(descriptorManager_), material(material_) {}

VulkanDeferredMaterialBinder::~VulkanDeferredMaterialBinder() {
  if (materialDescriptorSet) {
    vkFreeDescriptorSets(descriptorManager->getDevice(),
                         descriptorManager->getDescriptorPool(), 1,
                         &materialDescriptorSet);
  }
}

VkDescriptorSet
VulkanDeferredMaterialBinder::getDescriptorSet(VkDescriptorSetLayout layout) {
  if (!materialDescriptorSet) {
    materialDescriptorSet = descriptorManager->createMaterialDescriptorSet(
        std::dynamic_pointer_cast<VulkanHardwareBuffer>(
            material->getUniformBuffer()),
        material->getTextures(), layout);
  }

  return materialDescriptorSet;
}

} // namespace liquid
