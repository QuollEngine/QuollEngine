#pragma once

#include "liquid/core/Base.h"
#include "liquid/scene/Vertex.h"

namespace liquid {

class StatsManager;

/**
 * @brief Abstract hardware buffer
 *
 * Stores vendor specific buffers for renderer to
 * bind during drawing
 */
class HardwareBuffer {
public:
  enum HardwareBufferType { Vertex, Index, Uniform, Transfer };

public:
  /**
   * @brief Default constructor
   *
   * @param type Buffer type
   * @param size Buffer size
   * @param statsManager Stats manager
   */
  HardwareBuffer(HardwareBufferType type, size_t bufferSize,
                 StatsManager &statsManager);

  /**
   * @brief Default Destructor
   */
  virtual ~HardwareBuffer();

  HardwareBuffer(const HardwareBuffer &rhs) = delete;
  HardwareBuffer(HardwareBuffer &&rhs) = delete;
  HardwareBuffer &operator=(const HardwareBuffer &rhs) = delete;
  HardwareBuffer &operator=(HardwareBuffer &&rhs) = delete;

  /**
   * @brief Map buffer
   *
   * @return Mapped data
   */
  virtual void *map() = 0;

  /**
   * @brief Unmap buffer
   */
  virtual void unmap() = 0;

  /**
   * @brief Update buffer
   *
   * @param data Uniform data
   */
  virtual void update(void *data) = 0;

  /**
   * @brief Gets buffer size
   *
   * @return Buffer size
   */
  inline size_t getBufferSize() { return bufferSize; }

  /**
   * @brief Gets buffer type
   *
   * Values can be vertex, index, or uniform.
   *
   * @return Buffer type
   */
  inline HardwareBufferType getType() { return bufferType; }

private:
  size_t bufferSize = 0;
  HardwareBufferType bufferType;
  StatsManager &statsManager;
};

} // namespace liquid
