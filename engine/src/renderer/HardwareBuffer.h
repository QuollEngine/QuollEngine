#pragma once

#include "core/Base.h"
#include "scene/Vertex.h"

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
  enum HardwareBufferType { VERTEX, INDEX, UNIFORM };

public:
  /**
   * @brief Constructor for vertex buffer
   *
   * @param vertices List of vertices
   * @param statsManager Stats manager
   */
  HardwareBuffer(const std::vector<Vertex> &vertices,
                 const SharedPtr<StatsManager> &statsManager = nullptr);

  /**
   * @brief Constructor for index buffer
   *
   * @param indices List of indices
   * @param statsManager Stats manager
   */
  HardwareBuffer(const std::vector<uint32_t> &indices,
                 const SharedPtr<StatsManager> &statsManager = nullptr);

  /**
   * @brief Constructor for uniform buffer
   *
   * @param bufferSize Buffer size
   * @param statsManager Stats manager
   */
  HardwareBuffer(size_t bufferSize,
                 const SharedPtr<StatsManager> &statsManager = nullptr);

  /**
   * @brief Default Destructor
   */
  virtual ~HardwareBuffer();

  HardwareBuffer(const HardwareBuffer &rhs) = delete;
  HardwareBuffer(HardwareBuffer &&rhs) = delete;
  HardwareBuffer &operator=(const HardwareBuffer &rhs) = delete;
  HardwareBuffer &operator=(HardwareBuffer &&rhs) = delete;

  /**
   * @brief Update buffer
   *
   * @param data Uniform data
   */
  virtual void update(void *data) = 0;

  /**
   * @brief Gets number of items in the buffer
   *
   * @return Number of items
   */
  inline size_t getItemSize() { return itemSize; }

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
  size_t itemSize;
  size_t bufferSize;
  HardwareBufferType bufferType;
  SharedPtr<StatsManager> statsManager;
};

} // namespace liquid
