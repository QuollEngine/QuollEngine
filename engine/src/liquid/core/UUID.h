#pragma once

namespace liquid {

/**
 * @brief UUID object
 */
class UUID {
  // UUID string + dashes
  static constexpr size_t UUID_LENGTH = 36;

public:
  /**
   * @brief Create UUID
   *
   * @param value String value
   */
  UUID(const String &value);

  /**
   * @brief Check if two UUIDs are equal
   *
   * @param rhs Other UUID
   * @retval true UUIDs are equal
   * @retval false UUIDs are NOT equal
   */
  inline bool operator==(const UUID &rhs) const { return mValue == rhs.mValue; }

  /**
   * @brief Check if two UUIDs are NOT equal
   *
   * @param rhs Other UUID
   * @retval true UUIDs are NOT equal
   * @retval false UUIDs are equal
   */
  inline bool operator!=(const UUID &rhs) const { return mValue != rhs.mValue; }

  /**
   * @brief Check if UUID is valid
   *
   * @retval true UUID is valid
   * @retval false UUID is not valid
   */
  inline bool isValid() const { return mValue.length() == UUID_LENGTH; }

  /**
   * @brief Convert UUID to string
   *
   * @return String representation of UUID
   */
  inline const String &toString() const { return mValue; }

private:
  String mValue;
};

} // namespace liquid
