#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "GLFWWindow.h"
#include "GLFWError.h"

#include <GLFW/glfw3.h>

namespace liquid {

GLFWWindow::GLFWWindow(const String &title, uint32_t width, uint32_t height) {
  if (!glfwInit()) {
    throw GLFWError("Failed to initialize GLFW");
  }

  // Do not create OpenGL context
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  windowInstance =
      glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
                       title.c_str(), nullptr, nullptr);
  if (!windowInstance) {
    throw GLFWError("Failed to create window");
  }

  LOG_DEBUG("[GLFW] Window Created");

  glfwSetWindowUserPointer(windowInstance, this);

  glfwSetFramebufferSizeCallback(
      windowInstance, [](::GLFWwindow *windowInstance, int width, int height) {
        auto *window =
            static_cast<GLFWWindow *>(glfwGetWindowUserPointer(windowInstance));
        for (auto &[_, handler] : window->resizeHandlers) {
          handler(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }
      });

  glfwSetKeyCallback(windowInstance, [](::GLFWwindow *windowInstance, int key,
                                        int scancode, int action, int mods) {
    auto *window =
        static_cast<GLFWWindow *>(glfwGetWindowUserPointer(windowInstance));

    for (auto &[_, handler] : window->keyHandlers) {
      handler(key, scancode, action, mods);
    }
  });

  glfwSetCursorPosCallback(windowInstance, [](::GLFWwindow *windowInstance,
                                              double xpos, double ypos) {
    auto *window =
        static_cast<GLFWWindow *>(glfwGetWindowUserPointer(windowInstance));

    for (auto &[_, handler] : window->mouseMoveHandlers) {
      handler(xpos, ypos);
    }
  });

  glfwSetMouseButtonCallback(
      windowInstance,
      [](::GLFWwindow *windowInstance, int button, int action, int mods) {
        auto *window =
            static_cast<GLFWWindow *>(glfwGetWindowUserPointer(windowInstance));

        for (auto &[_, handler] : window->mouseButtonHandlers) {
          handler(button, action, mods);
        }
      });

  glfwSetScrollCallback(windowInstance, [](::GLFWwindow *windowInstance,
                                           double xoffset, double yoffset) {
    auto *window =
        static_cast<GLFWWindow *>(glfwGetWindowUserPointer(windowInstance));
    for (auto &[_, handler] : window->scrollWheelHandlers) {
      handler(xoffset, yoffset);
    }
  });
}

GLFWWindow::~GLFWWindow() {
  resizeHandlers.clear();
  keyHandlers.clear();

  if (windowInstance) {
    glfwDestroyWindow(windowInstance);
    windowInstance = nullptr;
  }
  LOG_DEBUG("[GLFW] Window destroyed");
  glfwTerminate();
}

SizeObject GLFWWindow::getFramebufferSize() {
  int width = 0, height = 0;
  glfwGetFramebufferSize(windowInstance, &width, &height);

  return {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height),
  };
}

SizeObject GLFWWindow::getWindowSize() {
  int width = 0, height = 0;
  glfwGetWindowSize(windowInstance, &width, &height);

  return {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height),
  };
}

bool GLFWWindow::shouldClose() { return glfwWindowShouldClose(windowInstance); }

void GLFWWindow::pollEvents() { glfwPollEvents(); }

uint32_t GLFWWindow::addResizeHandler(
    const std::function<void(uint32_t, uint32_t)> &handler) {
  uint32_t id = static_cast<uint32_t>(resizeHandlers.size());

  resizeHandlers.insert(std::make_pair(id, handler));
  return id;
}

void GLFWWindow::removeResizeHandler(uint32_t handle) {
  resizeHandlers.erase(resizeHandlers.find(handle));
}

uint32_t GLFWWindow::addKeyHandler(
    const std::function<void(int, int, int, int)> &handler) {
  uint32_t id = static_cast<uint32_t>(keyHandlers.size());

  keyHandlers.insert(std::make_pair(id, handler));
  return id;
}

void GLFWWindow::removeKeyHandler(uint32_t handle) {
  keyHandlers.erase(keyHandlers.find(handle));
}

uint32_t GLFWWindow::addMouseMoveHandler(
    const std::function<void(double xpos, double ypos)> &handler) {
  uint32_t id = static_cast<uint32_t>(mouseMoveHandlers.size());
  mouseMoveHandlers.insert(std::make_pair(id, handler));
  return id;
}

void GLFWWindow::removeMouseMoveHandler(uint32_t handle) {
  mouseMoveHandlers.erase(mouseMoveHandlers.find(handle));
}

uint32_t GLFWWindow::addMouseButtonHandler(
    const std::function<void(int button, int action, int mods)> &handler) {
  uint32_t id = static_cast<uint32_t>(mouseButtonHandlers.size());
  mouseButtonHandlers.insert(std::make_pair(id, handler));
  return id;
}

void GLFWWindow::removeMouseButtonHandler(uint32_t handle) {
  mouseButtonHandlers.erase(mouseButtonHandlers.find(handle));
}

uint32_t GLFWWindow::addScrollWheelHandler(
    const std::function<void(double xoffset, double yoffset)> &handler) {
  uint32_t id = static_cast<uint32_t>(scrollWheelHandlers.size());
  scrollWheelHandlers.insert(std::make_pair(id, handler));
  return id;
}

void GLFWWindow::removeScrollWheelHandler(uint32_t handle) {
  scrollWheelHandlers.erase(scrollWheelHandlers.find(handle));
}

glm::vec2 GLFWWindow::getCurrentMousePosition() const {
  glm::dvec2 mousePos;
  glfwGetCursorPos(windowInstance, &mousePos.x, &mousePos.y);

  return glm::vec2(mousePos);
}

void GLFWWindow::setMousePosition(const glm::vec2 &position) {
  glfwSetCursorPos(windowInstance, position.x, position.y);
}

bool GLFWWindow::isKeyPressed(int key) const {
  return glfwGetKey(windowInstance, key) == GLFW_PRESS;
}

} // namespace liquid
