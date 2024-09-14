#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class TransformLuaTable {
public:
  TransformLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  std::reference_wrapper<glm::vec3> getPosition();

  void setPosition(glm::vec3 position);

  std::reference_wrapper<glm::quat> getRotation();

  void setRotation(glm::quat rotation);

  std::reference_wrapper<glm::vec3> getScale();

  void setScale(glm::vec3 scale);

  void deleteThis();

  static void create(sol::usertype<TransformLuaTable> usertype,
                     sol::state_view state);

  static const String getName() { return "localTransform"; }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
