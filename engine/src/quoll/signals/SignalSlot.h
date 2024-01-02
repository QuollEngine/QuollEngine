#pragma once

namespace quoll {

/**
 * @brief Signal slot
 */
class SignalSlot {
public:
  /**
   * @brief Default constructor
   */
  SignalSlot() = default;

  /**
   * @brief Create signal slot
   *
   * @param disconnect Disconnect function
   */
  SignalSlot(std::function<void()> &&disconnect);

  /**
   * @brief Disconnect signal slot
   */
  inline void disconnect() { return mDisconnect(); }

private:
  std::function<void()> mDisconnect;
};

} // namespace quoll
