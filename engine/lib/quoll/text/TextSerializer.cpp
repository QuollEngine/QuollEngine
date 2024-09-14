#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "Text.h"
#include "TextSerializer.h"

namespace quoll {

void TextSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity) {
  if (entityDatabase.has<Text>(entity)) {
    const auto &text = entityDatabase.get<Text>(entity);

    if (!text.content.empty() && text.font) {
      node["text"]["content"] = text.content;
      node["text"]["lineHeight"] = text.lineHeight;
      node["text"]["font"] = text.font.meta().uuid;
    }
  }
}

void TextSerializer::deserialize(const YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 AssetCache &assetCache) {
  if (node["text"] && node["text"].IsMap()) {
    auto uuid = node["text"]["font"].as<Uuid>(Uuid{});

    auto font = assetCache.request<FontAsset>(uuid);

    if (font) {
      Text textComponent{};
      textComponent.font = font;

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
