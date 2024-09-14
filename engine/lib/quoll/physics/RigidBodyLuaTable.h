#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class RigidBodyLuaTable {
public:
  RigidBodyLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  void setDefaultParams();

  sol_maybe<u32> getType();

  sol_maybe<f32> getMass();

  void setMass(f32 mass);

  sol_maybe<std::reference_wrapper<glm::vec3>> getInertia();

  void setInertia(glm::vec3 inertia);

  sol_maybe<bool> isGravityApplied();

  void applyGravity(bool apply);

  void applyForce(f32 x, f32 y, f32 z);

  void applyImpulse(f32 x, f32 y, f32 z);

  void applyTorque(f32 x, f32 y, f32 z);

  void clear();

  void deleteThis();

  static void create(sol::usertype<RigidBodyLuaTable> usertype,
                     sol::state_view state);

  static const String getName() { return "rigidBody"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
