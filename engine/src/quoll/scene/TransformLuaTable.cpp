#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/lua-scripting/Messages.h"
#include "quoll/scene/LocalTransform.h"
#include "TransformLuaTable.h"

namespace quoll {

TransformLuaTable::TransformLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<std::reference_wrapper<glm::vec3>> TransformLuaTable::getPosition() {
  if (!mEntity.has<LocalTransform>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<LocalTransform>()->localPosition;
}

void TransformLuaTable::setPosition(glm::vec3 position) {
  if (!mEntity.has<LocalTransform>()) {
    mEntity.set<LocalTransform>({.localPosition = position});
  } else {
    mEntity.get_ref<LocalTransform>()->localPosition = position;
  }
}

sol_maybe<std::reference_wrapper<glm::quat>> TransformLuaTable::getRotation() {
  if (!mEntity.has<LocalTransform>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<LocalTransform>()->localRotation;
}

void TransformLuaTable::setRotation(glm::quat rotation) {
  if (!mEntity.has<LocalTransform>()) {
    mEntity.set<LocalTransform>({.localRotation = rotation});
  } else {
    mEntity.get_ref<LocalTransform>()->localRotation = rotation;
  }
}

sol_maybe<std::reference_wrapper<glm::vec3>> TransformLuaTable::getScale() {
  if (!mEntity.has<LocalTransform>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<LocalTransform>()->localScale;
}

void TransformLuaTable::setScale(glm::vec3 scale) {
  if (!mEntity.has<LocalTransform>()) {
    mEntity.set<LocalTransform>({.localScale = scale});
  } else {
    mEntity.get_ref<LocalTransform>()->localScale = scale;
  }
}

void TransformLuaTable::deleteThis() {
  if (mEntity.has<LocalTransform>()) {
    mEntity.remove<LocalTransform>();
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
