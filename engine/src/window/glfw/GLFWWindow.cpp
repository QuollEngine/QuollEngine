#include "GLFWWindow.h"
#include "GLFWError.h"
#include "core/EngineGlobals.h"

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

} // namespace liquid
