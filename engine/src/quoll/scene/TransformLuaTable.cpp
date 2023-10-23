#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/scene/LocalTransform.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "TransformLuaTable.h"

namespace quoll {

TransformLuaTable::TransformLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

std::tuple<f32, f32, f32> TransformLuaTable::getPosition() {
  auto pos =
      mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localPosition;
  return {pos.x, pos.y, pos.z};
}

void TransformLuaTable::setPosition(f32 x, f32 y, f32 z) {
  mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localPosition =
      glm::vec3{x, y, z};
}

std::tuple<f32, f32, f32> TransformLuaTable::getRotation() {
  const auto &rotationQuat =
      mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localRotation;

  glm::vec3 rotationEuler{};

  glm::extractEulerAngleXYZ(glm::toMat4(rotationQuat), rotationEuler.x,
                            rotationEuler.y, rotationEuler.z);
  rotationEuler = glm::degrees(rotationEuler);
  return {rotationEuler.z, rotationEuler.y, rotationEuler.x};
}

void TransformLuaTable::setRotation(f32 x, f32 y, f32 z) {
  glm::vec3 rotation{x, y, z};
  auto newRotation = glm::radians(rotation);

  // glm::quat accepts euler angles
  // in pitch-yaw-roll representation,
  // while we pass XYZ
  mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localRotation =
      glm::toQuat(
          glm::eulerAngleXYZ(newRotation.x, newRotation.y, newRotation.z));
}

std::tuple<f32, f32, f32> TransformLuaTable::getScale() {
  auto scale =
      mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localScale;
  return {scale.x, scale.y, scale.z};
}

void TransformLuaTable::setScale(f32 x, f32 y, f32 z) {
  mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localScale =
      glm::vec3{x, y, z};
}

void TransformLuaTable::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<LocalTransform>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<LocalTransform>(mEntity);
  }
}

void TransformLuaTable::create(sol::usertype<TransformLuaTable> usertype) {
  usertype["get_position"] = &TransformLuaTable::getPosition;
  usertype["set_position"] = &TransformLuaTable::setPosition;
  usertype["get_rotation"] = &TransformLuaTable::getRotation;
  usertype["set_rotation"] = &TransformLuaTable::setRotation;
  usertype["get_scale"] = &TransformLuaTable::getScale;
  usertype["set_scale"] = &TransformLuaTable::setScale;
  usertype["delete"] = &TransformLuaTable::deleteThis;
}

} // namespace quoll
