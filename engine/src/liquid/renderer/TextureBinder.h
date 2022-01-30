#pragma once

namespace liquid {

class TextureBinder {
public:
  /**
   * @brief Default virtual destructor
   */
  virtual ~TextureBinder() = default;

  TextureBinder() = default;
  TextureBinder(const TextureBinder &rhs) = delete;
  TextureBinder(TextureBinder &&rhs) = delete;
  TextureBinder &operator=(const TextureBinder &rhs) = delete;
  TextureBinder &operator=(TextureBinder &&rhs) = delete;
};

} // namespace liquid
