#include "quoll/core/Base.h"
#include "Text.h"
#include "TextSerializer.h"

namespace quoll {

void TextSerializer::serialize(YAML::Node &node, EntityDatabase &entityDatabase,
                               Entity entity, AssetRegistry &assetRegistry) {
  if (entity.has<Text>()) {
    auto text = entity.get_ref<Text>();

    if (!text->content.empty() &&
        assetRegistry.getFonts().hasAsset(text->font)) {
      auto font = assetRegistry.getFonts().getAsset(text->font).uuid;

      node["text"]["content"] = text->content;
      node["text"]["lineHeight"] = text->lineHeight;
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
        textComponent.content =
            node["text"]["content"].as<String>(textComponent.content);
      }

      textComponent.lineHeight =
          node["text"]["lineHeight"].as<f32>(textComponent.lineHeight);

      entity.set(textComponent);
    }
  }
}

} // namespace quoll
