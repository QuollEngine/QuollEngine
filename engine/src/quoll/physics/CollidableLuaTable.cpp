#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/lua-scripting/Messages.h"
#include "quoll/physics/Collidable.h"
#include "quoll/physics/PhysicsSystem.h"
#include "CollidableLuaTable.h"

namespace quoll {

CollidableLuaTable::CollidableLuaTable(Entity entity,
                                       ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void CollidableLuaTable::setDefaultMaterial() {
  if (!mEntity.has<Collidable>()) {
    mEntity.set(Collidable{});
  } else {
    mEntity.get_ref<Collidable>()->materialDesc = quoll::PhysicsMaterialDesc{};
  }
}

sol_maybe<f32> CollidableLuaTable::getStaticFriction() {
  if (!mEntity.has<Collidable>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<Collidable>()->materialDesc.staticFriction;
}

void CollidableLuaTable::setStaticFriction(f32 staticFriction) {
  if (!mEntity.has<Collidable>()) {
    Collidable collidable{};
    collidable.materialDesc.staticFriction = staticFriction;
    mEntity.set(collidable);
  } else {
    mEntity.get_ref<Collidable>()->materialDesc.staticFriction = staticFriction;
  }
}

sol_maybe<f32> CollidableLuaTable::getDynamicFriction() {
  if (!mEntity.has<Collidable>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<Collidable>()->materialDesc.dynamicFriction;
}

void CollidableLuaTable::setDynamicFriction(f32 dynamicFriction) {
  if (!mEntity.has<Collidable>()) {
    Collidable collidable{};
    collidable.materialDesc.dynamicFriction = dynamicFriction;
    mEntity.set(collidable);
  } else {
    mEntity.get_ref<Collidable>()->materialDesc.dynamicFriction =
        dynamicFriction;
  }
}

sol_maybe<f32> CollidableLuaTable::getRestitution() {
  if (!mEntity.has<Collidable>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mEntity.get_ref<Collidable>()->materialDesc.restitution;
}

void CollidableLuaTable::setRestitution(f32 restitution) {
  if (!mEntity.has<Collidable>()) {
    Collidable collidable{};
    collidable.materialDesc.restitution = restitution;
    mEntity.set(collidable);
  } else {
    mEntity.get_ref<Collidable>()->materialDesc.restitution = restitution;
  }
}

void CollidableLuaTable::setBoxGeometry(f32 hx, f32 hy, f32 hz) {
  glm::vec3 halfExtents{hx, hy, hz};

  if (!mEntity.has<Collidable>()) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Box;
    collidable.geometryDesc.params = PhysicsGeometryBox{halfExtents};
    mEntity.set(collidable);
  } else {
    auto collidable = mEntity.get_ref<Collidable>();
    collidable->geometryDesc.type = PhysicsGeometryType::Box;
    collidable->geometryDesc.params = PhysicsGeometryBox{halfExtents};
  }
}

void CollidableLuaTable::setSphereGeometry(f32 radius) {
  if (!mEntity.has<Collidable>()) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Sphere;
    collidable.geometryDesc.params = PhysicsGeometrySphere{radius};
    mEntity.set(collidable);
  } else {
    auto collidable = mEntity.get_ref<Collidable>();
    collidable->geometryDesc.type = PhysicsGeometryType::Sphere;
    collidable->geometryDesc.params = PhysicsGeometrySphere{radius};
  }
}

void CollidableLuaTable::setCapsuleGeometry(f32 radius, f32 halfHeight) {
  if (!mEntity.has<Collidable>()) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Capsule;
    collidable.geometryDesc.params = PhysicsGeometryCapsule{radius, halfHeight};
    mEntity.set(collidable);
  } else {
    auto collidable = mEntity.get_ref<Collidable>();
    collidable->geometryDesc.type = PhysicsGeometryType::Capsule;
    collidable->geometryDesc.params =
        PhysicsGeometryCapsule{radius, halfHeight};
  }
}

void CollidableLuaTable::setPlaneGeometry() {
  if (!mEntity.has<Collidable>()) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Plane;
    collidable.geometryDesc.params = PhysicsGeometryPlane{};

    mEntity.set(collidable);
  } else {
    auto collidable = mEntity.get_ref<Collidable>();
    collidable->geometryDesc.type = PhysicsGeometryType::Plane;
    collidable->geometryDesc.params = PhysicsGeometryPlane{};
  }
}

std::tuple<bool, sol_maybe<CollisionHit>>
CollidableLuaTable::sweep(f32 dx, f32 dy, f32 dz, f32 maxDistance) {
  if (!mEntity.has<Collidable>()) {
    return {false, sol::nil};
  }

  CollisionHit hit{};
  auto result = mScriptGlobals.physicsSystem.sweep(
      mScriptGlobals.entityDatabase, mEntity, {dx, dy, dz}, maxDistance, hit);

  if (result) {
    return {result, hit};
  }

  return {result, sol::nil};
}

void CollidableLuaTable::deleteThis() {
  if (mEntity.has<Collidable>()) {
    mEntity.remove<Collidable>();
  }
}

void CollidableLuaTable::create(sol::usertype<CollidableLuaTable> usertype,
                                sol::state_view state) {
  usertype["setDefaultMaterial"] = &CollidableLuaTable::setDefaultMaterial;
  usertype["staticFriction"] =
      sol::property(&CollidableLuaTable::getStaticFriction,
                    &CollidableLuaTable::setStaticFriction);
  usertype["dynamicFriction"] =
      sol::property(&CollidableLuaTable::getDynamicFriction,
                    &CollidableLuaTable::setDynamicFriction);
  usertype["restitution"] = sol::property(&CollidableLuaTable::getRestitution,
                                          &CollidableLuaTable::setRestitution);
  usertype["setBoxGeometry"] = &CollidableLuaTable::setBoxGeometry;
  usertype["setSphereGeometry"] = &CollidableLuaTable::setSphereGeometry;
  usertype["setCapsuleGeometry"] = &CollidableLuaTable::setCapsuleGeometry;
  usertype["setPlaneGeometry"] = &CollidableLuaTable::setPlaneGeometry;
  usertype["sweep"] = &CollidableLuaTable::sweep;
  usertype["delete"] = &CollidableLuaTable::deleteThis;
}

} // namespace quoll
