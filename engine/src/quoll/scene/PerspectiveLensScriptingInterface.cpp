#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityDatabase.h"
#include "quoll/scripting/LuaMessages.h"

#include "PerspectiveLensScriptingInterface.h"

namespace quoll {

PerspectiveLensScriptingInterface::LuaInterface::LuaInterface(
    Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

sol_maybe<float> PerspectiveLensScriptingInterface::LuaInterface::getNear() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.near;
}

void PerspectiveLensScriptingInterface::LuaInterface::setNear(float near) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.near = near;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).near = near;
  }
}

sol_maybe<float> PerspectiveLensScriptingInterface::LuaInterface::getFar() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.far;
}

void PerspectiveLensScriptingInterface::LuaInterface::setFar(float far) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.far = far;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).far = far;
  }
}

std::tuple<sol_maybe<float>, sol_maybe<float>>
PerspectiveLensScriptingInterface::LuaInterface::getSensorSize() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return {sol::nil, sol::nil};
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return {lens.sensorSize.x, lens.sensorSize.y};
}

void PerspectiveLensScriptingInterface::LuaInterface::setSensorSize(
    float width, float height) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.sensorSize = {width, height};
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).sensorSize = {
        width, height};
  }
}

sol_maybe<float>
PerspectiveLensScriptingInterface::LuaInterface::getFocalLength() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.focalLength;
}

void PerspectiveLensScriptingInterface::LuaInterface::setFocalLength(
    float focalLength) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.focalLength = focalLength;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).focalLength =
        focalLength;
  }
}

sol_maybe<float>
PerspectiveLensScriptingInterface::LuaInterface::getAperture() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.aperture;
}

void PerspectiveLensScriptingInterface::LuaInterface::setAperture(
    float aperture) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.aperture = aperture;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).aperture =
        aperture;
  }
}

sol_maybe<float>
PerspectiveLensScriptingInterface::LuaInterface::getShutterSpeed() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return 1.0f / lens.shutterSpeed;
}

void PerspectiveLensScriptingInterface::LuaInterface::setShutterSpeed(
    float shutterSpeed) {
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

sol_maybe<uint32_t>
PerspectiveLensScriptingInterface::LuaInterface::getSensitivity() {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), mEntity);

    return sol::nil;
  }

  const auto &lens =
      mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity);

  return lens.sensitivity;
}

void PerspectiveLensScriptingInterface::LuaInterface::setSensitivity(
    uint32_t sensitivity) {
  if (!mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    PerspectiveLens lens{};
    lens.sensitivity = sensitivity;
    mScriptGlobals.entityDatabase.set(mEntity, lens);
  } else {
    mScriptGlobals.entityDatabase.get<PerspectiveLens>(mEntity).sensitivity =
        sensitivity;
  }
}

void PerspectiveLensScriptingInterface::LuaInterface::deleteThis() {
  if (mScriptGlobals.entityDatabase.has<PerspectiveLens>(mEntity)) {
    mScriptGlobals.entityDatabase.remove<PerspectiveLens>(mEntity);
  }
}

void PerspectiveLensScriptingInterface::LuaInterface ::create(
    sol::usertype<PerspectiveLensScriptingInterface::LuaInterface> usertype) {
  usertype["get_near"] = &LuaInterface::getNear;
  usertype["set_near"] = &LuaInterface::setNear;
  usertype["get_far"] = &LuaInterface::getFar;
  usertype["set_far"] = &LuaInterface::setFar;
  usertype["get_sensor_size"] = &LuaInterface::getSensorSize;
  usertype["set_sensor_size"] = &LuaInterface::setSensorSize;
  usertype["get_focal_length"] = &LuaInterface::getFocalLength;
  usertype["set_focal_length"] = &LuaInterface::setFocalLength;
  usertype["get_aperture"] = &LuaInterface::getAperture;
  usertype["set_aperture"] = &LuaInterface::setAperture;
  usertype["get_shutter_speed"] = &LuaInterface::getShutterSpeed;
  usertype["set_shutter_speed"] = &LuaInterface::setShutterSpeed;
  usertype["get_sensitivity"] = &LuaInterface::getSensitivity;
  usertype["set_sensitivity"] = &LuaInterface::setSensitivity;
  usertype["delete"] = &LuaInterface::deleteThis;
}

} // namespace quoll
