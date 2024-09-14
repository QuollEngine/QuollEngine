#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "CollisionHit.h"

namespace quoll {

class CollidableLuaTable {
public:
  CollidableLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  void setDefaultMaterial();

  sol_maybe<f32> getStaticFriction();

  void setStaticFriction(f32 staticFriction);

  sol_maybe<f32> getDynamicFriction();

  void setDynamicFriction(f32 dynamicFriction);

  sol_maybe<f32> getRestitution();

  void setRestitution(f32 restitution);

  void setBoxGeometry(f32 hx, f32 hy, f32 hz);

  void setSphereGeometry(f32 radius);

  void setCapsuleGeometry(f32 radius, f32 halfHeight);

  void setPlaneGeometry();

  std::tuple<bool, sol_maybe<CollisionHit>> sweep(f32 dx, f32 dy, f32 dz,
                                                  f32 maxDistance);

  void deleteThis();

  static void create(sol::usertype<CollidableLuaTable> usertype,
                     sol::state_view state);

  static const String getName() { return "collidable"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
