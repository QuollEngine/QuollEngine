#include "liquid/core/Base.h"
#include "EntityDecorator.h"

#include "LuaScope.h"

namespace liquid {

/**
 * @brief Registry component with scope
 *
 * @tparam TComponent Component to registry
 * @param scope Lua scope
 * @param table Main table
 */
template <class TComponent>
void registerComponent(LuaScope &scope, LuaTable &table) {
  using TComponentLuaInterface = typename TComponent::LuaInterface;

  constexpr auto tableSize =
      static_cast<uint32_t>(sizeof(TComponentLuaInterface::fields));
  auto componentTable = scope.createTable(tableSize);

  for (auto &field : TComponentLuaInterface::fields) {
    componentTable.set(field.key, field.fn);
  }

  table.set(TComponentLuaInterface::getName().c_str(), componentTable);
}

void EntityDecorator::attachToScope(LuaScope &scope, Entity entity,
                                    EntityContext &entityContext) {
  scope.setGlobal<LuaUserData>("__privateContext",
                               {static_cast<void *>(&entityContext)});

  auto table = scope.createTable(2);
  table.set("id", entity);

  registerComponent<NameComponent>(scope, table);
  registerComponent<LocalTransformComponent>(scope, table);

  scope.setPreviousValueAsGlobal("entity");
}

} // namespace liquid
