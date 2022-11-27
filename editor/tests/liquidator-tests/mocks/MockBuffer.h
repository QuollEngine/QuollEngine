#pragma once

#include "liquid/rhi/NativeBuffer.h"
#include "liquid/rhi/BufferDescription.h"

class MockBuffer : public liquid::rhi::NativeBuffer {
public:
  MockBuffer(const liquid::rhi::BufferDescription &description);

  void *map();

  void unmap();

  void update(void *data_);

  void resize(size_t size);

  const void *getData() const;

  const size_t getSize() const;

public:
  void *mData = nullptr;
  liquid::rhi::BufferDescription mDescription;
};
