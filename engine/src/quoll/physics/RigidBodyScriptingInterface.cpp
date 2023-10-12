#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "RigidBodyScriptingInterface.h"

namespace quoll {

RigidBodyScriptingInterface::LuaInterface::LuaInterface(
    Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void RigidBodyScriptingInterface::LuaInterface::setDefaultParams() {
  mScriptGlobals.entityDatabase.set<RigidBody>(mEntity, {});
}

sol_maybe<float> RigidBodyScriptingInterface::LuaInterface::getMass() {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<RigidBody>(mEntity).dynamicDesc.mass;
}

void RigidBodyScriptingInterface::LuaInterface::setMass(float mass) {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.mass = mass;
    mScriptGlobals.entityDatabase.set(mEntity, rigidBody);
  } else {
    mScriptGlobals.entityDatabase.get<RigidBody>(mEntity).dynamicDesc.mass =
        mass;
  }
}

std::tuple<sol_maybe<float>, sol_maybe<float>, sol_maybe<float>>
RigidBodyScriptingInterface::LuaInterface::getInertia() {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return {sol::nil, sol::nil, sol::nil};
  }

  const auto &inertia =
      mScriptGlobals.entityDatabase.get<RigidBody>(mEntity).dynamicDesc.inertia;

  return {inertia.x, inertia.y, inertia.z};
}

void RigidBodyScriptingInterface::LuaInterface::setInertia(float x, float y,
                                                           float z) {
  glm::vec3 inertia{x, y, z};

  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.inertia = inertia;
    mScriptGlobals.entityDatabase.set(mEntity, rigidBody);
  } else {
    mScriptGlobals.entityDatabase.get<RigidBody>(mEntity).dynamicDesc.inertia =
        inertia;
  }
}

sol_maybe<bool> RigidBodyScriptingInterface::LuaInterface::isGravityApplied() {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<RigidBody>(mEntity)
      .dynamicDesc.applyGravity;
}

void RigidBodyScriptingInterface::LuaInterface::applyGravity(bool apply) {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.applyGravity = apply;
    mScriptGlobals.entityDatabase.set(mEntity, rigidBody);
  } else {
    mScriptGlobals.entityDatabase.get<RigidBody>(mEntity)
        .dynamicDesc.applyGravity = apply;
  }
}

void RigidBodyScriptingInterface::LuaInterface::applyForce(float x, float y,
                                                           float z) {
  glm::vec3 force{x, y, z};
  mScriptGlobals.entityDatabase.set<Force>(mEntity, {force});
}

void RigidBodyScriptingInterface::LuaInterface::applyTorque(float x, float y,
                                                            float z) {
  glm::vec3 torque{x, y, z};
  mScriptGlobals.entityDatabase.set<Torque>(mEntity, {torque});
}

void RigidBodyScriptingInterface::LuaInterface::clear() {
  mScriptGlobals.entityDatabase.set<RigidBodyClear>(mEntity, {});
}

void RigidBodyScriptingInterface::LuaInterface::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<RigidBody>(mEntity);
  }
}

void RigidBodyScriptingInterface::LuaInterface::create(
    sol::usertype<RigidBodyScriptingInterface::LuaInterface> usertype) {
  usertype["set_default_params"] = &LuaInterface::setDefaultParams;
  usertype["get_mass"] = &LuaInterface::getMass;
  usertype["set_mass"] = &LuaInterface::setMass;
  usertype["get_inertia"] = &LuaInterface::getInertia;
  usertype["set_inertia"] = &LuaInterface::setInertia;
  usertype["is_gravity_applied"] = &LuaInterface::isGravityApplied;
  usertype["apply_gravity"] = &LuaInterface::applyGravity;
  usertype["apply_force"] = &LuaInterface::applyForce;
  usertype["apply_torque"] = &LuaInterface::applyTorque;
  usertype["clear"] = &LuaInterface::clear;
  usertype["delete"] = &LuaInterface::deleteThis;
}

} // namespace quoll
