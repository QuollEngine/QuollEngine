#pragma once

#include "liquid/renderer/Material.h"
#include "liquid/rhi/Descriptor.h"
#include "liquid/scene/Scene.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

class RenderData {
  constexpr static size_t MAX_LIGHTS = 16;

  struct LightData {
    // NOLINTNEXTLINE
    alignas(16) glm::vec4 color;
    // NOLINTNEXTLINE
    alignas(16) glm::vec4 direction;
    // NOLINTNEXTLINE
    alignas(16) glm::uvec4 type;
    // NOLINTNEXTLINE
    alignas(16) glm::mat4 lightSpaceMatrix;
  };

  struct SceneBufferObject {
    std::array<LightData, MAX_LIGHTS> lights{};
    glm::uvec4 numLights;
    glm::uvec4 hasIBL;
  };

public:
  /**
   * @brief Creates render data
   *
   * @param entityContext Entity context
   * @param scene Scene
   * @param descriptorManager rhi::Descriptor manager
   * @param shadowMaterials Shadow materials
   * @param registry Resource registry
   */
  RenderData(EntityContext &entityContext, Scene *scene,
             const std::vector<SharedPtr<Material>> &shadowMaterials,
             rhi::ResourceRegistry &registry);

  /**
   * @brief Get scene
   *
   * @return Scene
   */
  inline Scene *getScene() { return mScene; }

  /**
   * @brief Get scene buffer
   *
   * @return Scene buffer
   */
  inline rhi::BufferHandle getSceneBuffer() const { return mSceneBuffer; }

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
  inline bool isEnvironmentChanged() const { return mEnvironmentChanged; }

  /**
   * @brief Get environment map textures
   *
   * @return Environment map textures
   */
  std::array<rhi::TextureHandle, 3> getEnvironmentTextures() const;

  /**
   * @brief Clean environment change flag
   */
  void cleanEnvironmentChangeFlag();

private:
  EntityContext &mEntityContext;
  rhi::BufferHandle mSceneBuffer;
  Scene *mScene;
  Entity mEnvironmentMapEntity = ENTITY_MAX;
  bool mEnvironmentChanged = false;
  rhi::Descriptor mDescriptor;
  SceneBufferObject mSceneData{};
  rhi::ResourceRegistry &mRegistry;

  const std::vector<SharedPtr<Material>> &mShadowMaterials;
};

} // namespace liquid
