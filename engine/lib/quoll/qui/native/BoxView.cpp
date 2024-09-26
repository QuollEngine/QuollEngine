#include "quoll/core/Base.h"
#include "BoxView.h"
#include <imgui.h>

namespace qui {

void BoxView::render() {
  if (mBackground != Color::Transparent) {
    auto *drawList = ImGui::GetWindowDrawList();

    drawList->AddRectFilled(
        ImVec2(mPosition.x, mPosition.y),
        ImVec2(mPosition.x + mSize.x, mPosition.y + mSize.y),
        ImColor(mBackground.value.x, mBackground.value.y, mBackground.value.z,
                mBackground.value.w),
        mBorderRadius);
  }

  if (mChild) {
    ImGui::PushClipRect({mPosition.x, mPosition.y},
                        {mPosition.x + mSize.x, mPosition.y + mSize.y}, true);
    mChild->render();
    ImGui::PopClipRect();
  }
}

LayoutOutput BoxView::layout(const LayoutInput &input) {
  mPosition = input.position;

  if (!mChild) {
    mSize.x = mWidth > 0 ? input.constraints.clampWidth(mWidth)
                         : input.constraints.maxWidth;
    mSize.y = mHeight > 0 ? input.constraints.clampHeight(mHeight)
                          : input.constraints.maxHeight;

    return {mSize};
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

  mSize.x = constraints.clampWidth(output.size.x + mPadding.horizontal.start +
                                   mPadding.horizontal.end);
  mSize.y = constraints.clampHeight(output.size.y + mPadding.vertical.start +
                                    mPadding.vertical.end);

  return {mSize};
}

} // namespace qui
