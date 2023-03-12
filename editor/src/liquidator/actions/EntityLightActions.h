#pragma once

#include "liquidator/actions/Action.h"

namespace liquid::editor {

/**
 * @brief Set directional light for entity action
 */
class EntitySetDirectionalLight : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param light Directional light
   */
  EntitySetDirectionalLight(Entity entity, DirectionalLight light);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  Entity mEntity;
  DirectionalLight mLight;
};

/**
 * @brief Enable cascaded shadow map for entity action
 */
class EntityEnableCascadedShadowMap : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityEnableCascadedShadowMap(Entity entity);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  Entity mEntity;
};

/**
 * @brief Disable cascaded shadow map for entity action
 */
class EntityDisableCascadedShadowMap : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   */
  EntityDisableCascadedShadowMap(Entity entity);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  Entity mEntity;
};

/**
 * @brief Set cascaded shadow map for entity action
 */
class EntitySetCascadedShadowMapAction : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param shadowMap Cascaded shadow map
   */
  EntitySetCascadedShadowMapAction(Entity entity, CascadedShadowMap shadowMap);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state) override;

private:
  Entity mEntity;
  CascadedShadowMap mShadowMap;
};

} // namespace liquid::editor
