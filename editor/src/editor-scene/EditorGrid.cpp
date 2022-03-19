#include "liquid/core/Base.h"
#include "EditorGrid.h"

namespace liquidator {

EditorGrid::EditorGrid(liquid::rhi::ResourceRegistry &registry)
    : mRegistry(registry) {
  mBuffer = mRegistry.setBuffer(
      {liquid::rhi::BufferType::Uniform, sizeof(EditorGridData), &mData});
  updateBuffer();
}

void EditorGrid::setGridLinesFlag(bool flag) {
  mData.gridLines.x = static_cast<uint32_t>(flag);
  updateBuffer();
}

void EditorGrid::setAxisLinesFlag(bool flag) {
  mData.gridLines.y = static_cast<uint32_t>(flag);
  updateBuffer();
}

void EditorGrid::updateBuffer() {
  mRegistry.setBuffer(
      {liquid::rhi::BufferType::Uniform, sizeof(EditorGridData), &mData},
      mBuffer);
}

} // namespace liquidator
