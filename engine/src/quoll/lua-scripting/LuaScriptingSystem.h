#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "Interpreter.h"
#include "LuaScript.h"
#include "ScriptLoop.h"

namespace quoll {

class AssetRegistry;
class WindowSignals;
class PhysicsSystem;
class EntityDatabase;

class LuaScriptingSystem : NoCopyMove {
public:
  LuaScriptingSystem(AssetRegistry &assetRegistry);

  ~LuaScriptingSystem() = default;

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
