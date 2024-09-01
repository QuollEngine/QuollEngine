#include "quoll/core/Base.h"
#include "quoll/asset/AssetCache.h"
#include "quoll/asset/AssetHandle.h"
#include "quoll/renderer/TextureAsset.h"
#include "quoll/ui/UILuaTable.h"
#include "UILuaTable.h"
#include "YogaSerializer.h"

namespace quoll {

sol::table UILuaTable::create(sol::state_view state, AssetCache &assetCache) {
  auto uiImage = state.new_usertype<UIImage>("UIImage", sol::no_constructor);
  uiImage["texture"] = sol::property(
      [](UIImage &image) { return image.texture.handle().getRawId(); },
      [&](UIImage &image, AssetHandleType value) {
        AssetHandle<TextureAsset> handle(value);
        if (assetCache.getRegistry().has(handle)) {
          image.texture = AssetRef<TextureAsset>(
              assetCache.getRegistry().getMap<TextureAsset>(), handle);
        }
      });

  auto uiText = state.new_usertype<UIText>("UIText", sol::no_constructor);
  uiText["content"] = &UIText::content;

  auto uiView = state.new_usertype<UIView>("UIView", sol::no_constructor);
  uiView["children"] = &UIView::children;

  auto ui = state.create_table();

  ui["image"] = [&](sol::table props) {
    AssetHandle<TextureAsset> handle(props.get<AssetHandleType>("texture"));

    AssetRef<TextureAsset> texture;

    if (assetCache.getRegistry().has(handle)) {
      texture = AssetRef<TextureAsset>(
          assetCache.getRegistry().getMap<TextureAsset>(), handle);
    }

    return UIImage{.texture = texture};
  };

  ui["text"] = [](sol::table props) {
    UIText text{.content = props.get<String>("content")};

    return text;
  };

  ui["view"] = [](sol::table props) {
    UIView view{};

    auto style = props["style"];
    if (style.is<sol::table>()) {
      if (style["grow"].is<f32>()) {
        // TODO: Throw warning if value is less than 0
        view.style.grow = std::max(0.0f, style["grow"].get<f32>());
      }

      if (style["shrink"].is<f32>()) {
        // TODO: Throw warning if value is less than 0
        view.style.shrink = std::max(0.0f, style["shrink"].get<f32>());
      }

      if (style["direction"].is<String>()) {
        view.style.direction = getYogaDirectionFromString(style["direction"],
                                                          view.style.direction);
      }

      if (style["justifyContent"].is<String>()) {
        view.style.justifyContent = getYogaJustifyFromString(
            style["justifyContent"], view.style.justifyContent);
      }

      if (style["alignContent"].is<String>()) {
        view.style.alignContent = getYogaAlignFromString(
            style["alignContent"], view.style.alignContent);
      }

      if (style["alignItems"].is<String>()) {
        view.style.alignItems =
            getYogaAlignFromString(style["alignItems"], view.style.alignItems);
      }

      static constexpr usize ColorNumComponents = 4;
      if (style["backgroundColor"]
              .is<std::array<float, ColorNumComponents>>()) {
        // TODO: Throw warning if color value is not in range [0.0, 1.0]
        auto color = style["backgroundColor"].get<std::array<float, 4>>();
        view.style.backgroundColor = {glm::clamp(color.at(0), 0.0f, 1.0f),
                                      glm::clamp(color.at(1), 0.0f, 1.0f),
                                      glm::clamp(color.at(2), 0.0f, 1.0f),
                                      glm::clamp(color.at(3), 0.0f, 1.0f)};
      }
    }

    auto children = props["children"];
    if (!children.is<sol::table>()) {
      return view;
    }

    for (auto [k, v] : children.get<sol::table>()) {
      if (v.is<UIImage>()) {
        view.children.push_back(v.as<UIImage>());
      } else if (v.is<UIText>()) {
        view.children.push_back(v.as<UIText>());
      } else if (v.is<UIView>()) {
        view.children.push_back(v.as<UIView>());
      }
    }

    return view;
  };

  return ui;
}

} // namespace quoll
