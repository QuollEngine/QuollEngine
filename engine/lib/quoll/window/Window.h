#pragma once

#include "quoll/input/InputDeviceManager.h"
#include "WindowSignals.h"

struct GLFWwindow;

namespace quoll {

class Window : NoCopyMove {
public:
  Window(StringView title, u32 width, u32 height,
         InputDeviceManager &deviceManager);

  ~Window();

  inline ::GLFWwindow *getInstance() { return mWindowInstance; }

  glm::uvec2 getFramebufferSize();

  glm::uvec2 getWindowSize();

  void setWindowSize(const glm::uvec2 &size);

  void maximize();

  bool shouldClose();

  void pollEvents();

  glm::vec2 getCurrentMousePosition() const;

  void setMousePosition(const glm::vec2 &position);

  bool isKeyPressed(int key) const;

  void focus();

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
