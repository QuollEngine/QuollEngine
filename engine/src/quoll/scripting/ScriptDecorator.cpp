#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/entity/EntityQueryScriptingInterface.h"
#include "quoll/entity/EntitySpawnerScriptingInterface.h"
#include "quoll/core/NameScriptingInterface.h"
#include "quoll/physics/RigidBodyScriptingInterface.h"
#include "quoll/physics/CollidableScriptingInterface.h"
#include "quoll/audio/AudioScriptingInterface.h"
#include "quoll/scene/TransformScriptingInterface.h"
#include "quoll/scene/PerspectiveLensScriptingInterface.h"
#include "quoll/text/TextScriptingInterface.h"
#include "quoll/animation/AnimatorScriptingInterface.h"
#include "quoll/input/InputMapScriptingInterface.h"

#include "LuaMessages.h"
#include "LuaScope.h"
#include "ScriptDecorator.h"
#include "ScriptLogger.h"

namespace quoll {

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
  auto table = scope.createTable(3);
  table.set("id", entity);

  registerEntityInterface<NameScriptingInterface>(scope, table, entity);
  registerEntityInterface<TransformScriptingInterface>(scope, table, entity);
  registerEntityInterface<PerspectiveLensScriptingInterface>(scope, table,
                                                             entity);
  registerEntityInterface<RigidBodyScriptingInterface>(scope, table, entity);
  registerEntityInterface<CollidableScriptingInterface>(scope, table, entity);
  registerEntityInterface<AudioScriptingInterface>(scope, table, entity);
  registerEntityInterface<TextScriptingInterface>(scope, table, entity);
  registerEntityInterface<AnimatorScriptingInterface>(scope, table, entity);
  registerEntityInterface<InputMapScriptingInterface>(scope, table, entity);
}

void ScriptDecorator::attachToScope(LuaScope &scope, Entity entity,
                                    EntityDatabase &entityDatabase,
                                    AssetRegistry &assetRegistry) {
  scope.setGlobal<LuaUserData>("__privateDatabase",
                               {static_cast<void *>(&entityDatabase)});
  scope.setGlobal<LuaUserData>("__privateAssetRegistry",
                               {static_cast<void *>(&assetRegistry)});

  createEntityTable(scope, entity);
  scope.setPreviousValueAsGlobal("entity");

  createInterfaceTable<EntityQueryScriptingInterface::LuaInterface>(scope);
  scope.setPreviousValueAsGlobal("entity_query");

  createInterfaceTable<EntitySpawnerScriptingInterface::LuaInterface>(scope);
  scope.setPreviousValueAsGlobal("entity_spawner");

  createScriptLogger(scope);
  scope.setPreviousValueAsGlobal("logger");
}

void ScriptDecorator::attachVariableInjectors(
    LuaScope &scope,
    std::unordered_map<String, LuaScriptInputVariable> &variables) {
  scope.setGlobal<LuaUserData>("__privateVariables",
                               {static_cast<void *>(&variables)});
  auto table = scope.createTable(1);

  table.set("register", [](void *state) {
    LuaScope scope(state);
    if (!scope.is<String>(1) || !scope.is<uint32_t>(2)) {
      scope.error(LuaMessages::invalidArguments<String, uint32_t>("input_vars",
                                                                  "register"));
      return 0;
    }

    auto name = scope.get<String>(1);
    auto type = scope.get<uint32_t>(2);

    if (type >= static_cast<uint32_t>(LuaScriptVariableType::Invalid)) {
      scope.error("Variable \"" + name + "\" has invalid type");
      return 0;
    }

    auto &data =
        *static_cast<std::unordered_map<String, LuaScriptInputVariable> *>(
            scope.getGlobal<LuaUserData>("__privateVariables").pointer);

    auto value = data.at(name);
    if (value.isType(LuaScriptVariableType::String)) {
      scope.set(value.get<String>());
    } else if (value.isType(LuaScriptVariableType::AssetPrefab)) {
      scope.set(static_cast<uint32_t>(value.get<PrefabAssetHandle>()));
    } else if (value.isType(LuaScriptVariableType::AssetTexture)) {
      scope.set(static_cast<uint32_t>(value.get<TextureAssetHandle>()));
    }

    return 1;
  });

  auto typesTable = scope.createTable(3);
  typesTable.set("Invalid",
                 static_cast<uint32_t>(LuaScriptVariableType::Invalid));
  typesTable.set("String",
                 static_cast<uint32_t>(LuaScriptVariableType::String));
  typesTable.set("AssetPrefab",
                 static_cast<uint32_t>(LuaScriptVariableType::AssetPrefab));
  typesTable.set("AssetTexture",
                 static_cast<uint32_t>(LuaScriptVariableType::AssetTexture));
  table.set("types", typesTable);
  scope.setPreviousValueAsGlobal("input_vars");
}

void ScriptDecorator::removeVariableInjectors(LuaScope &scope) {
  scope.setGlobal("input_vars", nullptr);
  scope.setGlobal("__privateVariables", nullptr);
}

} // namespace quoll
