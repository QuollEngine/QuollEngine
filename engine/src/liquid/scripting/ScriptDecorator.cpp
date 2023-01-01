#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/core/NameScriptingInterface.h"
#include "liquid/entity/EntityQueryScriptingInterface.h"
#include "liquid/physics/RigidBodyScriptingInterface.h"
#include "liquid/audio/AudioScriptingInterface.h"
#include "liquid/scene/TransformScriptingInterface.h"
#include "liquid/text/TextScriptingInterface.h"

#include "LuaScope.h"
#include "ScriptDecorator.h"
#include "ScriptLogger.h"

namespace liquid {

/**
 * @brief Create interface table
 *
 * @tparam TLuaInterface Lua interface
 * @param scope Lua scope
 * @return Lua table
 */
template <class TLuaInterface> LuaTable createInterfaceTable(LuaScope &scope) {
  static constexpr auto TableSize =
      static_cast<uint32_t>(sizeof(TLuaInterface::Fields));
  auto table = scope.createTable(TableSize + 1);

  for (auto &field : TLuaInterface::Fields) {
    table.set(field.key, field.fn);
  }

  return table;
}

/**
 * @brief Register component with scope
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

void ScriptDecorator::createEntityTable(LuaScope &scope, Entity entity) {
  auto table = scope.createTable(2);
  table.set("id", entity);

  registerEntityInterface<NameScriptingInterface>(scope, table, entity);
  registerEntityInterface<TransformScriptingInterface>(scope, table, entity);
  registerEntityInterface<RigidBodyScriptingInterface>(scope, table, entity);
  registerEntityInterface<AudioScriptingInterface>(scope, table, entity);
  registerEntityInterface<TextScriptingInterface>(scope, table, entity);
}

void ScriptDecorator::attachToScope(LuaScope &scope, Entity entity,
                                    EntityDatabase &entityDatabase) {
  scope.setGlobal<LuaUserData>("__privateDatabase",
                               {static_cast<void *>(&entityDatabase)});

  createEntityTable(scope, entity);
  scope.setPreviousValueAsGlobal("entity");

  createInterfaceTable<EntityQueryScriptingInterface::LuaInterface>(scope);
  scope.setPreviousValueAsGlobal("entity_query");

  createScriptLogger(scope);
  scope.setPreviousValueAsGlobal("logger");
}

} // namespace liquid
