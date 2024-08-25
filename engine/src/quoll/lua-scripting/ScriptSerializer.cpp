#include "quoll/core/Base.h"
#include "LuaScript.h"
#include "ScriptSerializer.h"

namespace quoll {

void ScriptSerializer::serialize(YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 AssetRegistry &assetRegistry) {
  if (entityDatabase.has<LuaScript>(entity)) {
    const auto &script = entityDatabase.get<LuaScript>(entity);
    if (assetRegistry.has(script.handle)) {
      const auto &asset = assetRegistry.get(script.handle);

      auto uuid = asset.uuid;

      node["script"]["asset"] = uuid;

      for (auto &[name, value] : script.variables) {
        auto it = asset.data.variables.find(name);
        if (it == asset.data.variables.end() ||
            !value.isType(it->second.type)) {
          continue;
        }

        if (value.isType(LuaScriptVariableType::String)) {
          node["script"]["variables"][name]["type"] = "string";
          node["script"]["variables"][name]["value"] = value.get<String>();
        } else if (value.isType(LuaScriptVariableType::AssetPrefab)) {
          auto handle = value.get<AssetHandle<PrefabAsset>>();
          if (assetRegistry.has(handle)) {
            auto uuid = assetRegistry.get(handle).uuid;

            node["script"]["variables"][name]["type"] = "prefab";
            node["script"]["variables"][name]["value"] = uuid;
          }
        } else if (value.isType(LuaScriptVariableType::AssetTexture)) {
          auto handle = value.get<AssetHandle<TextureAsset>>();
          if (assetRegistry.has(handle)) {
            auto uuid = assetRegistry.get(handle).uuid;

            node["script"]["variables"][name]["type"] = "texture";
            node["script"]["variables"][name]["value"] = uuid;
          }
        }
      }
    }
  }
}

void ScriptSerializer::deserialize(const YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity,
                                   AssetRegistry &assetRegistry) {
  if (node["script"]) {
    LuaScript script{};
    Uuid uuid;
    if (node["script"].IsScalar()) {
      uuid = node["script"].as<Uuid>(Uuid{});
    } else if (node["script"].IsMap()) {
      uuid = node["script"]["asset"].as<Uuid>(Uuid{});

      if (node["script"]["variables"] && node["script"]["variables"].IsMap()) {
        for (const auto &var : node["script"]["variables"]) {
          if (!var.second.IsMap()) {
            continue;
          }
          auto name = var.first.as<String>("");
          auto type = var.second["type"].as<String>("");
          auto value = var.second["value"].as<String>("");

          if (type == "string") {
            script.variables.insert_or_assign(name, value);
          } else if (type == "prefab") {
            auto handle =
                assetRegistry.findHandleByUuid<PrefabAsset>(Uuid(value));
            if (handle) {
              script.variables.insert_or_assign(name, handle);
            }
          } else if (type == "texture") {
            auto handle =
                assetRegistry.findHandleByUuid<TextureAsset>(Uuid(value));
            if (handle) {
              script.variables.insert_or_assign(name, handle);
            }
          }
        }
      }
    }

    script.handle = assetRegistry.findHandleByUuid<LuaScriptAsset>(uuid);

    if (script.handle) {
      entityDatabase.set(entity, script);
    }
  }
}

} // namespace quoll
