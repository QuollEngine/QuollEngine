#include "quoll/core/Base.h"
#include "LuaScript.h"
#include "ScriptSerializer.h"

namespace quoll {

void ScriptSerializer::serialize(YAML::Node &node,
                                 EntityDatabase &entityDatabase,
                                 Entity entity) {
  if (entityDatabase.has<LuaScript>(entity)) {
    const auto &script = entityDatabase.get<LuaScript>(entity);
    if (script.asset) {
      const auto &asset = script.asset.get();

      node["script"]["asset"] = script.asset.meta().uuid;

      for (auto &[name, value] : script.variables) {
        auto it = asset.variables.find(name);
        if (it == asset.variables.end() || !value.isType(it->second.type)) {
          continue;
        }

        if (value.isType(LuaScriptVariableType::String)) {
          node["script"]["variables"][name]["type"] = "string";
          node["script"]["variables"][name]["value"] = value.get<String>();
        } else if (value.isType(LuaScriptVariableType::AssetPrefab)) {
          const auto &prefab = value.get<AssetRef<PrefabAsset>>();
          if (prefab) {

            node["script"]["variables"][name]["type"] = "prefab";
            node["script"]["variables"][name]["value"] = prefab.meta().uuid;
          }
        } else if (value.isType(LuaScriptVariableType::AssetTexture)) {
          const auto &texture = value.get<AssetRef<TextureAsset>>();
          if (texture) {
            node["script"]["variables"][name]["type"] = "texture";
            node["script"]["variables"][name]["value"] = texture.meta().uuid;
          }
        }
      }
    }
  }
}

void ScriptSerializer::deserialize(const YAML::Node &node,
                                   EntityDatabase &entityDatabase,
                                   Entity entity, AssetCache &assetCache) {
  if (node["script"]) {
    Uuid uuid;
    if (node["script"].IsScalar()) {
      uuid = node["script"].as<Uuid>(Uuid{});
    } else if (node["script"].IsMap()) {
      uuid = node["script"]["asset"].as<Uuid>(Uuid{});
    }

    auto asset = assetCache.request<LuaScriptAsset>(uuid);
    if (!asset) {
      return;
    }

    LuaScript script{asset};

    if (node["script"].IsMap() && node["script"]["variables"] &&
        node["script"]["variables"].IsMap()) {
      for (const auto &var : node["script"]["variables"]) {
        if (!var.second.IsMap()) {
          continue;
        }
        auto name = var.first.as<String>("");
        auto type = var.second["type"].as<String>("");

        if (type == "string") {
          auto value = var.second["value"].as<String>("");
          script.variables.insert_or_assign(name, value);
        } else if (type == "prefab") {
          auto value = var.second["value"].as<Uuid>(Uuid{});
          auto prefab = assetCache.request<PrefabAsset>(value);

          if (prefab) {
            script.variables.insert_or_assign(name, prefab.data());
          }
        } else if (type == "texture") {
          auto value = var.second["value"].as<Uuid>(Uuid{});
          auto texture = assetCache.request<TextureAsset>(value);

          if (texture) {
            script.variables.insert_or_assign(name, texture.data());
          }
        }
      }
    }

    entityDatabase.set(entity, script);
  }
}

} // namespace quoll
