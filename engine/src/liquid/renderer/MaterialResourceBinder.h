#pragma once

namespace liquid {

class MaterialResourceBinder {
public:
  /**
   * @brief Destroy material resource binder
   */
  virtual ~MaterialResourceBinder() = default;

  MaterialResourceBinder() = default;
  MaterialResourceBinder(const MaterialResourceBinder &rhs) = delete;
  MaterialResourceBinder(MaterialResourceBinder &&rhs) = delete;
  MaterialResourceBinder &operator=(const MaterialResourceBinder &rhs) = delete;
  MaterialResourceBinder &operator=(MaterialResourceBinder &&rhs) = delete;
};

} // namespace liquid
