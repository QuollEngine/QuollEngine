#pragma once

#include "liquid/entity/Entity.h"
#include "liquid/entity/EntityContext.h"

namespace liquid {

class Scene {
public:
  /**
   * @brief Creates root scene
   *
   * @param entityContext Entity context
   */
  Scene(EntityContext &entityContext);

  /**
   * @brief Destroys root scene
   */
  ~Scene();

  Scene(const Scene &rhs) = delete;
  Scene(Scene &&rhs) = delete;
  Scene &operator=(const Scene &rhs) = delete;
  Scene &operator=(Scene &&rhs) = delete;

  /**
   * @brief Updates scene
   */
  void update();

  /**
   * @brief Get entity context
   *
   * @return Entity context
   */
  inline EntityContext &getEntityContext() { return mEntityContext; }

private:
  /**
   * @brief Update all transforms
   */
  void updateTransforms();

  /**
   * @brief Update all cameras using transforms
   */
  void updateCameras();

  /**
   * @brief Update all lights using transforms
   */
  void updateLights();

private:
  EntityContext &mEntityContext;
};

} // namespace liquid
