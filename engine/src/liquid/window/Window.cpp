#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace liquid {

Window::Window(const String &title, uint32_t width, uint32_t height) {

  auto initReturnValue = glfwInit();
  if (initReturnValue == GLFW_FALSE) {
    const char *errorMsg = nullptr;
    glfwGetError(&errorMsg);
    LIQUID_ASSERT(initReturnValue,
                  "[GLFW] Failed to initialize GLFW: " + String(errorMsg));
    engineLogger.log(Logger::Error)
        << "Failed to initialize GLFW: " << errorMsg;
  }

  // Do not create OpenGL context
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  mWindowInstance =
      glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
                       title.c_str(), nullptr, nullptr);

  LIQUID_ASSERT(mWindowInstance, "[GLFW] Failed to create windows");

  LOG_DEBUG("[GLFW] Window Created");

  glfwSetWindowUserPointer(mWindowInstance, this);

  glfwSetFramebufferSizeCallback(
      mWindowInstance, [](::GLFWwindow *windowInstance, int width, int height) {
        auto *window =
            static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));
        for (auto &[_, handler] : window->mResizeHandlers) {
          handler(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }
      });

  glfwSetKeyCallback(mWindowInstance, [](::GLFWwindow *windowInstance, int key,
                                         int scancode, int action, int mods) {
    auto *window =
        static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));

    for (auto &[_, handler] : window->mKeyHandlers) {
      handler(key, scancode, action, mods);
    }
  });

  glfwSetCursorPosCallback(mWindowInstance, [](::GLFWwindow *windowInstance,
                                               double xpos, double ypos) {
    auto *window =
        static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));

    for (auto &[_, handler] : window->mMouseMoveHandlers) {
      handler(xpos, ypos);
    }
  });

  glfwSetMouseButtonCallback(
      mWindowInstance,
      [](::GLFWwindow *windowInstance, int button, int action, int mods) {
        auto *window =
            static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));

        for (auto &[_, handler] : window->mMouseButtonHandlers) {
          handler(button, action, mods);
        }
      });

  glfwSetScrollCallback(mWindowInstance, [](::GLFWwindow *windowInstance,
                                            double xoffset, double yoffset) {
    auto *window =
        static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));
    for (auto &[_, handler] : window->mScrollWheelHandlers) {
      handler(xoffset, yoffset);
    }
  });
}

Window::~Window() {
  mResizeHandlers.clear();
  mKeyHandlers.clear();

  if (mWindowInstance) {
    glfwDestroyWindow(mWindowInstance);
    mWindowInstance = nullptr;
  }
  LOG_DEBUG("[GLFW] Window destroyed");
  glfwTerminate();
}

glm::uvec2 Window::getFramebufferSize() {
  int width = 0, height = 0;
  glfwGetFramebufferSize(mWindowInstance, &width, &height);

  return {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height),
  };
}

glm::uvec2 Window::getWindowSize() {
  int width = 0, height = 0;
  glfwGetWindowSize(mWindowInstance, &width, &height);

  return {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height),
  };
}

bool Window::shouldClose() { return glfwWindowShouldClose(mWindowInstance); }

void Window::pollEvents() { glfwPollEvents(); }

uint32_t Window::addResizeHandler(
    const std::function<void(uint32_t, uint32_t)> &handler) {
  uint32_t id = static_cast<uint32_t>(mResizeHandlers.size());

  mResizeHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeResizeHandler(uint32_t handle) {
  mResizeHandlers.erase(mResizeHandlers.find(handle));
}

uint32_t
Window::addKeyHandler(const std::function<void(int, int, int, int)> &handler) {
  uint32_t id = static_cast<uint32_t>(mKeyHandlers.size());

  mKeyHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeKeyHandler(uint32_t handle) {
  mKeyHandlers.erase(mKeyHandlers.find(handle));
}

uint32_t Window::addMouseMoveHandler(
    const std::function<void(double xpos, double ypos)> &handler) {
  uint32_t id = static_cast<uint32_t>(mMouseMoveHandlers.size());
  mMouseMoveHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeMouseMoveHandler(uint32_t handle) {
  mMouseMoveHandlers.erase(mMouseMoveHandlers.find(handle));
}

uint32_t Window::addMouseButtonHandler(
    const std::function<void(int button, int action, int mods)> &handler) {
  uint32_t id = static_cast<uint32_t>(mMouseButtonHandlers.size());
  mMouseButtonHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeMouseButtonHandler(uint32_t handle) {
  mMouseButtonHandlers.erase(mMouseButtonHandlers.find(handle));
}

uint32_t Window::addScrollWheelHandler(
    const std::function<void(double xoffset, double yoffset)> &handler) {
  uint32_t id = static_cast<uint32_t>(mScrollWheelHandlers.size());
  mScrollWheelHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeScrollWheelHandler(uint32_t handle) {
  mScrollWheelHandlers.erase(mScrollWheelHandlers.find(handle));
}

glm::vec2 Window::getCurrentMousePosition() const {
  glm::dvec2 mousePos;
  glfwGetCursorPos(mWindowInstance, &mousePos.x, &mousePos.y);

  return glm::vec2(mousePos);
}

void Window::setMousePosition(const glm::vec2 &position) {
  glfwSetCursorPos(mWindowInstance, position.x, position.y);
}

bool Window::isKeyPressed(int key) const {
  return glfwGetKey(mWindowInstance, key) == GLFW_PRESS;
}

void Window::focus() { glfwFocusWindow(mWindowInstance); }

} // namespace liquid
