#include "quoll/core/Base.h"
#include "FlexView.h"
#include <imgui.h>

namespace qui {

struct FlexLine {
  std::vector<View *> children;
  std::vector<glm::vec2> childSizes;
  f32 crossSize = 0.0f;
  f32 crossPos = 0.0f;
};

void FlexView::render() {
  ImGui::PushClipRect({mPosition.x, mPosition.y},
                      {mPosition.x + mSize.x, mPosition.y + mSize.y}, true);
  for (auto *child : mChildren) {
    child->render();
  }

  ImGui::PopClipRect();
}

LayoutOutput FlexView::layout(const LayoutInput &input) {
  const glm::length_t mainAxis = mDirection == Direction::Row ? 0 : 1;
  const glm::length_t crossAxis = mDirection == Direction::Row ? 1 : 0;

  mSize = {0.0f, 0.0f};

  std::vector<FlexLine> lines;

  if (mWrap == Wrap::NoWrap) {
    FlexLine line{.children = mChildren,
                  .childSizes = {},
                  .crossSize = 0.0f,
                  .crossPos = input.position[crossAxis]};

    for (auto *child : mChildren) {
      auto constraints = input.constraints;
      constraints.max[mainAxis] = Constraints::Infinity;
      auto output = child->layout({constraints, input.position});

      line.childSizes.emplace_back(output.size);
    }

    lines.emplace_back(line);
  } else if (mWrap == Wrap::Wrap) {
    f32 totalLineSize = 0.0f;

    FlexLine currentLine{.crossPos = input.position[crossAxis]};

    for (auto *child : mChildren) {
      auto output = child->layout({Constraints(), input.position});

      totalLineSize += output.size[mainAxis] + mSpacing[mainAxis];

      if (totalLineSize > input.constraints.max[mainAxis]) {
        totalLineSize = output.size[mainAxis] + mSpacing[mainAxis];
        lines.emplace_back(currentLine);
        currentLine.childSizes.emplace_back(output.size);

        currentLine = FlexLine{.children = {child},
                               .childSizes = {output.size},
                               .crossSize = output.size[crossAxis]};
      } else {
        currentLine.crossSize =
            std::max(output.size[crossAxis], currentLine.crossSize);

        currentLine.children.emplace_back(child);
        currentLine.childSizes.emplace_back(output.size);
      }
    }

    if (currentLine.children.size() > 0) {
      lines.emplace_back(currentLine);
    }

    for (usize i = 1; i < lines.size(); ++i) {
      lines.at(i).crossPos =
          lines.at(i - 1).crossPos + lines.at(i - 1).crossSize;
    }
  }

  for (usize li = 0; li < lines.size(); ++li) {
    auto &line = lines.at(li);
    std::vector<f32> shrinkFactors(line.children.size(), 1.0f);
    std::vector<f32> growFactors(line.children.size(), 0.0f);

    f32 totalCrossSize = 0.0f;
    f32 totalMainSize = 0.0f;
    f32 totalScaledShrink = 0.0f;
    f32 totalGrow = 0.0f;
    for (usize i = 0; i < line.children.size(); ++i) {
      auto *child = line.children.at(i);

      f32 shrink = 1.0f;
      f32 grow = 0.0f;
      if (auto *flex = dynamic_cast<FlexView *>(child)) {
        shrink = flex->mShrink;
        grow = flex->mGrow;
      }

      shrinkFactors.at(i) = shrink;
      growFactors.at(i) = grow;

      auto &childSize = line.childSizes.at(i);
      totalCrossSize = std::max(totalCrossSize, childSize[crossAxis]);
      totalMainSize += childSize[mainAxis];
      totalScaledShrink += childSize[mainAxis] * shrink;
      totalGrow += grow;
    }

    totalGrow = std::max(1.0f, totalGrow);
    totalScaledShrink = std::max(1.0f, totalScaledShrink);

    const f32 totalSpacing =
        mSpacing[mainAxis] * (static_cast<f32>(line.children.size() - 1));

    f32 overflow = 0.0f;
    f32 remaining = 0.0f;
    if (input.constraints.max[mainAxis] < Constraints::Infinity) {
      const f32 mainSizeWithoutSpacing =
          input.constraints.max[mainAxis] - totalSpacing;
      overflow = std::max(0.0f, totalMainSize - mainSizeWithoutSpacing);
      remaining = std::max(0.0f, mainSizeWithoutSpacing - totalMainSize);
    }

    glm::vec2 position = input.position;
    position[crossAxis] =
        line.crossPos + mSpacing[crossAxis] * static_cast<f32>(li);

    f32 lineMainSize = totalSpacing;
    for (usize i = 0; i < line.children.size(); ++i) {
      auto *child = line.children.at(i);

      const f32 childMainSize = line.childSizes.at(i)[mainAxis];
      const f32 childCrossSize = line.childSizes.at(i)[crossAxis];

      const f32 growAmount = (growFactors.at(i) * remaining) / totalGrow;
      const f32 shrinkAmount =
          (childMainSize * shrinkFactors.at(i) * overflow) / totalScaledShrink;
      const f32 mainSize = childMainSize + growAmount - shrinkAmount;

      Constraints constraints = input.constraints;
      constraints.min[mainAxis] = mainSize;
      constraints.max[mainAxis] = mainSize;
      constraints.min[crossAxis] = 0.0f;
      constraints.max[crossAxis] = Constraints::Infinity;
      child->layout({constraints, position});

      position[mainAxis] += mSpacing[mainAxis] + mainSize;

      lineMainSize += mainSize;
    }

    mSize[mainAxis] = std::max(lineMainSize, mSize[mainAxis]);
    mSize[crossAxis] += totalCrossSize;
  }

  mSize[crossAxis] += mSpacing[crossAxis] * static_cast<f32>(lines.size() - 1);

  mSize = input.constraints.clamp(mSize);
  mPosition = input.position;

  return {mSize};
}

View *FlexView::hitTest(const glm::vec2 &point) {
  if (point.x >= mPosition.x && point.x <= mPosition.x + mSize.x &&
      point.y >= mPosition.y && point.y <= mPosition.y + mSize.y) {
    for (auto *child : mChildren) {
      auto *hit = child->hitTest(point);
      if (hit) {
        return hit;
      }
    }
    return this;
  }

  return nullptr;
}

} // namespace qui
