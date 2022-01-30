#include "liquid/core/Base.h"
#include "EditorGrid.h"

namespace liquidator {

EditorGrid::EditorGrid(liquid::ResourceAllocator *resourceAllocator) {
  buffer = resourceAllocator->createUniformBuffer(sizeof(EditorGridData));
  updateUniformBuffer();
}

void EditorGrid::setGridLinesFlag(bool flag) {
  data.gridLines.x = static_cast<uint32_t>(flag);
  updateUniformBuffer();
}

void EditorGrid::setAxisLinesFlag(bool flag) {
  data.gridLines.y = static_cast<uint32_t>(flag);
  updateUniformBuffer();
}

void EditorGrid::updateUniformBuffer() { buffer->update(&data); }

} // namespace liquidator
