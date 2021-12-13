#include "core/Base.h"
#include "VulkanDeferredResourceManager.h"
#include "VulkanDeferredMaterialBinder.h"

namespace liquid {

VulkanDeferredResourceManager::VulkanDeferredResourceManager(
    VulkanDescriptorManager *descriptorManager_)
    : descriptorManager(descriptorManager_) {}

SharedPtr<MaterialResourceBinder>
VulkanDeferredResourceManager::createMaterialResourceBinder(
    Material *material) {
  return std::make_shared<VulkanDeferredMaterialBinder>(material,
                                                        descriptorManager);
}

} // namespace liquid
