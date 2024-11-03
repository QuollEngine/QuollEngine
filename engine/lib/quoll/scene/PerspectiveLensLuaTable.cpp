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
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.near;
}

void PerspectiveLensLuaTable::setNear(f32 near) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.near = near;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).near = near;
  }
}

sol_maybe<f32> PerspectiveLensLuaTable::getFar() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.far;
}

void PerspectiveLensLuaTable::setFar(f32 far) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.far = far;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).far = far;
  }
}

std::tuple<sol_maybe<f32>, sol_maybe<f32>>
PerspectiveLensLuaTable::getSensorSize() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return {sol::nil, sol::nil};
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return {lens.sensorSize.x, lens.sensorSize.y};
}

void PerspectiveLensLuaTable::setSensorSize(f32 width, f32 height) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.sensorSize = {width, height};
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).sensorSize = {
        width, height};
  }
}

sol_maybe<f32> PerspectiveLensLuaTable::getFocalLength() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.focalLength;
}

void PerspectiveLensLuaTable::setFocalLength(f32 focalLength) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.focalLength = focalLength;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).focalLength =
        focalLength;
  }
}

sol_maybe<f32> PerspectiveLensLuaTable::getAperture() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.aperture;
}

void PerspectiveLensLuaTable::setAperture(f32 aperture) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.aperture = aperture;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).aperture =
        aperture;
  }
}

sol_maybe<f32> PerspectiveLensLuaTable::getShutterSpeed() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return 1.0f / lens.shutterSpeed;
}

void PerspectiveLensLuaTable::setShutterSpeed(f32 shutterSpeed) {
  const f32 value = 1.0f / shutterSpeed;

  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.shutterSpeed = value;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).shutterSpeed =
        value;
  }
}

sol_maybe<u32> PerspectiveLensLuaTable::getSensitivity() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << lua::Messages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.sensitivity;
}

void PerspectiveLensLuaTable::setSensitivity(u32 sensitivity) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.sensitivity = sensitivity;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).sensitivity =
        sensitivity;
  }
}

void PerspectiveLensLuaTable::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<PerspectiveLens>(mEntity);
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
