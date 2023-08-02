#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/entity/EntityDatabase.h"

#include "liquid/scripting/LuaScope.h"
#include "liquid/scripting/LuaMessages.h"
#include "liquid/scripting/ComponentLuaInterfaceCommon.h"

#include "PerspectiveLensScriptingInterface.h"

namespace liquid {

int PerspectiveLensScriptingInterface::LuaInterface::getNear(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_near");

    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);

    scope.set(nullptr);
    return 1;
  }

  const auto &lens = entityDatabase.get<PerspectiveLens>(entity);

  scope.set(lens.near);

  return 1;
}

int PerspectiveLensScriptingInterface::LuaInterface::setNear(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_near");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float>(getName(), "set_near");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float value = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    PerspectiveLens lens{};
    lens.near = value;
    entityDatabase.set(entity, lens);
  } else {
    entityDatabase.get<PerspectiveLens>(entity).near = value;
  }

  return 0;
}

int PerspectiveLensScriptingInterface::LuaInterface::getFar(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_far");

    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);

    scope.set(nullptr);
    return 1;
  }

  const auto &lens = entityDatabase.get<PerspectiveLens>(entity);

  scope.set(lens.far);

  return 1;
}

int PerspectiveLensScriptingInterface::LuaInterface::setFar(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_far");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float>(getName(), "set_far");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float value = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    PerspectiveLens lens{};
    lens.far = value;
    entityDatabase.set(entity, lens);
  } else {
    entityDatabase.get<PerspectiveLens>(entity).far = value;
  }

  return 0;
}

int PerspectiveLensScriptingInterface::LuaInterface::getSensorSize(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_sensor_size");
    scope.set(nullptr);
    scope.set(nullptr);
    return 2;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);

    scope.set(nullptr);
    scope.set(nullptr);
    return 2;
  }

  const auto &lens = entityDatabase.get<PerspectiveLens>(entity);
  scope.set(lens.sensorSize.x);
  scope.set(lens.sensorSize.y);

  return 2;
}

int PerspectiveLensScriptingInterface::LuaInterface::setSensorSize(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_sensor_size");

    return 0;
  }

  if (!scope.is<float>(2) || !scope.is<float>(3)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float, float>(getName(),
                                                       "set_sensor_size");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  glm::vec2 value;
  value.x = scope.get<float>(2);
  value.y = scope.get<float>(3);
  scope.pop(3);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    PerspectiveLens lens{};
    lens.sensorSize = value;
    entityDatabase.set(entity, lens);
  } else {
    entityDatabase.get<PerspectiveLens>(entity).sensorSize = value;
  }

  return 0;
}

int PerspectiveLensScriptingInterface::LuaInterface::getFocalLength(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_focal_length");
    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);

    scope.set(nullptr);
    return 1;
  }

  const auto &lens = entityDatabase.get<PerspectiveLens>(entity);
  scope.set(lens.focalLength);

  return 1;
}

int PerspectiveLensScriptingInterface::LuaInterface::setFocalLength(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_focal_length");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float>(getName(), "set_focal_length");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float value = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    PerspectiveLens lens{};
    lens.focalLength = value;
    entityDatabase.set(entity, lens);
  } else {
    entityDatabase.get<PerspectiveLens>(entity).focalLength = value;
  }

  return 0;
}

int PerspectiveLensScriptingInterface::LuaInterface::getAperture(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_aperture");

    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);

    scope.set(nullptr);
    return 1;
  }

  const auto &lens = entityDatabase.get<PerspectiveLens>(entity);
  scope.set(lens.aperture);

  return 1;
}

int PerspectiveLensScriptingInterface::LuaInterface::setAperture(void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_aperture");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float>(getName(), "set_aperture");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float value = scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    PerspectiveLens lens{};
    lens.aperture = value;
    entityDatabase.set(entity, lens);
  } else {
    entityDatabase.get<PerspectiveLens>(entity).aperture = value;
  }

  return 0;
}

int PerspectiveLensScriptingInterface::LuaInterface::getShutterSpeed(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_shutter_speed");

    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);

    scope.set(nullptr);
    return 1;
  }

  const auto &lens = entityDatabase.get<PerspectiveLens>(entity);
  scope.set(1.0f / lens.shutterSpeed);

  return 1;
}

int PerspectiveLensScriptingInterface::LuaInterface::setShutterSpeed(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_shutter_speed");

    return 0;
  }

  if (!scope.is<float>(2)) {
    Engine::getUserLogger().error()
        << LuaMessages::invalidArguments<float>(getName(), "set_shutter_speed");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  float value = 1.0f / scope.get<float>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    PerspectiveLens lens{};
    lens.shutterSpeed = value;
    entityDatabase.set(entity, lens);
  } else {
    entityDatabase.get<PerspectiveLens>(entity).shutterSpeed = value;
  }

  return 0;
}

int PerspectiveLensScriptingInterface::LuaInterface::getSensitivity(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "get_sensitivity");

    scope.set(nullptr);
    return 1;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    Engine::getUserLogger().error()
        << LuaMessages::componentDoesNotExist(getName(), entity);

    scope.set(nullptr);
    return 1;
  }

  const auto &lens = entityDatabase.get<PerspectiveLens>(entity);
  scope.set(lens.sensitivity);

  return 1;
}

int PerspectiveLensScriptingInterface::LuaInterface::setSensitivity(
    void *state) {
  LuaScope scope(state);

  if (!scope.is<LuaTable>(1)) {
    Engine::getUserLogger().error()
        << LuaMessages::noEntityTable(getName(), "set_sensitivity");

    return 0;
  }

  if (!scope.is<uint32_t>(2)) {
    Engine::getUserLogger().error() << LuaMessages::invalidArguments<uint32_t>(
        getName(), "set_sensitivity");

    return 0;
  }

  auto entityTable = scope.get<LuaTable>(1);
  entityTable.get("id");
  Entity entity = scope.get<Entity>();
  scope.pop(1);

  uint32_t value = scope.get<uint32_t>(2);
  scope.pop(2);

  EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
      scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

  if (!entityDatabase.has<PerspectiveLens>(entity)) {
    PerspectiveLens lens{};
    lens.sensitivity = value;
    entityDatabase.set(entity, lens);
  } else {
    entityDatabase.get<PerspectiveLens>(entity).sensitivity = value;
  }

  return 0;
}

int PerspectiveLensScriptingInterface::LuaInterface::deleteThis(void *state) {
  return ComponentLuaInterfaceCommon::deleteComponent<PerspectiveLens>(
      getName(), state);
}

} // namespace liquid
