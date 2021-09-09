#pragma once

#include "renderer/HardwareBuffer.h"
#include "profiler/StatsManager.h"

class TestBuffer : public liquid::HardwareBuffer {
public:
  TestBuffer(
      const std::vector<liquid::Vertex> &vertices,
      const liquid::SharedPtr<liquid::StatsManager> &statsManager = nullptr)
      : liquid::HardwareBuffer(vertices, statsManager) {}
  TestBuffer(
      const std::vector<uint32_t> &indices,
      const liquid::SharedPtr<liquid::StatsManager> &statsManager = nullptr)
      : liquid::HardwareBuffer(indices, statsManager) {}
  TestBuffer(
      size_t bufferSize,
      const liquid::SharedPtr<liquid::StatsManager> &statsManager = nullptr)
      : liquid::HardwareBuffer(bufferSize, statsManager) {}
  TestBuffer(
      HardwareBufferType type, size_t bufferSize,
      const liquid::SharedPtr<liquid::StatsManager> &statsManager = nullptr)
      : liquid::HardwareBuffer(type, bufferSize, statsManager) {}

  ~TestBuffer() {
    if (data) {
      delete[](char *) data;
    }
  }

  void update(void *data_) {
    if (!data) {
      data = new char[getBufferSize()];
    }
    memcpy(data, data_, getBufferSize());
  }

  void *map() {
    if (!data) {
      data = new char[getBufferSize()];
    }
    return data;
  }

  void unmap() {}

public:
  void *data = nullptr;
};
