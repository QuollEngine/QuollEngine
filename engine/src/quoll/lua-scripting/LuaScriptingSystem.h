#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/system/SystemView.h"
#include "Interpreter.h"
#include "LuaScript.h"
#include "ScriptLoop.h"

namespace quoll {

class AssetRegistry;
class WindowSignals;
class PhysicsSystem;
class EntityDatabase;
struct SystemView;

class LuaScriptingSystem : NoCopyMove {
public:
  LuaScriptingSystem(AssetRegistry &assetRegistry);

  ~LuaScriptingSystem() = default;

  void createSystemViewData(SystemView &view);

  void start(SystemView &view, PhysicsSystem &physicsSystem,
             WindowSignals &windowSignals);

  void update(f32 dt, SystemView &view);

  void cleanup(SystemView &view);

private:
  void destroyScriptingData(LuaScript &component);

private:
  AssetRegistry &mAssetRegistry;
  lua::Interpreter mLuaInterpreter;
  lua::ScriptLoop mScriptLoop;
};

} // namespace quoll
