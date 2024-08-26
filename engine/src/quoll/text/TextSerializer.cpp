#include "quoll/core/Base.h"
#include "Text.h"
#include "TextSerializer.h"

namespace quoll {

void TextSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity, AssetRegistry &assetRegistry) {
  if (entityDatabase.has<Text>(entity)) {
    const auto &text = entityDatabase.get<Text>(entity);

    if (!text.content.empty() && assetRegistry.has(text.font)) {
      node["text"]["content"] = text.content;
      node["text"]["lineHeight"] = text.lineHeight;
      node["text"]["font"] = assetRegistry.getMeta(text.font).uuid;
    }
  }
}

void TextSerializer::deserialize(const YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 AssetRegistry &assetRegistry) {
  if (node["text"] && node["text"].IsMap()) {
    auto uuid = node["text"]["font"].as<Uuid>(Uuid{});
    auto handle = assetRegistry.findHandleByUuid<FontAsset>(uuid);

    Text textComponent{};
    textComponent.font = handle;

    if (handle) {
      if (node["text"]["content"] && node["text"]["content"].IsScalar()) {
        textComponent.content =
            node["text"]["content"].as<String>(textComponent.content);
      }

      textComponent.lineHeight =
          node["text"]["lineHeight"].as<f32>(textComponent.lineHeight);

      entityDatabase.set(entity, textComponent);
    }
  }
}

} // namespace quoll
