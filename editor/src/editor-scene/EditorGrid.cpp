#include "liquid/core/Base.h"
#include "EditorGrid.h"

namespace liquidator {

void EditorGrid::setGridLinesFlag(bool flag) {
  mData.gridLines.x = static_cast<uint32_t>(flag);
}

void EditorGrid::setAxisLinesFlag(bool flag) {
  mData.gridLines.y = static_cast<uint32_t>(flag);
}

} // namespace liquidator
