#pragma once

namespace quoll::lua {

/**
 * @brief Deferred loader
 *
 * Creates loader data but does
 * not execute it until it is
 * requested to be loaded. If the
 * method is requested to be loaded
 * more than once, the subsequent load
 * request will just be ignored
 */
class DeferredLoader {
public:
  /**
   * @brief Set loader
   *
   * @param callback Callback
   * @return Self
   */
  DeferredLoader &operator=(std::function<void()> callback);

  /**
   * @brief Wait
   *
   * Requests loader to be called
   */
  void wait();

private:
  std::function<void()> mCallback;
  bool mIsExecuted = false;
};

} // namespace quoll::lua
