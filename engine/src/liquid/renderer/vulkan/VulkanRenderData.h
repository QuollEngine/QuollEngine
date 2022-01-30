#pragma once

#include "liquid/renderer/HardwareBuffer.h"
#include "liquid/renderer/ResourceAllocator.h"
#include "liquid/renderer/Material.h"
#include "liquid/renderer/Descriptor.h"
#include "liquid/scene/Scene.h"
#include "liquid/entity/EntityContext.h"

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
  inline const SharedPtr<HardwareBuffer> &getSceneBuffer() const {
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
  SharedPtr<HardwareBuffer> sceneBuffer;
  Scene *scene;
  Entity environmentMapEntity = ENTITY_MAX;
  bool environmentChanged = false;
  Descriptor descriptor;

  const std::vector<SharedPtr<Material>> &shadowMaterials;
};

} // namespace liquid
