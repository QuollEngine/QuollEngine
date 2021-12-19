#pragma once

#define GLFW_INCLUDE_VULKAN

#include "core/Base.h"
#include <GLFW/glfw3.h>

struct GLFWwindow;

namespace liquid {

struct SizeObject {
  uint32_t width;
  uint32_t height;
};

/**
 * @brief GLFW Window
 *
 * Manages lifecycle and handles of
 * GLFW window
 */
class GLFWWindow {
public:
  /**
   * @brief Creates window
   *
   * @param title Window title
   * @param width Window width
   * @param height Window height
   */
  GLFWWindow(const String &title, uint32_t width, uint32_t height);

  /**
   * @brief Destroys window
   */
  ~GLFWWindow();

  GLFWWindow(const GLFWWindow &rhs) = delete;
  GLFWWindow(GLFWWindow &&rhs) = delete;
  GLFWWindow &operator=(const GLFWWindow &rhs) = delete;
  GLFWWindow &operator=(GLFWWindow &&rhs) = delete;

  /**
   * @brief Gets GLFW instance
   *
   * @return GLFWwindow Pointer to GLFW window instance
   */
  inline ::GLFWwindow *getInstance() { return windowInstance; }

  /**
   * @brief Gets framebuffer size
   *
   * @return Framebuffer size
   */
  SizeObject getFramebufferSize();

  /**
   * @brief Gets window size
   *
   * @return Framebuffer size
   */
  SizeObject getWindowSize();

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
   * @brief Add key handler
   *
   * @param handler Key handler
   * @return Key handler ID
   */
  uint32_t addKeyHandler(
      const std::function<void(int key, int scancode, int action, int mods)>
          &handler);

  /**
   * @brief Remove key down handler
   *
   * @param handle Handle
   */
  void removeKeyHandler(uint32_t handle);

  /**
   * @brief Add mouse move handler
   *
   * @param handler Mouse move handler
   * @return Mouse move handler ID
   */
  uint32_t addMouseMoveHandler(
      const std::function<void(double xpos, double ypos)> &handler);

  /**
   * @brief Remove mouse move handler
   *
   * @param handle Mouse move handle
   */
  void removeMouseMoveHandler(uint32_t handle);

  /**
   * @brief Add mouse button handler
   *
   * @param handler Mouse button handler
   * @return Mouse button handler ID
   */
  uint32_t addMouseButtonHandler(
      const std::function<void(int button, int action, int mods)> &handler);

  /**
   * @brief Remove mouse button handler
   *
   * @param handle Mouse button handle
   */
  void removeMouseButtonHandler(uint32_t handle);

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

private:
  ::GLFWwindow *windowInstance;

  template <class FunctionType>
  using HandlerMap = std::map<uint32_t, std::function<FunctionType>>;

  HandlerMap<void(uint32_t, uint32_t)> resizeHandlers;
  HandlerMap<void(int key, int scancode, int action, int mods)> keyHandlers;
  HandlerMap<void(int button, int action, int mods)> mouseButtonHandlers;
  HandlerMap<void(double xpos, double ypos)> mouseMoveHandlers;
};

} // namespace liquid
