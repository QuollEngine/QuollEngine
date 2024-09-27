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
                         : input.constraints.max.x;
    mSize.y = mHeight > 0 ? input.constraints.clampHeight(mHeight)
                          : input.constraints.max.y;

    return {mSize};
  }

  Constraints childConstraints = input.constraints;
  Constraints constraints = input.constraints;

  if (mWidth > 0.0f) {
    auto width = input.constraints.clampWidth(mWidth);
    childConstraints.max.x =
        width - mPadding.horizontal.start - mPadding.horizontal.end;
    childConstraints.min.x = childConstraints.max.x;

    constraints.min.x = width;
    constraints.max.x = width;
  } else {
    childConstraints.max.x -=
        mPadding.horizontal.start + mPadding.horizontal.end;
  }

  if (mHeight > 0.0f) {
    auto height = input.constraints.clampHeight(mHeight);
    childConstraints.max.y =
        height - mPadding.vertical.start - mPadding.vertical.end;
    childConstraints.min.y = childConstraints.max.y;

    constraints.min.y = height;
    constraints.max.y = height;
  } else {
    childConstraints.max.y -= mPadding.vertical.start + mPadding.vertical.end;
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
