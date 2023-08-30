#include "liquid/core/Base.h"

#include "TinyGLTF.h"
#include "Buffer.h"

namespace quoll::editor {

BufferMeta getBufferMetaForAccessor(const tinygltf::Model &model,
                                    int accessorIndex) {
  auto accessor = model.accessors.at(accessorIndex);
  auto bufferView = model.bufferViews.at(accessor.bufferView);
  const unsigned char *bufferStart =
      &model.buffers.at(bufferView.buffer).data.at(0);
  const unsigned char *bufferOffset =
      bufferStart + accessor.byteOffset + bufferView.byteOffset;

  return {accessor, bufferView, bufferOffset};
}

} // namespace quoll::editor
