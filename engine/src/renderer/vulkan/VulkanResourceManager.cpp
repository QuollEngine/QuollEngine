#include "core/Base.h"
#include "VulkanResourceManager.h"
#include "VulkanMaterialResourceBinder.h"

namespace liquid {

VulkanResourceManager::VulkanResourceManager(
    VulkanDescriptorManager *descriptorManager_)
    : descriptorManager(descriptorManager_) {}

SharedPtr<MaterialResourceBinder>
VulkanResourceManager::createMaterialResourceBinder(Material *material) {
  return std::make_shared<VulkanMaterialResourceBinder>(material,
                                                        descriptorManager);
}

} // namespace liquid
