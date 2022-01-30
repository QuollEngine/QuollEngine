#include "liquid/core/Base.h"
#include "HardwareBuffer.h"
#include "liquid/profiler/StatsManager.h"

namespace liquid {

HardwareBuffer::HardwareBuffer(HardwareBufferType type, size_t bufferSize_,
                               StatsManager &statsManager_)
    : bufferType(type), bufferSize(bufferSize_), statsManager(statsManager_) {
  statsManager.addBuffer(bufferSize);
}

HardwareBuffer::~HardwareBuffer() { statsManager.removeBuffer(bufferSize); }

} // namespace liquid
