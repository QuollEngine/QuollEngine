#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/physics/Collidable.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/lua-scripting/Messages.h"

#include "CollidableLuaTable.h"

namespace quoll {

CollidableLuaTable::CollidableLuaTable(Entity entity,
                                       ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void CollidableLuaTable::setDefaultMaterial() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    mScriptGlobals.entityDatabase.set(mEntity, Collidable{});
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity).materialDesc =
        quoll::PhysicsMaterialDesc{};
  }
}

sol_maybe<f32> CollidableLuaTable::getStaticFriction() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
      .materialDesc.staticFriction;
}

void CollidableLuaTable::setStaticFriction(f32 staticFriction) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.materialDesc.staticFriction = staticFriction;
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
        .materialDesc.staticFriction = staticFriction;
  }
}

sol_maybe<f32> CollidableLuaTable::getDynamicFriction() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
      .materialDesc.dynamicFriction;
}

void CollidableLuaTable::setDynamicFriction(f32 dynamicFriction) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.materialDesc.dynamicFriction = dynamicFriction;
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
        .materialDesc.dynamicFriction = dynamicFriction;
  }
}

sol_maybe<f32> CollidableLuaTable::getRestitution() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
      .materialDesc.restitution;
}

void CollidableLuaTable::setRestitution(f32 restitution) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.materialDesc.restitution = restitution;
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
        .materialDesc.restitution = restitution;
  }
}

void CollidableLuaTable::setBoxGeometry(f32 hx, f32 hy, f32 hz) {
  glm::vec3 halfExtents{hx, hy, hz};

  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Box;
    collidable.geometryDesc.params = PhysicsGeometryBox{halfExtents};
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    auto &collidable = mScriptGlobals.entityDatabase.get<Collidable>(mEntity);
    collidable.geometryDesc.type = PhysicsGeometryType::Box;
    collidable.geometryDesc.params = PhysicsGeometryBox{halfExtents};
  }
}

void CollidableLuaTable::setSphereGeometry(f32 radius) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Sphere;
    collidable.geometryDesc.params = PhysicsGeometrySphere{radius};
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    auto &collidable = mScriptGlobals.entityDatabase.get<Collidable>(mEntity);
    collidable.geometryDesc.type = PhysicsGeometryType::Sphere;
    collidable.geometryDesc.params = PhysicsGeometrySphere{radius};
  }
}

void CollidableLuaTable::setCapsuleGeometry(f32 radius, f32 halfHeight) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Capsule;
    collidable.geometryDesc.params = PhysicsGeometryCapsule{radius, halfHeight};
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    auto &collidable = mScriptGlobals.entityDatabase.get<Collidable>(mEntity);
    collidable.geometryDesc.type = PhysicsGeometryType::Capsule;
    collidable.geometryDesc.params = PhysicsGeometryCapsule{radius, halfHeight};
  }
}

void CollidableLuaTable::setPlaneGeometry() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.geometryDesc.type = PhysicsGeometryType::Plane;
    collidable.geometryDesc.params = PhysicsGeometryPlane{};

    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    auto &collidable = mScriptGlobals.entityDatabase.get<Collidable>(mEntity);
    collidable.geometryDesc.type = PhysicsGeometryType::Plane;
    collidable.geometryDesc.params = PhysicsGeometryPlane{};
  }
}

std::tuple<bool, sol_maybe<CollisionHit>>
CollidableLuaTable::sweep(f32 dx, f32 dy, f32 dz, f32 maxDistance) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
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
  if (mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<Collidable>(mEntity);
  }
}

void CollidableLuaTable::create(sol::usertype<CollidableLuaTable> usertype) {
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
