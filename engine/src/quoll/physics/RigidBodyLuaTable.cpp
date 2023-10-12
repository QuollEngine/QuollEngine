#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "RigidBodyLuaTable.h"

namespace quoll {

RigidBodyLuaTable::RigidBodyLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void RigidBodyLuaTable::setDefaultParams() {
  mScriptGlobals.entityDatabase.set<RigidBody>(mEntity, {});
}

sol_maybe<float> RigidBodyLuaTable::getMass() {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<RigidBody>(mEntity).dynamicDesc.mass;
}

void RigidBodyLuaTable::setMass(float mass) {
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
RigidBodyLuaTable::getInertia() {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return {sol::nil, sol::nil, sol::nil};
  }

  const auto &inertia =
      mScriptGlobals.entityDatabase.get<RigidBody>(mEntity).dynamicDesc.inertia;

  return {inertia.x, inertia.y, inertia.z};
}

void RigidBodyLuaTable::setInertia(float x, float y, float z) {
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

sol_maybe<bool> RigidBodyLuaTable::isGravityApplied() {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<RigidBody>(mEntity)
      .dynamicDesc.applyGravity;
}

void RigidBodyLuaTable::applyGravity(bool apply) {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.applyGravity = apply;
    mScriptGlobals.entityDatabase.set(mEntity, rigidBody);
  } else {
    mScriptGlobals.entityDatabase.get<RigidBody>(mEntity)
        .dynamicDesc.applyGravity = apply;
  }
}

void RigidBodyLuaTable::applyForce(float x, float y, float z) {
  glm::vec3 force{x, y, z};
  mScriptGlobals.entityDatabase.set<Force>(mEntity, {force});
}

void RigidBodyLuaTable::applyTorque(float x, float y, float z) {
  glm::vec3 torque{x, y, z};
  mScriptGlobals.entityDatabase.set<Torque>(mEntity, {torque});
}

void RigidBodyLuaTable::clear() {
  mScriptGlobals.entityDatabase.set<RigidBodyClear>(mEntity, {});
}

void RigidBodyLuaTable::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<RigidBody>(mEntity);
  }
}

void RigidBodyLuaTable::create(sol::usertype<RigidBodyLuaTable> usertype) {
  usertype["set_default_params"] = &RigidBodyLuaTable::setDefaultParams;
  usertype["get_mass"] = &RigidBodyLuaTable::getMass;
  usertype["set_mass"] = &RigidBodyLuaTable::setMass;
  usertype["get_inertia"] = &RigidBodyLuaTable::getInertia;
  usertype["set_inertia"] = &RigidBodyLuaTable::setInertia;
  usertype["is_gravity_applied"] = &RigidBodyLuaTable::isGravityApplied;
  usertype["apply_gravity"] = &RigidBodyLuaTable::applyGravity;
  usertype["apply_force"] = &RigidBodyLuaTable::applyForce;
  usertype["apply_torque"] = &RigidBodyLuaTable::applyTorque;
  usertype["clear"] = &RigidBodyLuaTable::clear;
  usertype["delete"] = &RigidBodyLuaTable::deleteThis;
}

} // namespace quoll
