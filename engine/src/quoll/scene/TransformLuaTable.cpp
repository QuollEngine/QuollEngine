#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/scene/LocalTransform.h"

#include "quoll/entity/EntityDatabase.h"

#include "TransformLuaTable.h"

namespace quoll {

TransformLuaTable::TransformLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

std::reference_wrapper<glm::vec3> TransformLuaTable::getPosition() {
  return mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity)
      .localPosition;
}

void TransformLuaTable::setPosition(glm::vec3 position) {
  mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localPosition =
      position;
}

std::reference_wrapper<glm::quat> TransformLuaTable::getRotation() {
  return mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity)
      .localRotation;
}

void TransformLuaTable::setRotation(glm::quat rotation) {
  mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localRotation =
      rotation;
}

std::reference_wrapper<glm::vec3> TransformLuaTable::getScale() {
  return mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localScale;
}

void TransformLuaTable::setScale(glm::vec3 scale) {
  mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localScale = scale;
}

void TransformLuaTable::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<LocalTransform>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<LocalTransform>(mEntity);
  }
}

void TransformLuaTable::create(sol::usertype<TransformLuaTable> usertype,
                               sol::state_view state) {
  usertype["position"] = sol::property(&TransformLuaTable::getPosition,
                                       &TransformLuaTable::setPosition);
  usertype["scale"] =
      sol::property(&TransformLuaTable::getScale, &TransformLuaTable::setScale);

  usertype["rotation"] = sol::property(&TransformLuaTable::getRotation,
                                       &TransformLuaTable::setRotation);
  usertype["delete"] = &TransformLuaTable::deleteThis;
}

} // namespace quoll
