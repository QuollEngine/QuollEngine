#pragma once

#include "VulkanDescriptorManager.h"
#include "renderer/HardwareBuffer.h"
#include "renderer/ResourceAllocator.h"
#include "renderer/Material.h"
#include "scene/Scene.h"
#include "entity/EntityContext.h"

namespace liquid {

class VulkanRenderData {
public:
  /**
   * @brief Creates render data
   *
   * @param entityContext Entity context
   * @param scene Scene
   * @param descriptorManager Descriptor manager
   * @param resourceAllocator Resource allocator
   * @param shadowmaps Shadow maps
   * @param shadowMaterials Shadow materials
   */
  VulkanRenderData(EntityContext &entityContext, Scene *scene,
                   VulkanDescriptorManager *manager,
                   ResourceAllocator *resourceAllocator,
                   const SharedPtr<Texture> &shadowmaps,
                   const std::vector<SharedPtr<Material>> &shadowMaterials);

  /**
   * @brief Gets scene descriptor set
   *
   * @return Scene descriptor set
   */
  inline VkDescriptorSet *getSceneDescriptorSet() {
    return &sceneDescriptorSet;
  }

  /**
   * @brief Get scene
   *
   * @return Scene
   */
  inline Scene *getScene() { return scene; }

  /**
   * @brief Update scene data
   */
  void update();

private:
  EntityContext &entityContext;
  VkDescriptorSet sceneDescriptorSet;
  VulkanDescriptorManager *descriptorManager;
  SharedPtr<Texture> shadowmaps;
  SharedPtr<VulkanHardwareBuffer> sceneBuffer;
  Scene *scene;
  Entity environmentMapEntity = ENTITY_MAX;

  const std::vector<SharedPtr<Material>> &shadowMaterials;
};

} // namespace liquid
