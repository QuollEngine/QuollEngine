#pragma once

namespace quoll::editor {

struct BufferMeta {
  tinygltf::Accessor accessor;

  tinygltf::BufferView bufferView;

  const unsigned char *rawData;
};

BufferMeta getBufferMetaForAccessor(const tinygltf::Model &model,
                                    int accessorIndex);

} // namespace quoll::editor
