#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

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

sol_maybe<float> CollidableLuaTable::getStaticFriction() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
      .materialDesc.staticFriction;
}

void CollidableLuaTable::setStaticFriction(float staticFriction) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.materialDesc.staticFriction = staticFriction;
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
        .materialDesc.staticFriction = staticFriction;
  }
}

sol_maybe<float> CollidableLuaTable::getDynamicFriction() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
      .materialDesc.dynamicFriction;
}

void CollidableLuaTable::setDynamicFriction(float dynamicFriction) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.materialDesc.dynamicFriction = dynamicFriction;
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
        .materialDesc.dynamicFriction = dynamicFriction;
  }
}

sol_maybe<float> CollidableLuaTable::getRestitution() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
      .materialDesc.restitution;
}

void CollidableLuaTable::setRestitution(float restitution) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.materialDesc.restitution = restitution;
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
        .materialDesc.restitution = restitution;
  }
}

void CollidableLuaTable::setBoxGeometry(float hx, float hy, float hz) {
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

void CollidableLuaTable::setSphereGeometry(float radius) {
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

void CollidableLuaTable::setCapsuleGeometry(float radius, float halfHeight) {
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
CollidableLuaTable::sweep(float dx, float dy, float dz, float distance) {
  CollisionHit hit{};
  auto result = mScriptGlobals.physicsSystem.sweep(
      mScriptGlobals.entityDatabase, mEntity, {dx, dy, dz}, distance, hit);

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
  usertype["set_default_material"] = &CollidableLuaTable::setDefaultMaterial;
  usertype["get_static_friction"] = &CollidableLuaTable::getStaticFriction;
  usertype["set_static_friction"] = &CollidableLuaTable::setStaticFriction;
  usertype["get_dynamic_friction"] = &CollidableLuaTable::getDynamicFriction;
  usertype["set_dynamic_friction"] = &CollidableLuaTable::setDynamicFriction;
  usertype["get_restitution"] = &CollidableLuaTable::getRestitution;
  usertype["set_restitution"] = &CollidableLuaTable::setRestitution;
  usertype["set_box_geometry"] = &CollidableLuaTable::setBoxGeometry;
  usertype["set_sphere_geometry"] = &CollidableLuaTable::setSphereGeometry;
  usertype["set_capsule_geometry"] = &CollidableLuaTable::setCapsuleGeometry;
  usertype["set_plane_geometry"] = &CollidableLuaTable::setPlaneGeometry;
  usertype["sweep"] = &CollidableLuaTable::sweep;
  usertype["delete"] = &CollidableLuaTable::deleteThis;
}

} // namespace quoll
