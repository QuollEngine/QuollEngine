#include "liquid/core/Base.h"
#include "EditorGrid.h"

namespace liquidator {

EditorGrid::EditorGrid(liquid::rhi::ResourceRegistry &registry_)
    : registry(registry_) {
  buffer = registry.addBuffer(
      {liquid::rhi::BufferType::Uniform, sizeof(EditorGridData), &data});
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

void EditorGrid::updateUniformBuffer() {
  registry.updateBuffer(buffer, {liquid::rhi::BufferType::Uniform,
                                 sizeof(EditorGridData), &data});
}

} // namespace liquidator
