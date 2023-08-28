#pragma once

namespace liquid {

/**
 * @brief UUID
 */
class Uuid {
public:
  /**
   * @brief Generate uuid
   *
   * @return Newly generated uuid
   */
  static Uuid generate();

public:
  /**
   * @brief Default constructor
   */
  Uuid() = default;

  /**
   * @brief Create UUID from string
   *
   * @param uuid String
   */
  explicit Uuid(String uuid);

  /**
   * @brief Get UUID as string
   *
   * @return UUID as string
   */
  inline String toString() const { return mUuid; }

  /**
   * @brief Check if two UUIDs are equal
   *
   * @param rhs Other Uuid
   * @retval true Uuids are equal
   * @retval false Uuids are not equal
   */
  inline bool operator==(const Uuid &rhs) const { return mUuid == rhs.mUuid; }

  /**
   * @brief Check if two UUIDs are not equal
   *
   * @param rhs Other Uuid
   * @retval true Uuids are not equal
   * @retval false Uuids are equal
   */
  inline bool operator!=(const Uuid &rhs) const { return mUuid != rhs.mUuid; }

  /**
   * @brief Check if Uuid is valid
   *
   * @retval true Uuid is valid
   * @retval false Uuid is not valid
   */
  bool isValid() const { return !mUuid.empty(); }

  /**
   * @brief Check if Uuid is empty
   *
   * @retval true Uuid is empty
   * @retval false Uuid is not empty
   */
  bool isEmpty() const { return mUuid.empty(); }

  /**
   * @brief Update with string
   *
   * Mainly used for decoding data
   * into the Uuid
   *
   * @param uuid String uuid
   */
  void updateWithString(String uuid);

private:
  String mUuid = "";
};

} // namespace liquid
