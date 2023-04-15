#pragma once

#include "liquid/scripting/ComponentLuaInterface.h"

namespace liquid {

/**
 * @brief Scripting interfaces for entity spawner
 */
struct EntitySpawnerScriptingInterface {
public:
  class LuaInterface;
};

/**
 * @brief Lua scripting interface for entity spawner
 */
class EntitySpawnerScriptingInterface::LuaInterface
    : public ComponentLuaInterface<
          EntitySpawnerScriptingInterface::LuaInterface> {
public:
  /**
   * @brief Get first entity by name
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int spawnEmpty(void *state);

  /**
   * @brief Delete entity
   *
   * @param state Lua state
   * @return Number of arguments
   */
  static int spawnPrefab(void *state);

  /**
   * @brief Interface fields
   */
  static constexpr std::array<InterfaceField, 2> Fields{
      InterfaceField{"spawn_empty", spawnEmpty},
      InterfaceField{"spawn_prefab", spawnPrefab}};

  /**
   * @brief Get component name in scripts
   *
   * @return Component name
   */
  static const String getName() { return "entity_spawner"; }
};

} // namespace liquid
