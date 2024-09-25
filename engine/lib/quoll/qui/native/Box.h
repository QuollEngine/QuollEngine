#pragma once

#include "../component/Component.h"
#include "../component/Element.h"
#include "../properties/Color.h"
#include "../properties/EdgeInsets.h"
#include "../reactive/Value.h"
#include "BoxView.h"

namespace qui {

/**
 * @brief A box with a child element
 *
 * @layout if no child is set and width or height is provided,
 *  - Box size = clamp(width or height, min constraint, max constraint)
 * @layout if no child is set and width or height is not provided,
 *  - Box size = max constraint
 * @layout if width or height is provided,
 *	- Child min and max constraint for axis = dimension - padding
 *	- Box size = clamp(dimension, min constraint, max constraint)
 * @layout if width or height not provided,
 *  - Child constraint for axis = input constraint - padding
 *	- Box size = clamp(child size, min constraint, max constraint)
 * @layout child position = position + padding start
 */
class Box : public Component {
public:
  Box(Value<Element> child);

  Box &padding(Value<EdgeInsets> padding);

  Box &background(Value<Color> color);

  Box &width(Value<f32> width);

  Box &height(Value<f32> height);

  Box &borderRadius(Value<f32> radius);

  void build() override;

  constexpr View *getView() { return &mView; }

public:
  inline auto getChild() { return mChild(); }
  constexpr auto getBackground() { return mBackground(); }
  constexpr auto getPadding() { return mPadding(); }
  constexpr auto getWidth() { return mWidth(); }
  constexpr auto getHeight() { return mHeight(); }
  constexpr auto getBorderRadius() { return mBorderRadius(); }

private:
  Value<Element> mChild;
  Value<Color> mBackground;
  Value<EdgeInsets> mPadding;
  Value<f32> mWidth{0};
  Value<f32> mHeight{0};
  Value<f32> mBorderRadius{0};

  BoxView mView;
};

} // namespace qui
