#pragma once

#include "liquidator/actions/Action.h"
#include "liquidator/actions/EntityDefaultDeleteAction.h"

namespace quoll::editor {

/**
 * @brief Create script entity action
 */
class EntityCreateScript : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param handle Script asset handle
   */
  EntityCreateScript(Entity entity, LuaScriptAssetHandle handle);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  LuaScriptAssetHandle mHandle;
};

/**
 * @brief Set script for entity action
 */
class EntitySetScript : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param script Script handle
   */
  EntitySetScript(Entity entity, LuaScriptAssetHandle script);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  LuaScriptAssetHandle mScript;
  LuaScriptAssetHandle mOldScript{};
};

/**
 * @brief Set script variable for entity
 */
class EntitySetScriptVariable : public Action {
public:
  /**
   * @brief Create action
   *
   * @param entity Entity
   * @param name Variable name
   * @param value Variable value
   */
  EntitySetScriptVariable(Entity entity, const String &name,
                          const LuaScriptInputVariable &value);

  /**
   * @brief Action executor
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  /**
   * @brief Action undo
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @return Executor result
   */
  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  /**
   * @brief Action predicate
   *
   * @param state Workspace state
   * @param assetRegistry Asset registry
   * @retval true Predicate is true
   * @retval false Predicate is false
   */
  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

  /**
   * @brief Set variable value
   *
   * @param value Variable value
   */
  void setValue(LuaScriptInputVariable value);

  /**
   * @brief Get value
   *
   * @return Variable value
   */
  inline const LuaScriptInputVariable &getValue() const { return mValue; }

  /**
   * @brief Get variable name
   *
   * @return Variable name
   */
  inline const String &getName() const { return mName; }

private:
  Entity mEntity;
  String mName;
  LuaScriptInputVariable mValue;
  Script mOldScript{};
};

using EntityDeleteScript = EntityDefaultDeleteAction<Script>;

} // namespace quoll::editor
