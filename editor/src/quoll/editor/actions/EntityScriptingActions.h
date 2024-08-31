#pragma once

#include "quoll/lua-scripting/LuaScript.h"
#include "Action.h"

namespace quoll::editor {

class EntitySetScriptVariable : public Action {
public:
  EntitySetScriptVariable(Entity entity, const String &name,
                          const LuaScriptInputVariable &value);

  ActionExecutorResult onExecute(WorkspaceState &state,
                                 AssetCache &assetCache) override;

  ActionExecutorResult onUndo(WorkspaceState &state,
                              AssetCache &assetCache) override;

  bool predicate(WorkspaceState &state, AssetCache &assetCache) override;

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
