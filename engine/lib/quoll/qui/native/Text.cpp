#include "quoll/core/Base.h"
#include "Text.h"

namespace qui {

Text::Text(Value<quoll::String> text) { mText = text; }

Text &Text::color(Value<Color> color) {
  mColor = color;
  return *this;
}

void Text::build() {
  auto observeText = [this] { mView.setText(mText()); };
  auto observeColor = [this] { mView.setColor(mColor()); };

  mText.observe(observeText);
  mColor.observe(observeColor);

  observeText();
  observeColor();
}

} // namespace qui
