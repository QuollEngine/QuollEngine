#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace liquid {

Window::Window(const String &title, uint32_t width, uint32_t height) {
  LIQUID_ASSERT(glfwInit(), "[GLFW] Failed to initialize GLFW");

  // Do not create OpenGL context
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  windowInstance =
      glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
                       title.c_str(), nullptr, nullptr);

  LIQUID_ASSERT(windowInstance, "[GLFW] Failed to create windows");

  LOG_DEBUG("[GLFW] Window Created");

  glfwSetWindowUserPointer(windowInstance, this);

  glfwSetFramebufferSizeCallback(
      windowInstance, [](::GLFWwindow *windowInstance, int width, int height) {
        auto *window =
            static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));
        for (auto &[_, handler] : window->resizeHandlers) {
          handler(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }
      });

  glfwSetKeyCallback(windowInstance, [](::GLFWwindow *windowInstance, int key,
                                        int scancode, int action, int mods) {
    auto *window =
        static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));

    for (auto &[_, handler] : window->keyHandlers) {
      handler(key, scancode, action, mods);
    }
  });

  glfwSetCursorPosCallback(windowInstance, [](::GLFWwindow *windowInstance,
                                              double xpos, double ypos) {
    auto *window =
        static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));

    for (auto &[_, handler] : window->mouseMoveHandlers) {
      handler(xpos, ypos);
    }
  });

  glfwSetMouseButtonCallback(
      windowInstance,
      [](::GLFWwindow *windowInstance, int button, int action, int mods) {
        auto *window =
            static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));

        for (auto &[_, handler] : window->mouseButtonHandlers) {
          handler(button, action, mods);
        }
      });

  glfwSetScrollCallback(windowInstance, [](::GLFWwindow *windowInstance,
                                           double xoffset, double yoffset) {
    auto *window =
        static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));
    for (auto &[_, handler] : window->scrollWheelHandlers) {
      handler(xoffset, yoffset);
    }
  });
}

Window::~Window() {
  resizeHandlers.clear();
  keyHandlers.clear();

  if (windowInstance) {
    glfwDestroyWindow(windowInstance);
    windowInstance = nullptr;
  }
  LOG_DEBUG("[GLFW] Window destroyed");
  glfwTerminate();
}

glm::uvec2 Window::getFramebufferSize() {
  int width = 0, height = 0;
  glfwGetFramebufferSize(windowInstance, &width, &height);

  return {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height),
  };
}

glm::uvec2 Window::getWindowSize() {
  int width = 0, height = 0;
  glfwGetWindowSize(windowInstance, &width, &height);

  return {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height),
  };
}

bool Window::shouldClose() { return glfwWindowShouldClose(windowInstance); }

void Window::pollEvents() { glfwPollEvents(); }

uint32_t Window::addResizeHandler(
    const std::function<void(uint32_t, uint32_t)> &handler) {
  uint32_t id = static_cast<uint32_t>(resizeHandlers.size());

  resizeHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeResizeHandler(uint32_t handle) {
  resizeHandlers.erase(resizeHandlers.find(handle));
}

uint32_t
Window::addKeyHandler(const std::function<void(int, int, int, int)> &handler) {
  uint32_t id = static_cast<uint32_t>(keyHandlers.size());

  keyHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeKeyHandler(uint32_t handle) {
  keyHandlers.erase(keyHandlers.find(handle));
}

uint32_t Window::addMouseMoveHandler(
    const std::function<void(double xpos, double ypos)> &handler) {
  uint32_t id = static_cast<uint32_t>(mouseMoveHandlers.size());
  mouseMoveHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeMouseMoveHandler(uint32_t handle) {
  mouseMoveHandlers.erase(mouseMoveHandlers.find(handle));
}

uint32_t Window::addMouseButtonHandler(
    const std::function<void(int button, int action, int mods)> &handler) {
  uint32_t id = static_cast<uint32_t>(mouseButtonHandlers.size());
  mouseButtonHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeMouseButtonHandler(uint32_t handle) {
  mouseButtonHandlers.erase(mouseButtonHandlers.find(handle));
}

uint32_t Window::addScrollWheelHandler(
    const std::function<void(double xoffset, double yoffset)> &handler) {
  uint32_t id = static_cast<uint32_t>(scrollWheelHandlers.size());
  scrollWheelHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeScrollWheelHandler(uint32_t handle) {
  scrollWheelHandlers.erase(scrollWheelHandlers.find(handle));
}

glm::vec2 Window::getCurrentMousePosition() const {
  glm::dvec2 mousePos;
  glfwGetCursorPos(windowInstance, &mousePos.x, &mousePos.y);

  return glm::vec2(mousePos);
}

void Window::setMousePosition(const glm::vec2 &position) {
  glfwSetCursorPos(windowInstance, position.x, position.y);
}

bool Window::isKeyPressed(int key) const {
  return glfwGetKey(windowInstance, key) == GLFW_PRESS;
}

void Window::focus() { glfwFocusWindow(windowInstance); }

} // namespace liquid
