#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace liquid {

Window::Window(StringView title, uint32_t width, uint32_t height,
               EventSystem &eventSystem)
    : mEventSystem(eventSystem) {
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
                       String(title).c_str(), nullptr, nullptr);

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

    if (action == GLFW_PRESS) {
      window->mEventSystem.dispatch(KeyboardEvent::Pressed,
                                    {key, scancode, mods});
    } else if (action == GLFW_RELEASE) {
      window->mEventSystem.dispatch(KeyboardEvent::Released,
                                    {key, scancode, mods});
    } else if (action == GLFW_REPEAT) {

      window->mEventSystem.dispatch(KeyboardEvent::Pressed,
                                    {key, scancode, mods});
    }
  });

  glfwSetCursorPosCallback(mWindowInstance, [](::GLFWwindow *windowInstance,
                                               double xpos, double ypos) {
    auto *window =
        static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));

    window->mEventSystem.dispatch(
        MouseCursorEvent::Moved,
        {static_cast<float>(xpos), static_cast<float>(ypos)});
  });

  glfwSetMouseButtonCallback(mWindowInstance, [](::GLFWwindow *windowInstance,
                                                 int button, int action,
                                                 int mods) {
    auto *window =
        static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));

    if (action == GLFW_RELEASE) {
      window->mEventSystem.dispatch(MouseButtonEvent::Released, {button, mods});
    } else if (action == GLFW_PRESS) {
      window->mEventSystem.dispatch(MouseButtonEvent::Pressed, {button, mods});
    }
  });

  glfwSetScrollCallback(mWindowInstance, [](::GLFWwindow *windowInstance,
                                            double xoffset, double yoffset) {
    auto *window =
        static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));
    window->mEventSystem.dispatch(
        MouseScrollEvent::Scroll,
        {static_cast<float>(xoffset), static_cast<float>(yoffset)});
  });
}

Window::~Window() {
  mResizeHandlers.clear();

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
