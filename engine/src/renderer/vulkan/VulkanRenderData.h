#pragma once

#include "VulkanHardwareBuffer.h"
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
                   ResourceAllocator *resourceAllocator,
                   const SharedPtr<Texture> &shadowmaps,
                   const std::vector<SharedPtr<Material>> &shadowMaterials);

  /**
   * @brief Get scene
   *
   * @return Scene
   */
  inline Scene *getScene() { return scene; }

  /**
   * @brief Get scene buffer
   *
   * @return Scene buffer
   */
  inline const SharedPtr<VulkanHardwareBuffer> &getSceneBuffer() const {
    return sceneBuffer;
  }

  /**
   * @brief Update scene data
   */
  void update();

  /**
   * @brief Check if environment is changed
   *
   * @retval true Environment changes
   * @retval false Environment did not change
   */
  inline bool isEnvironmentChanged() const { return environmentChanged; }

  /**
   * @brief Get environment map textures
   *
   * @return Environment map textures
   */
  std::array<SharedPtr<Texture>, 3> getEnvironmentTextures() const;

  /**
   * @brief Clean environment change flag
   */
  void cleanEnvironmentChangeFlag();

private:
  EntityContext &entityContext;
  SharedPtr<Texture> shadowmaps;
  SharedPtr<VulkanHardwareBuffer> sceneBuffer;
  Scene *scene;
  Entity environmentMapEntity = ENTITY_MAX;
  bool environmentChanged = false;

  const std::vector<SharedPtr<Material>> &shadowMaterials;
};

} // namespace liquid
