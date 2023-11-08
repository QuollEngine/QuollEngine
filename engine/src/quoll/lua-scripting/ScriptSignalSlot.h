#pragma once

namespace quoll::lua {

/**
 * @brief Script signal slot
 */
class ScriptSignalSlot {
public:
  /**
   * @brief Create script signal slot
   *
   * @param disconnect Disconnect function
   */
  ScriptSignalSlot(std::function<void()> &&disconnect);

  /**
   * @brief Disconnect signal slot
   */
  inline void disconnect() { return mDisconnect(); }

private:
  std::function<void()> mDisconnect;
};

} // namespace quoll::lua
