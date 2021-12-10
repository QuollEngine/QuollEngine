#include "core/Base.h"
#include "VulkanMaterialResourceBinder.h"
#include "VulkanShader.h"
#include "VulkanHardwareBuffer.h"

namespace liquid {

VulkanMaterialResourceBinder::VulkanMaterialResourceBinder(
    Material *material, VulkanDescriptorManager *descriptorManager_)
    : descriptorManager(descriptorManager_) {
  materialDescriptorSet = descriptorManager->createMaterialDescriptorSet(
      std::dynamic_pointer_cast<VulkanHardwareBuffer>(
          material->getUniformBuffer()),
      material->getTextures());
}

VulkanMaterialResourceBinder::~VulkanMaterialResourceBinder() {
  if (materialDescriptorSet) {
    vkFreeDescriptorSets(descriptorManager->getDevice(),
                         descriptorManager->getDescriptorPool(), 1,
                         &materialDescriptorSet);
  }
}

} // namespace liquid
