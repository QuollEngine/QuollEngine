#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "TransformScriptingInterface.h"

namespace quoll {

TransformScriptingInterface::LuaInterface::LuaInterface(
    Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

std::tuple<float, float, float>
TransformScriptingInterface::LuaInterface::getPosition() {
  auto pos =
      mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localPosition;
  return {pos.x, pos.y, pos.z};
}

void TransformScriptingInterface::LuaInterface::setPosition(float x, float y,
                                                            float z) {
  mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localPosition =
      glm::vec3{x, y, z};
}

std::tuple<float, float, float>
TransformScriptingInterface::LuaInterface::getRotation() {
  const auto &rotationQuat =
      mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localRotation;

  glm::vec3 rotationEuler{};

  glm::extractEulerAngleXYZ(glm::toMat4(rotationQuat), rotationEuler.x,
                            rotationEuler.y, rotationEuler.z);
  rotationEuler = glm::degrees(rotationEuler);
  return {rotationEuler.z, rotationEuler.y, rotationEuler.x};
}

void TransformScriptingInterface::LuaInterface::setRotation(float x, float y,
                                                            float z) {
  glm::vec3 rotation{x, y, z};
  auto newRotation = glm::radians(rotation);

  // glm::quat accepts euler angles
  // in pitch-yaw-roll representation,
  // while we pass XYZ
  mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localRotation =
      glm::toQuat(
          glm::eulerAngleXYZ(newRotation.x, newRotation.y, newRotation.z));
}

std::tuple<float, float, float>
TransformScriptingInterface::LuaInterface::getScale() {
  auto scale =
      mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localScale;
  return {scale.x, scale.y, scale.z};
}

void TransformScriptingInterface::LuaInterface::setScale(float x, float y,
                                                         float z) {
  mScriptGlobals.entityDatabase.get<LocalTransform>(mEntity).localScale =
      glm::vec3{x, y, z};
}

void TransformScriptingInterface::LuaInterface::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<LocalTransform>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<LocalTransform>(mEntity);
  }
}

void TransformScriptingInterface::LuaInterface::create(
    sol::usertype<TransformScriptingInterface::LuaInterface> usertype) {
  usertype["get_position"] = &LuaInterface::getPosition;
  usertype["set_position"] = &LuaInterface::setPosition;
  usertype["get_rotation"] = &LuaInterface::getRotation;
  usertype["set_rotation"] = &LuaInterface::setRotation;
  usertype["get_scale"] = &LuaInterface::getScale;
  usertype["set_scale"] = &LuaInterface::setScale;
  usertype["delete"] = &LuaInterface::deleteThis;
}

} // namespace quoll
