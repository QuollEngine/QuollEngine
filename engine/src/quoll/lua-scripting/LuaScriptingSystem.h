#pragma once

#include "quoll/asset/AssetRegistry.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/window/Window.h"
#include "Interpreter.h"
#include "LuaScript.h"
#include "ScriptLoop.h"

namespace quoll {

class LuaScriptingSystem {
public:
  LuaScriptingSystem(AssetRegistry &assetRegistry);

  ~LuaScriptingSystem() = default;

  LuaScriptingSystem(const LuaScriptingSystem &) = delete;
  LuaScriptingSystem &operator=(const LuaScriptingSystem &) = delete;
  LuaScriptingSystem(LuaScriptingSystem &&) = delete;
  LuaScriptingSystem &operator=(LuaScriptingSystem &&) = delete;

  void start(EntityDatabase &entityDatabase, PhysicsSystem &physicsSystem,
             WindowSignals &windowSignals);

  void update(f32 dt, EntityDatabase &entityDatabase);

  void cleanup(EntityDatabase &entityDatabase);

  void observeChanges(EntityDatabase &entityDatabase);

private:
  void destroyScriptingData(LuaScript &component);

  void createScriptingData(LuaScript &component, Entity event);

private:
  AssetRegistry &mAssetRegistry;
  lua::Interpreter mLuaInterpreter;
  lua::ScriptLoop mScriptLoop;

  EntityDatabaseObserver<LuaScript> mScriptRemoveObserver;
};

} // namespace quoll
