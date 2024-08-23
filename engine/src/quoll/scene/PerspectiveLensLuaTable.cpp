#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/entity/EntityDatabase.h"
#include "quoll/lua-scripting/Messages.h"
#include "PerspectiveLens.h"
#include "PerspectiveLensLuaTable.h"

namespace quoll {

PerspectiveLensLuaTable::PerspectiveLensLuaTable(Entity entity,
                                                 ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<f32> PerspectiveLensLuaTable::getNear() {
  if (!mEntity.has<PerspectiveLens>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  return mEntity.get_ref<PerspectiveLens>()->near;
}

void PerspectiveLensLuaTable::setNear(f32 near) {
  if (!mEntity.has<PerspectiveLens>()) {
    PerspectiveLens lens{};
    lens.near = near;
    mEntity.set(lens);
  } else {
    mEntity.get_ref<PerspectiveLens>()->near = near;
  }
}

sol_maybe<f32> PerspectiveLensLuaTable::getFar() {
  if (!mEntity.has<PerspectiveLens>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  return mEntity.get_ref<PerspectiveLens>()->far;
}

void PerspectiveLensLuaTable::setFar(f32 far) {
  if (!mEntity.has<PerspectiveLens>()) {
    PerspectiveLens lens{};
    lens.far = far;
    mEntity.set(lens);
  } else {
    mEntity.get_ref<PerspectiveLens>()->far = far;
  }
}

std::tuple<sol_maybe<f32>, sol_maybe<f32>>
PerspectiveLensLuaTable::getSensorSize() {
  if (!mEntity.has<PerspectiveLens>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return {sol::nil, sol::nil};
  }

  auto lens = mEntity.get_ref<PerspectiveLens>();

  return {lens->sensorSize.x, lens->sensorSize.y};
}

void PerspectiveLensLuaTable::setSensorSize(f32 width, f32 height) {
  if (!mEntity.has<PerspectiveLens>()) {
    PerspectiveLens lens{};
    lens.sensorSize = {width, height};
    mEntity.set(lens);
  } else {
    mEntity.get_ref<PerspectiveLens>()->sensorSize = {width, height};
  }
}

sol_maybe<f32> PerspectiveLensLuaTable::getFocalLength() {
  if (!mEntity.has<PerspectiveLens>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  return mEntity.get_ref<PerspectiveLens>()->focalLength;
}

void PerspectiveLensLuaTable::setFocalLength(f32 focalLength) {
  if (!mEntity.has<PerspectiveLens>()) {
    PerspectiveLens lens{};
    lens.focalLength = focalLength;
    mEntity.set(lens);
  } else {
    mEntity.get_ref<PerspectiveLens>()->focalLength = focalLength;
  }
}

sol_maybe<f32> PerspectiveLensLuaTable::getAperture() {
  if (!mEntity.has<PerspectiveLens>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  return mEntity.get_ref<PerspectiveLens>()->aperture;
}

void PerspectiveLensLuaTable::setAperture(f32 aperture) {
  if (!mEntity.has<PerspectiveLens>()) {
    PerspectiveLens lens{};
    lens.aperture = aperture;
    mEntity.set(lens);
  } else {
    mEntity.get_ref<PerspectiveLens>()->aperture = aperture;
  }
}

sol_maybe<f32> PerspectiveLensLuaTable::getShutterSpeed() {
  if (!mEntity.has<PerspectiveLens>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  return 1.0f / mEntity.get_ref<PerspectiveLens>()->shutterSpeed;
}

void PerspectiveLensLuaTable::setShutterSpeed(f32 shutterSpeed) {
  f32 value = 1.0f / shutterSpeed;

  if (!mEntity.has<PerspectiveLens>()) {
    PerspectiveLens lens{};
    lens.shutterSpeed = value;
    mEntity.set(lens);
  } else {
    mEntity.get_ref<PerspectiveLens>()->shutterSpeed = value;
  }
}

sol_maybe<u32> PerspectiveLensLuaTable::getSensitivity() {
  if (!mEntity.has<PerspectiveLens>()) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  return mEntity.get_ref<PerspectiveLens>()->sensitivity;
}

void PerspectiveLensLuaTable::setSensitivity(u32 sensitivity) {
  if (!mEntity.has<PerspectiveLens>()) {
    PerspectiveLens lens{};
    lens.sensitivity = sensitivity;
    mEntity.set(lens);
  } else {
    mEntity.get_ref<PerspectiveLens>()->sensitivity = sensitivity;
  }
}

void PerspectiveLensLuaTable::deleteThis() {
  if (mEntity.has<PerspectiveLens>()) {
    mEntity.remove<PerspectiveLens>();
  }
}

void PerspectiveLensLuaTable ::create(
    sol::usertype<PerspectiveLensLuaTable> usertype, sol::state_view state) {
  usertype["near"] = sol::property(&PerspectiveLensLuaTable::getNear,
                                   &PerspectiveLensLuaTable::setNear);
  usertype["far"] = sol::property(&PerspectiveLensLuaTable::getFar,
                                  &PerspectiveLensLuaTable::setFar);
  usertype["getSensorSize"] = &PerspectiveLensLuaTable::getSensorSize;
  usertype["setSensorSize"] = &PerspectiveLensLuaTable::setSensorSize;

  usertype["focalLength"] =
      sol::property(&PerspectiveLensLuaTable::getFocalLength,
                    &PerspectiveLensLuaTable::setFocalLength);
  usertype["aperture"] = sol::property(&PerspectiveLensLuaTable::getAperture,
                                       &PerspectiveLensLuaTable::setAperture);
  usertype["shutterSpeed"] =
      sol::property(&PerspectiveLensLuaTable::getShutterSpeed,
                    &PerspectiveLensLuaTable::setShutterSpeed);
  usertype["sensitivity"] =
      sol::property(&PerspectiveLensLuaTable::getSensitivity,
                    &PerspectiveLensLuaTable::setSensitivity);

  usertype["delete"] = &PerspectiveLensLuaTable::deleteThis;
}

} // namespace quoll
