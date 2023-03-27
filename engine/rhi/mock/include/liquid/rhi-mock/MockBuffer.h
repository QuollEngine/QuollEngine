#pragma once

#include "liquid/rhi/BufferDescription.h"
#include "liquid/rhi/NativeBuffer.h"

namespace liquid::rhi {

/**
 * @brief Mock buffer
 */
class MockBuffer : public NativeBuffer {
public:
  /**
   * @brief Create mock buffer
   *
   * @param description Buffer description
   */
  MockBuffer(const BufferDescription &description);

  /**
   * @brief Map buffer
   *
   * @return Mapped buffer
   */
  void *map() override;

  /**
   * @brief Unmap buffer
   */
  void unmap() override;

  /**
   * @brief Resize buffer
   *
   * @param size Resized buffer
   */
  void resize(size_t size) override;

  /**
   * @brief Get buffer description
   *
   * @return Buffer description
   */
  inline const BufferDescription &getDescription() const {
    return mDescription;
  }

private:
  std::vector<uint8_t> mData;
  BufferDescription mDescription;
};

} // namespace liquid::rhi
