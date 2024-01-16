#include "quoll/core/Base.h"

#include "TextSerializer.h"
#include "Text.h"

namespace quoll {

void TextSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity, AssetRegistry &assetRegistry) {
  if (entityDatabase.has<Text>(entity)) {
    const auto &text = entityDatabase.get<Text>(entity);

    if (!text.text.empty() && assetRegistry.getFonts().hasAsset(text.font)) {
      auto font = assetRegistry.getFonts().getAsset(text.font).uuid;

      node["text"]["content"] = text.text;
      node["text"]["lineHeight"] = text.lineHeight;
      node["text"]["font"] = font;
    }
  }
}

void TextSerializer::deserialize(const YAML::Node &node,
                                 EntityDatabase &entityDatabase, Entity entity,
                                 AssetRegistry &assetRegistry) {
  if (node["text"] && node["text"].IsMap()) {
    auto uuid = node["text"]["font"].as<Uuid>(Uuid{});
    auto handle = assetRegistry.getFonts().findHandleByUuid(uuid);

    Text textComponent{};
    textComponent.font = handle;

    if (handle != FontAssetHandle::Null) {
      if (node["text"]["content"] && node["text"]["content"].IsScalar()) {
        textComponent.text =
            node["text"]["content"].as<String>(textComponent.text);
      }

      textComponent.lineHeight =
          node["text"]["lineHeight"].as<f32>(textComponent.lineHeight);

      entityDatabase.set(entity, textComponent);
    }
  }
}

} // namespace quoll
