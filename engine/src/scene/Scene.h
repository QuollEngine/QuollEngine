#pragma once

#include "core/Base.h"

#include "Camera.h"
#include "Light.h"
#include "entity/Entity.h"
#include "entity/EntityContext.h"

namespace liquid {

class Scene;

class SceneNode {
public:
  /**
   * @brief Create scene node
   *
   * @param entity Entity
   * @param transform Local transform
   * @param parent Parent node
   */
  SceneNode(Entity entity, glm::mat4 transform, SceneNode *parent,
            EntityContext &context);

  /**
   * @brief Destroys scene node and its children
   */
  ~SceneNode();

  SceneNode(const SceneNode &rhs) = delete;
  SceneNode(SceneNode &&rhs) = delete;
  SceneNode &operator=(const SceneNode &rhs) = delete;
  SceneNode &operator=(SceneNode &&rhs) = delete;

  /**
   * @brief Updates children
   *
   * @param forceUpdate Force update even if node is not modified
   */
  void update(bool forceUpdate = false);

  /**
   * @brief Adds child node with entity
   *
   * @param entity Entity
   * @param transform Local transform
   * @return Pointer to newly created scene node
   */
  SceneNode *addChild(Entity entity, glm::mat4 transform = glm::mat4{1.0});

  /**
   * @brief Sets transform
   *
   * @param transform Local transform
   */
  void setTransform(glm::mat4 transform);

  /**
   * @brief Set entity
   *
   * @param entity Entity
   */
  void setEntity(Entity entity);

  /**
   * @brief Gets world transform
   *
   * @return World transform matrix
   */
  inline const glm::mat4 &getWorldTransform() const {
    return entityContext.getComponent<TransformComponent>(entity)
        .transformWorld;
  }

  /**
   * @brief Get entity
   *
   * @return Entity
   */
  inline Entity getEntity() const { return entity; }

  /**
   * @brief Gets children
   *
   * @return List of children
   */
  inline const std::vector<SceneNode *> &getChildren() const {
    return children;
  }

  /**
   * @brief Gets parent node
   *
   * @return Parent node
   */
  inline SceneNode *getParent() { return parent; }

private:
  bool dirty = true;
  Entity entity = std::numeric_limits<Entity>::max();

  SceneNode *parent = nullptr;
  std::vector<SceneNode *> children;

  EntityContext &entityContext;
};

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
   * @brief Sets active camera
   *
   * @param camera Camera
   */
  void setActiveCamera(Camera *camera);

  /** @brief Gets active camera
   *
   * @return Camera
   */
  inline Camera *getActiveCamera() { return camera; }

  /**
   * @brief Get root node
   *
   * @return Root scene node
   */
  inline SceneNode *getRootNode() { return rootNode; }

  /**
   * @brief Get entity context
   *
   * @return Entity context
   */
  inline EntityContext &getEntityContext() { return entityContext; }

private:
  SceneNode *rootNode = nullptr;
  Camera *camera = nullptr;

  EntityContext &entityContext;
};

} // namespace liquid
