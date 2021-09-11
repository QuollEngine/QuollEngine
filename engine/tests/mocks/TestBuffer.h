#pragma once

#include "renderer/HardwareBuffer.h"
#include "profiler/StatsManager.h"

class TestBuffer : public liquid::HardwareBuffer {
public:
  TestBuffer(HardwareBufferType type, size_t bufferSize,
             liquid::StatsManager &statsManager)
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
