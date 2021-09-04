#include "HardwareBuffer.h"
#include "profiler/StatsManager.h"

namespace liquid {

HardwareBuffer::HardwareBuffer(HardwareBufferType type, size_t bufferSize_,
                               const SharedPtr<StatsManager> &statsManager_)
    : bufferType(type), bufferSize(bufferSize_), itemSize(0),
      statsManager(statsManager_) {
  if (statsManager) {
    statsManager->addBuffer(bufferSize);
  }
}

HardwareBuffer::HardwareBuffer(const std::vector<Vertex> &vertices,
                               const SharedPtr<StatsManager> &statsManager_)
    : bufferType(VERTEX), itemSize(vertices.size()),
      bufferSize(itemSize * sizeof(Vertex)), statsManager(statsManager_) {
  if (statsManager) {
    statsManager->addBuffer(bufferSize);
  }
}

HardwareBuffer::HardwareBuffer(const std::vector<uint32_t> &indices,
                               const SharedPtr<StatsManager> &statsManager_)
    : bufferType(INDEX), itemSize(indices.size()),
      bufferSize(itemSize * sizeof(uint32_t)), statsManager(statsManager_) {
  if (statsManager) {
    statsManager->addBuffer(bufferSize);
  }
}

HardwareBuffer::HardwareBuffer(size_t bufferSize_,
                               const SharedPtr<StatsManager> &statsManager_)
    : bufferSize(bufferSize_), itemSize(0), bufferType(UNIFORM),
      statsManager(statsManager_) {
  if (statsManager) {
    statsManager->addBuffer(bufferSize);
  }
}

HardwareBuffer::~HardwareBuffer() {
  if (statsManager) {
    statsManager->removeBuffer(bufferSize);
  }
}

} // namespace liquid
