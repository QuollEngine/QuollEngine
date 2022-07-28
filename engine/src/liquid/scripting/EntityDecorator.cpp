#include "liquid/core/Base.h"
#include "EntityDecorator.h"

#include "liquid/entity/EntityQueryScriptingInterface.h"
#include "liquid/physics/RigidBodyScriptingInterface.h"
#include "liquid/audio/AudioScriptingInterface.h"

#include "LuaScope.h"

namespace liquid {

/**
 * @brief Create interface table
 *
 * @tparam TLuaInterface Lua interface
 * @param scope Lua scope
 * @return Lua table
 */
template <class TLuaInterface> LuaTable createInterfaceTable(LuaScope &scope) {
  constexpr auto tableSize =
      static_cast<uint32_t>(sizeof(TLuaInterface::fields));
  auto table = scope.createTable(tableSize + 1);

  for (auto &field : TLuaInterface::fields) {
    table.set(field.key, field.fn);
  }

  return table;
}

/**
 * @brief Registry component with scope
 *
 * @tparam TComponent Component to registry
 * @param scope Lua scope
 * @param table Main table
 * @param entity Entity
 */
template <class TInterface>
void registerEntityInterface(LuaScope &scope, LuaTable &table, Entity entity) {
  using TLuaInterface = typename TInterface::LuaInterface;

  auto componentTable = createInterfaceTable<TLuaInterface>(scope);
  componentTable.set("id", entity);

  table.set(TLuaInterface::getName().c_str(), componentTable);
}

void EntityDecorator::createEntityTable(LuaScope &scope, Entity entity) {
  auto table = scope.createTable(2);
  table.set("id", entity);

  registerEntityInterface<NameComponent>(scope, table, entity);
  registerEntityInterface<LocalTransformComponent>(scope, table, entity);
  registerEntityInterface<RigidBodyScriptingInterface>(scope, table, entity);
  registerEntityInterface<AudioScriptingInterface>(scope, table, entity);
}

void EntityDecorator::attachToScope(LuaScope &scope, Entity entity,
                                    EntityDatabase &entityDatabase) {
  scope.setGlobal<LuaUserData>("__privateDatabase",
                               {static_cast<void *>(&entityDatabase)});

  createEntityTable(scope, entity);
  scope.setPreviousValueAsGlobal("entity");

  createInterfaceTable<EntityQueryScriptingInterface::LuaInterface>(scope);
  scope.setPreviousValueAsGlobal("entity_query");
}

} // namespace liquid
