#pragma once

#include "quoll/lua-scripting/LuaScript.h"
#include "Action.h"

namespace quoll::editor {

class EntityCreateScript : public Action {
public:
  EntityCreateScript(Entity entity, LuaScriptAssetHandle handle);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  LuaScriptAssetHandle mHandle;
};

class EntitySetScript : public Action {
public:
  EntitySetScript(Entity entity, LuaScriptAssetHandle script);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

private:
  Entity mEntity;
  LuaScriptAssetHandle mScript;
  LuaScriptAssetHandle mOldScript{};
};

class EntitySetScriptVariable : public Action {
public:
  EntitySetScriptVariable(Entity entity, const String &name,
                          const LuaScriptInputVariable &value);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetRegistry &assetRegistry) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetRegistry &assetRegistry) override;

  bool predicate(WorkspaceState &state, AssetRegistry &assetRegistry) override;

  void setValue(LuaScriptInputVariable value);

  inline const LuaScriptInputVariable &getValue() const { return mValue; }

  inline const String &getName() const { return mName; }

private:
  Entity mEntity;
  String mName;
  LuaScriptInputVariable mValue;
  LuaScript mOldScript{};
};

} // namespace quoll::editor
