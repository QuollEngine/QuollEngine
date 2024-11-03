#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/lua-scripting/Messages.h"
#include "Force.h"
#include "Impulse.h"
#include "RigidBody.h"
#include "RigidBodyClear.h"
#include "RigidBodyLuaTable.h"
#include "Torque.h"

namespace quoll {

RigidBodyLuaTable::RigidBodyLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void RigidBodyLuaTable::setDefaultParams() {
  mScriptGlobals.entityDatabase.set<RigidBody>(mEntity, {});
}

sol_maybe<u32> RigidBodyLuaTable::getType() {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return static_cast<u32>(
      mScriptGlobals.entityDatabase.get<RigidBody>(mEntity).type);
}

sol_maybe<f32> RigidBodyLuaTable::getMass() {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<RigidBody>(mEntity).dynamicDesc.mass;
}

void RigidBodyLuaTable::setMass(f32 mass) {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.mass = mass;
    mScriptGlobals.entityDatabase.set(mEntity, rigidBody);
  } else {
    mScriptGlobals.entityDatabase.get<RigidBody>(mEntity).dynamicDesc.mass =
        mass;
  }
}
sol_maybe<std::reference_wrapper<glm::vec3>> RigidBodyLuaTable::getInertia() {
  if (!mScriptGlobals.entityDatabase.has<RigidBody>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<RigidBody>(mEntity)
      .dynamicDesc.inertia;
}

void RigidBodyLuaTable::setInertia(glm::vec3 inertia) {
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
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
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

void RigidBodyLuaTable::applyForce(f32 x, f32 y, f32 z) {
  const glm::vec3 force{x, y, z};
  mScriptGlobals.entityDatabase.set<Force>(mEntity, {force});
}

void RigidBodyLuaTable::applyImpulse(f32 x, f32 y, f32 z) {
  const glm::vec3 impulse{x, y, z};
  mScriptGlobals.entityDatabase.set<Impulse>(mEntity, {impulse});
}

void RigidBodyLuaTable::applyTorque(f32 x, f32 y, f32 z) {
  const glm::vec3 torque{x, y, z};
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

void RigidBodyLuaTable::create(sol::usertype<RigidBodyLuaTable> usertype,
                               sol::state_view state) {
  state["RigidBodyType"] = state.create_table_with(
      "Dynamic", RigidBodyType::Dynamic, "Kinematic", RigidBodyType::Kinematic);

  usertype["setDefaultParams"] = &RigidBodyLuaTable::setDefaultParams;
  usertype["type"] = sol::property(&RigidBodyLuaTable::getType);
  usertype["mass"] =
      sol::property(&RigidBodyLuaTable::getMass, &RigidBodyLuaTable::setMass);
  usertype["inertia"] = sol::property(&RigidBodyLuaTable::getInertia,
                                      &RigidBodyLuaTable::setInertia);
  usertype["isGravityApplied"] = sol::property(
      &RigidBodyLuaTable::isGravityApplied, &RigidBodyLuaTable::applyGravity);
  usertype["applyForce"] = &RigidBodyLuaTable::applyForce;
  usertype["applyImpulse"] = &RigidBodyLuaTable::applyImpulse;
  usertype["applyTorque"] = &RigidBodyLuaTable::applyTorque;
  usertype["clear"] = &RigidBodyLuaTable::clear;
  usertype["delete"] = &RigidBodyLuaTable::deleteThis;
}

} // namespace quoll
