#pragma once

namespace liquid {

class EntityError : public std::runtime_error {
public:
  /**
   * @brief Create entity error
   *
   * @param what Error message
   */
  EntityError(const String &what);
};

} // namespace liquid
