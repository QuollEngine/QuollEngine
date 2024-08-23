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

void RigidBodyLuaTable::setDefaultParams() { mEntity.set<RigidBody>({}); }

sol_maybe<u32> RigidBodyLuaTable::getType() {
  if (!mEntity.has<RigidBody>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return static_cast<u32>(mEntity.get_ref<RigidBody>()->type);
}

sol_maybe<f32> RigidBodyLuaTable::getMass() {
  if (!mEntity.has<RigidBody>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<RigidBody>()->dynamicDesc.mass;
}

void RigidBodyLuaTable::setMass(f32 mass) {
  if (!mEntity.has<RigidBody>()) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.mass = mass;
    mEntity.set(rigidBody);
  } else {
    mEntity.get_ref<RigidBody>()->dynamicDesc.mass = mass;
  }
}
sol_maybe<std::reference_wrapper<glm::vec3>> RigidBodyLuaTable::getInertia() {
  if (!mEntity.has<RigidBody>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<RigidBody>()->dynamicDesc.inertia;
}

void RigidBodyLuaTable::setInertia(glm::vec3 inertia) {
  if (!mEntity.has<RigidBody>()) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.inertia = inertia;
    mEntity.set(rigidBody);
  } else {
    mEntity.get_ref<RigidBody>()->dynamicDesc.inertia = inertia;
  }
}

sol_maybe<bool> RigidBodyLuaTable::isGravityApplied() {
  if (!mEntity.has<RigidBody>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<RigidBody>()->dynamicDesc.applyGravity;
}

void RigidBodyLuaTable::applyGravity(bool apply) {
  if (!mEntity.has<RigidBody>()) {
    RigidBody rigidBody{};
    rigidBody.dynamicDesc.applyGravity = apply;
    mEntity.set(rigidBody);
  } else {
    mEntity.get_ref<RigidBody>()->dynamicDesc.applyGravity = apply;
  }
}

void RigidBodyLuaTable::applyForce(f32 x, f32 y, f32 z) {
  glm::vec3 force{x, y, z};
  mEntity.set<Force>({force});
}

void RigidBodyLuaTable::applyImpulse(f32 x, f32 y, f32 z) {
  glm::vec3 impulse{x, y, z};
  mEntity.set<Impulse>({impulse});
}

void RigidBodyLuaTable::applyTorque(f32 x, f32 y, f32 z) {
  glm::vec3 torque{x, y, z};
  mEntity.set<Torque>({torque});
}

void RigidBodyLuaTable::clear() { mEntity.add<RigidBodyClear>(); }

void RigidBodyLuaTable::deleteThis() {
  if (mEntity.has<RigidBody>()) {
    mEntity.remove<RigidBody>();
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
