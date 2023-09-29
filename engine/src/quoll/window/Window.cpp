#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "Window.h"

#include <GLFW/glfw3.h>

#include "quoll/platform/tools/WindowUtils.h"
#include "quoll/input/KeyMappings.h"

namespace quoll {

Window::Window(StringView title, uint32_t width, uint32_t height,
               InputDeviceManager &deviceManager, EventSystem &eventSystem)
    : mDeviceManager(deviceManager), mEventSystem(eventSystem) {
  auto initReturnValue = glfwInit();
  if (initReturnValue == GLFW_FALSE) {
    const char *errorMsg = nullptr;
    glfwGetError(&errorMsg);
    QuollAssert(initReturnValue,
                "Failed to initialize GLFW: " + String(errorMsg));
    Engine::getLogger().error() << "Failed to initialize GLFW: " << errorMsg;
    return;
  }

  // Do not create OpenGL context
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  mWindowInstance =
      glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
                       String(title).c_str(), nullptr, nullptr);

  platform::WindowUtils::enableDarkMode(mWindowInstance);

  if (!mWindowInstance) {
    const char *errorMsg = nullptr;
    glfwGetError(&errorMsg);
    QuollAssert(initReturnValue,
                "Failed to create GLFW window: " + String(errorMsg));
    Engine::getLogger().error() << "Failed to create GLFW window: " << errorMsg;
  }

  Engine::getLogger().info() << "Window Created. Title: " << title
                             << "; Width: " << width << "; Height: " << height;

  glfwSetWindowUserPointer(mWindowInstance, this);

  glfwSetFramebufferSizeCallback(
      mWindowInstance, [](::GLFWwindow *windowInstance, int width, int height) {
        auto *window =
            static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));
        for (auto &[_, handler] : window->mResizeHandlers) {
          handler(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
        }
      });

  glfwSetWindowFocusCallback(
      mWindowInstance, [](::GLFWwindow *windowInstance, int focused) {
        auto *window =
            static_cast<Window *>(glfwGetWindowUserPointer(windowInstance));

        for (auto &[_, handler] : window->mFocusHandlers) {
          handler(focused == GLFW_TRUE);
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

  mDeviceManager.addDevice({.type = InputDeviceType::Keyboard,
                            .name = "Keyboard",
                            .index = 0,
                            .stateFn = std::bind(&Window::getKeyboardState,
                                                 this, std::placeholders::_1)});

  mDeviceManager.addDevice({.type = InputDeviceType::Mouse,
                            .name = "Mouse",
                            .index = 0,
                            .stateFn = std::bind(&Window::getMouseState, this,
                                                 std::placeholders::_1)});

  for (int jid = 0; jid <= GLFW_JOYSTICK_LAST; ++jid) {
    if (glfwJoystickPresent(jid)) {
      addGamepad(jid, this);
    }
  }

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
  static auto *StaticWindowObj = this;
  glfwSetJoystickCallback([](int jid, int event) {
    auto *window = StaticWindowObj;

    if (event == GLFW_CONNECTED) {
      addGamepad(jid, window);
    } else if (event == GLFW_DISCONNECTED) {
      window->mDeviceManager.removeDevice(InputDeviceType::Gamepad,
                                          static_cast<uint32_t>(jid));
    }
  });
}

Window::~Window() {
  if (mWindowInstance) {
    glfwDestroyWindow(mWindowInstance);
    mWindowInstance = nullptr;
  }
  Engine::getLogger().info() << "Window destroyed";
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

uint32_t Window::addFramebufferResizeHandler(
    const std::function<void(uint32_t, uint32_t)> &handler) {
  uint32_t id = static_cast<uint32_t>(mResizeHandlers.size());

  mResizeHandlers.insert(std::make_pair(id, handler));
  return id;
}

void Window::removeResizeHandler(uint32_t handle) {
  mResizeHandlers.erase(mResizeHandlers.find(handle));
}

uint32_t Window::addFocusHandler(const std::function<void(bool)> &handler) {
  uint32_t id = static_cast<uint32_t>(mFocusHandlers.size());
  mFocusHandlers.insert(std::make_pair(id, handler));

  return id;
}

void Window::removeFocusHandler(uint32_t handle) {
  mFocusHandlers.erase(handle);
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

InputStateValue Window::getKeyboardState(int key) {
  int val = glfwGetKey(mWindowInstance, input::getGlfwKeyboardKey(key));
  return val == GLFW_PRESS;
}

InputStateValue Window::getMouseState(int key) {
  if (input::isMouseMove(key)) {
    double xpos = 0.0, ypos = 0.0;
    glfwGetCursorPos(mWindowInstance, &xpos, &ypos);

    auto windowSize = getWindowSize();

    return glm::vec2{
        static_cast<float>(xpos) / static_cast<float>(windowSize.x),
        static_cast<float>(ypos) / static_cast<float>(windowSize.y)};
  }

  return glfwGetMouseButton(mWindowInstance, input::getGlfwMouseButton(key)) ==
         GLFW_PRESS;
}

InputStateValue Window::getGamepadState(int jid, int key) {
  GLFWgamepadstate state{};
  glfwGetGamepadState(jid, &state);

  if (input::isGamepadAxis(key)) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    return state.axes[input::getGlfwGamepadAxis(key)];
  }

  if (input::isGamepadButton(key)) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    return state.buttons[input::getGlfwGamepadButton(key)] == GLFW_PRESS;
  }

  return {};
}

void Window::addGamepad(int jid, Window *window) {
  if (!glfwJoystickIsGamepad(jid)) {
    return;
  }

  const char *name = glfwGetJoystickName(jid);

  InputDevice device = {.type = InputDeviceType::Gamepad,
                        .name = name,
                        .index = static_cast<uint32_t>(jid),
                        .stateFn = std::bind(&Window::getGamepadState, window,
                                             jid, std::placeholders::_1)};

  window->mDeviceManager.addDevice(device);
}

void Window::setWindowSize(const glm::uvec2 &size) {
  glfwSetWindowSize(mWindowInstance, static_cast<int>(size.x),
                    static_cast<int>(size.y));
}

void Window::maximize() { glfwMaximizeWindow(mWindowInstance); }

} // namespace quoll
