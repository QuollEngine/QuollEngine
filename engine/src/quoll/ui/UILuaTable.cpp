#include "quoll/core/Base.h"
#include "quoll/ui/UILuaTable.h"
#include "UILuaTable.h"

namespace quoll {

static YGFlexDirection getYogaDirectionFromString(String direction) {
  if (direction == "row") {
    return YGFlexDirectionRow;
  }

  if (direction == "row-reverse") {
    return YGFlexDirectionRowReverse;
  }

  if (direction == "column-reverse") {
    return YGFlexDirectionColumnReverse;
  }

  return YGFlexDirectionColumn;
}

sol::table UILuaTable::create(sol::state_view state) {
  auto uiImage = state.new_usertype<UIImage>("UIImage", sol::no_constructor);
  uiImage["texture"] = &UIImage::texture;

  auto uiText = state.new_usertype<UIText>("UIText", sol::no_constructor);
  uiText["content"] = &UIText::content;

  auto uiView = state.new_usertype<UIView>("UIView", sol::no_constructor);
  uiView["children"] = &UIView::children;

  auto ui = state.create_table();

  ui["image"] = [](sol::table props) {
    return UIImage{.texture = props.get<TextureAssetHandle>("texture")};
  };

  ui["text"] = [](sol::table props) {
    return UIText{.content = props.get<String>("content")};
  };

  ui["view"] = [](sol::table props) {
    UIView view{};

    auto style = props["style"];
    if (style.is<sol::table>()) {
      if (style["flexDirection"].is<String>()) {
        view.flexDirection = getYogaDirectionFromString(style["flexDirection"]);
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
