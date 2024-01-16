#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class PerspectiveLensLuaTable {
public:
  PerspectiveLensLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  sol_maybe<f32> getNear();

  void setNear(f32 near);

  sol_maybe<f32> getFar();

  void setFar(f32 far);

  std::tuple<sol_maybe<f32>, sol_maybe<f32>> getSensorSize();

  void setSensorSize(f32 width, f32 height);

  sol_maybe<f32> getFocalLength();

  void setFocalLength(f32 focalLength);

  sol_maybe<f32> getAperture();

  void setAperture(f32 aperture);

  sol_maybe<f32> getShutterSpeed();

  void setShutterSpeed(f32 shutterSpeed);

  sol_maybe<u32> getSensitivity();

  void setSensitivity(u32 sensitivity);

  void deleteThis();

  static void create(sol::usertype<PerspectiveLensLuaTable> usertype,
                     sol::state_view state);

  static const String getName() { return "perspectiveLens"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
