#pragma once

namespace quoll {

/**
 * @brief Common functions for lua interface
 */
class ComponentLuaInterfaceCommon {
public:
  /**
   * @brief Generic delete component function
   *
   * @tparam TComponent Component
   * @param name Component name
   * @param state Lua state
   * @return Number of arguments
   */
  template <class TComponent>
  static int deleteComponent(const String &name, void *state) {
    LuaScope scope(state);

    if (!scope.is<LuaTable>(1)) {
      Engine::getUserLogger().error()
          << LuaMessages::noEntityTable(name, "delete");
      return 0;
    }

    auto entityTable = scope.get<LuaTable>(1);
    entityTable.get("id");
    Entity entity = scope.get<Entity>();
    scope.pop(1);

    EntityDatabase &entityDatabase = *static_cast<EntityDatabase *>(
        scope.getGlobal<LuaUserData>("__privateDatabase").pointer);

    if (!entityDatabase.has<TComponent>(entity)) {
      Engine::getUserLogger().error()
          << LuaMessages::componentDoesNotExist(name, entity);
      return 0;
    }

    entityDatabase.remove<TComponent>(entity);
    return 0;
  }
};

} // namespace quoll
