#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "PerspectiveLensLuaTable.h"

namespace quoll {

PerspectiveLensLuaTable::PerspectiveLensLuaTable(Entity entity,
                                                 ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<float> PerspectiveLensLuaTable::getNear() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.near;
}

void PerspectiveLensLuaTable::setNear(float near) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.near = near;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).near = near;
  }
}

sol_maybe<float> PerspectiveLensLuaTable::getFar() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.far;
}

void PerspectiveLensLuaTable::setFar(float far) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.far = far;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).far = far;
  }
}

std::tuple<sol_maybe<float>, sol_maybe<float>>
PerspectiveLensLuaTable::getSensorSize() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return {sol::nil, sol::nil};
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return {lens.sensorSize.x, lens.sensorSize.y};
}

void PerspectiveLensLuaTable::setSensorSize(float width, float height) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.sensorSize = {width, height};
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).sensorSize = {
        width, height};
  }
}

sol_maybe<float> PerspectiveLensLuaTable::getFocalLength() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.focalLength;
}

void PerspectiveLensLuaTable::setFocalLength(float focalLength) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.focalLength = focalLength;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).focalLength =
        focalLength;
  }
}

sol_maybe<float> PerspectiveLensLuaTable::getAperture() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.aperture;
}

void PerspectiveLensLuaTable::setAperture(float aperture) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.aperture = aperture;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).aperture =
        aperture;
  }
}

sol_maybe<float> PerspectiveLensLuaTable::getShutterSpeed() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return 1.0f / lens.shutterSpeed;
}

void PerspectiveLensLuaTable::setShutterSpeed(float shutterSpeed) {
  float value = 1.0f / shutterSpeed;

  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.shutterSpeed = value;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).shutterSpeed =
        value;
  }
}

sol_maybe<uint32_t> PerspectiveLensLuaTable::getSensitivity() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.sensitivity;
}

void PerspectiveLensLuaTable::setSensitivity(uint32_t sensitivity) {
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
    sol::usertype<PerspectiveLensLuaTable> usertype) {
  usertype["get_near"] = &PerspectiveLensLuaTable::getNear;
  usertype["set_near"] = &PerspectiveLensLuaTable::setNear;
  usertype["get_far"] = &PerspectiveLensLuaTable::getFar;
  usertype["set_far"] = &PerspectiveLensLuaTable::setFar;
  usertype["get_sensor_size"] = &PerspectiveLensLuaTable::getSensorSize;
  usertype["set_sensor_size"] = &PerspectiveLensLuaTable::setSensorSize;
  usertype["get_focal_length"] = &PerspectiveLensLuaTable::getFocalLength;
  usertype["set_focal_length"] = &PerspectiveLensLuaTable::setFocalLength;
  usertype["get_aperture"] = &PerspectiveLensLuaTable::getAperture;
  usertype["set_aperture"] = &PerspectiveLensLuaTable::setAperture;
  usertype["get_shutter_speed"] = &PerspectiveLensLuaTable::getShutterSpeed;
  usertype["set_shutter_speed"] = &PerspectiveLensLuaTable::setShutterSpeed;
  usertype["get_sensitivity"] = &PerspectiveLensLuaTable::getSensitivity;
  usertype["set_sensitivity"] = &PerspectiveLensLuaTable::setSensitivity;
  usertype["delete"] = &PerspectiveLensLuaTable::deleteThis;
}

} // namespace quoll
