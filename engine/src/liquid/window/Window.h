#pragma once

#include "liquid/events/EventSystem.h"

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
   * @param eventSystem Event system
   */
  Window(StringView title, uint32_t width, uint32_t height,
         EventSystem &eventSystem);

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
   * @brief Add resize handler
   *
   * @param handler Resize handler
   * @return Resize handler ID
   */
  uint32_t
  addResizeHandler(const std::function<void(uint32_t, uint32_t)> &handler);

  /**
   * @brief Remove resize handler
   *
   * @param handle Handle
   */
  void removeResizeHandler(uint32_t handle);

  /**
   * @brief Add focus handler
   *
   * @param handler Focus handler
   * @return Focus handler ID
   */
  uint32_t addFocusHandler(const std::function<void(bool)> &handler);

  /**
   * @brief Remove focus handler
   *
   * @param handle Handle
   */
  void removeFocusHandler(uint32_t handle);

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

private:
  EventSystem &mEventSystem;
  ::GLFWwindow *mWindowInstance;

  template <class TFunctionType>
  using HandlerMap = std::map<uint32_t, std::function<TFunctionType>>;

  HandlerMap<void(uint32_t, uint32_t)> mResizeHandlers;
  HandlerMap<void(bool)> mFocusHandlers;
};

} // namespace quoll
