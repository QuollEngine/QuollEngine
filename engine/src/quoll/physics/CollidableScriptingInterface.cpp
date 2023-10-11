#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "CollidableScriptingInterface.h"

namespace quoll {

CollidableScriptingInterface::LuaInterface::LuaInterface(
    Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void CollidableScriptingInterface::LuaInterface::setDefaultMaterial() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    mScriptGlobals.entityDatabase.set(mEntity, Collidable{});
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity).materialDesc =
        quoll::PhysicsMaterialDesc{};
  }
}

sol_maybe<float>
CollidableScriptingInterface::LuaInterface::getStaticFriction() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
      .materialDesc.staticFriction;
}

void CollidableScriptingInterface::LuaInterface::setStaticFriction(
    float staticFriction) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.materialDesc.staticFriction = staticFriction;
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
        .materialDesc.staticFriction = staticFriction;
  }
}

sol_maybe<float>
CollidableScriptingInterface::LuaInterface::getDynamicFriction() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
      .materialDesc.dynamicFriction;
}

void CollidableScriptingInterface::LuaInterface::setDynamicFriction(
    float dynamicFriction) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.materialDesc.dynamicFriction = dynamicFriction;
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
        .materialDesc.dynamicFriction = dynamicFriction;
  }
}

sol_maybe<float> CollidableScriptingInterface::LuaInterface::getRestitution() {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);
    return sol::nil;
  }

  return mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
      .materialDesc.restitution;
}

void CollidableScriptingInterface::LuaInterface::setRestitution(
    float restitution) {
  if (!mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    Collidable collidable{};
    collidable.materialDesc.restitution = restitution;
    mScriptGlobals.entityDatabase.set(mEntity, collidable);
  } else {
    mScriptGlobals.entityDatabase.get<Collidable>(mEntity)
        .materialDesc.restitution = restitution;
  }
}

void CollidableScriptingInterface::LuaInterface::setBoxGeometry(float hx,
                                                                float hy,
                                                                float hz) {
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

void CollidableScriptingInterface::LuaInterface::setSphereGeometry(
    float radius) {
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

void CollidableScriptingInterface::LuaInterface::setCapsuleGeometry(
    float radius, float halfHeight) {
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

void CollidableScriptingInterface::LuaInterface::setPlaneGeometry() {
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
CollidableScriptingInterface::LuaInterface::sweep(float dx, float dy, float dz,
                                                  float distance) {
  CollisionHit hit{};
  auto result = mScriptGlobals.physicsSystem.sweep(
      mScriptGlobals.entityDatabase, mEntity, {dx, dy, dz}, distance, hit);

  if (result) {
    return {result, hit};
  }

  return {result, sol::nil};
}

void CollidableScriptingInterface::LuaInterface::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<Collidable>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<Collidable>(mEntity);
  }
}

void CollidableScriptingInterface::LuaInterface::create(
    sol::usertype<CollidableScriptingInterface::LuaInterface> usertype) {
  usertype["set_default_material"] = &LuaInterface::setDefaultMaterial;
  usertype["get_static_friction"] = &LuaInterface::getStaticFriction;
  usertype["set_static_friction"] = &LuaInterface::setStaticFriction;
  usertype["get_dynamic_friction"] = &LuaInterface::getDynamicFriction;
  usertype["set_dynamic_friction"] = &LuaInterface::setDynamicFriction;
  usertype["get_restitution"] = &LuaInterface::getRestitution;
  usertype["set_restitution"] = &LuaInterface::setRestitution;
  usertype["set_box_geometry"] = &LuaInterface::setBoxGeometry;
  usertype["set_sphere_geometry"] = &LuaInterface::setSphereGeometry;
  usertype["set_capsule_geometry"] = &LuaInterface::setCapsuleGeometry;
  usertype["set_plane_geometry"] = &LuaInterface::setPlaneGeometry;
  usertype["sweep"] = &LuaInterface::sweep;
  usertype["delete"] = &LuaInterface::deleteThis;
}

} // namespace quoll
