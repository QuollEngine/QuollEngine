#pragma once

#include "quoll/events/EventSystem.h"
#include "quoll/input/InputDeviceManager.h"
#include "quoll/lua-scripting/ScriptSignal.h"
#include "WindowSignals.h"

struct GLFWwindow;

namespace quoll {

/**
 * @brief GLFW Window
 *
 * Manages lifecycle and handles of
 * GLFW window
 */
class Window {
public:
  /**
   * @brief Creates window
   *
   * @param title Window title
   * @param width Window width
   * @param height Window height
   * @param deviceManager Device manager
   * @param eventSystem Event system
   */
  Window(StringView title, u32 width, u32 height,
         InputDeviceManager &deviceManager, EventSystem &eventSystem);

  /**
   * @brief Destroys window
   */
  ~Window();

  Window(const Window &rhs) = delete;
  Window(Window &&rhs) = delete;
  Window &operator=(const Window &rhs) = delete;
  Window &operator=(Window &&rhs) = delete;

  /**
   * @brief Gets GLFW instance
   *
   * @return GLFWwindow Pointer to GLFW window instance
   */
  inline ::GLFWwindow *getInstance() { return mWindowInstance; }

  /**
   * @brief Gets framebuffer size
   *
   * @return Framebuffer size
   */
  glm::uvec2 getFramebufferSize();

  /**
   * @brief Gets window size
   *
   * @return Framebuffer size
   */
  glm::uvec2 getWindowSize();

  /**
   * @brief Set window size
   *
   * @param size Window size
   */
  void setWindowSize(const glm::uvec2 &size);

  /**
   * @brief Maximize window
   */
  void maximize();

  /**
   * @brief Checks is window should close
   *
   * @retval true Window should close
   * @retval false Window should not close
   */
  bool shouldClose();

  /**
   * @brief Polls window events
   */
  void pollEvents();

  /**
   * @brief Add framebuffer resize handler
   *
   * @param handler Framebuffer resize handler
   * @return Framebuffer resize handler ID
   */
  u32 addFramebufferResizeHandler(const std::function<void(u32, u32)> &handler);

  /**
   * @brief Remove resize handler
   *
   * @param handle Handle
   */
  void removeResizeHandler(u32 handle);

  /**
   * @brief Add focus handler
   *
   * @param handler Focus handler
   * @return Focus handler ID
   */
  u32 addFocusHandler(const std::function<void(bool)> &handler);

  /**
   * @brief Remove focus handler
   *
   * @param handle Handle
   */
  void removeFocusHandler(u32 handle);

  /**
   * @brief Get current mouse position
   *
   * @return Current mouse position
   */
  glm::vec2 getCurrentMousePosition() const;

  /**
   * @brief Set mouse position
   *
   * @param position Mouse position
   */
  void setMousePosition(const glm::vec2 &position);

  /**
   * @brief Check if key is pressed
   *
   * @param key Keyboard key
   * @retval true Pressed
   * @retval false Not pressed
   */
  bool isKeyPressed(int key) const;

  /**
   * @brief Focus window
   */
  void focus();

  /**
   * @brief Get window signals
   *
   * @return Window signals
   */
  inline WindowSignals &getSignals() { return mSignals; }

private:
  InputStateValue getKeyboardState(int key);

  InputStateValue getMouseState(int key);

  InputStateValue getGamepadState(int jid, int key);

  static void addGamepad(int jid, Window *window);

private:
  InputDeviceManager &mDeviceManager;
  EventSystem &mEventSystem;
  ::GLFWwindow *mWindowInstance;

  template <class TFunctionType>
  using HandlerMap = std::map<u32, std::function<TFunctionType>>;

  WindowSignals mSignals;

  HandlerMap<void(u32, u32)> mResizeHandlers;
  HandlerMap<void(bool)> mFocusHandlers;
};

} // namespace quoll
