#include "quoll/core/Base.h"
#include "BoxView.h"

namespace qui {

void BoxView::render() {
  if (mChild) {
    mChild->render();
  }
}

LayoutOutput BoxView::layout(const LayoutInput &input) {
  if (!mChild) {
    glm::vec2 size{};

    size.x = mWidth > 0 ? input.constraints.clampWidth(mWidth)
                        : input.constraints.maxWidth;
    size.y = mHeight > 0 ? input.constraints.clampHeight(mHeight)
                         : input.constraints.maxHeight;

    return {size};
  }

  Constraints childConstraints = input.constraints;
  Constraints constraints = input.constraints;

  if (mWidth > 0.0f) {
    auto width = input.constraints.clampWidth(mWidth);
    childConstraints.maxWidth =
        width - mPadding.horizontal.start - mPadding.horizontal.end;
    childConstraints.minWidth = childConstraints.maxWidth;

    constraints.minWidth = width;
    constraints.maxWidth = width;
  } else {
    childConstraints.maxWidth -=
        mPadding.horizontal.start + mPadding.horizontal.end;
  }

  if (mHeight > 0.0f) {
    auto height = input.constraints.clampHeight(mHeight);
    childConstraints.maxHeight =
        height - mPadding.vertical.start - mPadding.vertical.end;
    childConstraints.minHeight = childConstraints.maxHeight;

    constraints.minHeight = height;
    constraints.maxHeight = height;
  } else {
    childConstraints.maxHeight -=
        mPadding.vertical.start + mPadding.vertical.end;
  }

  glm::vec2 position = input.position;
  position.x += mPadding.horizontal.start;
  position.y += mPadding.vertical.start;

  auto output = mChild->layout({childConstraints, position});

  output.size.x = constraints.clampWidth(output.size.x);
  output.size.y = constraints.clampHeight(output.size.y);

  return output;
}

} // namespace qui
