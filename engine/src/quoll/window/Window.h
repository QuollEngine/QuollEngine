#pragma once

#include "quoll/input/InputDeviceManager.h"
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
   */
  Window(StringView title, u32 width, u32 height,
         InputDeviceManager &deviceManager);

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
  ::GLFWwindow *mWindowInstance = nullptr;

  WindowSignals mSignals;
};

} // namespace quoll
