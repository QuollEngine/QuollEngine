#pragma once

#include "../component/Component.h"
#include "../properties/Color.h"
#include "../reactive/Value.h"
#include "TextView.h"

namespace qui {

class Text : public Component {
  static constexpr u32 DefaultTextSize = 12;

public:
  Text(Value<quoll::String> text);

  Text &color(Value<Color> color);

  void build(BuildContext &context) override;

  constexpr View *getView() override { return &mView; }

public:
  constexpr auto getText() const { return mText(); }
  constexpr auto getColor() const { return mColor(); }

private:
  Value<quoll::String> mText;
  Value<Color> mColor;

  TextView mView;
};

} // namespace qui
