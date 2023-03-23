#pragma once

namespace liquid::editor {

/**
 * @brief Keyboard shortcut
 */
class Shortcut {
public:
  /**
   * @brief Default constructor
   */
  Shortcut() = default;

  /**
   * @brief Create shortcut
   *
   * @param key Keyboard key
   * @param mods Keyboard mods
   */
  Shortcut(int key, int mods);

  /**
   * @brief Set shift mod
   *
   * @return This shortcut
   */
  Shortcut &shift();

  /**
   * @brief Set control mod
   *
   * @return This shortcut
   */
  Shortcut &control();

  /**
   * @brief Set alt mod
   *
   * @return This shortcut
   */
  Shortcut &alt();

  /**
   * @brief Set keyboard key
   *
   * @param key Keyboard key
   * @return This shortcut
   */
  Shortcut &key(char key);

  /**
   * @brief Get all keyboard mods
   *
   * @return Keyboard mods
   */
  inline int getMods() const { return mMods; }

  /**
   * @brief Get keyboard key
   *
   * @return Keyboard key
   */
  inline int getKey() const { return mKey; }

  /**
   * @brief Check if shortcut is valid
   *
   * @retval true Shortcut is valid
   * @retval false Shortcut is not valid
   */
  inline operator bool() const { return mKey != -1 && mMods != 0; }

  /**
   * @brief Get human readable string
   *
   * @return Human readable string
   */
  String toString() const;

  /**
   * @brief Check if two shortcuts are equal
   *
   * @param rhs Right hand side
   * @retval true Shortcuts are equal
   * @retval false Shortcuts are not equal
   */
  bool operator==(const Shortcut &rhs) const;

private:
  int mMods = 0;
  int mKey = -1;
};

} // namespace liquid::editor
