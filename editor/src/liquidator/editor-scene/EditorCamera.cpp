#include "liquid/core/Base.h"
#include "EditorCamera.h"

#include <GLFW/glfw3.h>

using liquid::Window;

namespace liquidator {

EditorCamera::EditorCamera(liquid::EntityDatabase &entityDatabase,
                           liquid::EventSystem &eventSystem,
                           liquid::Window &window)
    : mEntityDatabase(entityDatabase), mEventSystem(eventSystem),
      mWindow(window) {
  reset();

  mMouseButtonReleaseHandler = mEventSystem.observe(
      liquid::MouseButtonEvent::Released,
      [this](const auto &data) { mInputState = InputState::None; });

  mMouseButtonPressHandler = mEventSystem.observe(
      liquid::MouseButtonEvent::Pressed, [this](const auto &data) {
        if (data.button != GLFW_MOUSE_BUTTON_MIDDLE || !mCaptureMouse) {
          return;
        }

        const auto &cursorPos = mWindow.getCurrentMousePosition();

        if (mWindow.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
          mInputState = InputState::Pan;
          mPrevMousePos = cursorPos;
        } else if (mWindow.isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
          mInputState = InputState::Zoom;
          mPrevMousePos = cursorPos;
        } else {
          mInputState = InputState::Rotate;
          mPrevMousePos = cursorPos;
        }
      });

  mMouseCursorMoveHandler = mEventSystem.observe(
      liquid::MouseCursorEvent::Moved, [this](const auto &data) {
        if (mInputState == InputState::None) {
          return;
        }

        static constexpr float MinOOBThreshold = 2.0f;
        const auto &size = mWindow.getWindowSize();

        float minX = 0;
        float maxX = static_cast<float>(size.x);
        float minY = 0;
        float maxY = static_cast<float>(size.y);

        bool outOfBounds = false;

        glm::vec2 newPos{data.xpos, data.ypos};

        if (data.xpos <= minX) {
          newPos.x = maxX - MinOOBThreshold;
          outOfBounds = true;
        } else if (data.xpos >= maxX) {
          newPos.x = minX + MinOOBThreshold;
          outOfBounds = true;
        }

        if (data.ypos <= minY) {
          newPos.y = maxY - MinOOBThreshold;
          outOfBounds = true;
        } else if (data.ypos >= maxY) {
          newPos.y = minY + MinOOBThreshold;
          outOfBounds = true;
        }

        if (outOfBounds) {
          mPrevMousePos = newPos;
          mWindow.setMousePosition(newPos);
        }
      });

  mMouseScrollHandler = mEventSystem.observe(
      liquid::MouseScrollEvent::Scroll, [this](const auto &event) {
        if (!mCaptureMouse)
          return;

        const auto &pos = mWindow.getCurrentMousePosition();
        if (pos.x < mX || pos.x > mX + mWidth || pos.y < mY ||
            pos.y > mY + mHeight) {
          return;
        }

        if (mInputState != InputState::None) {
          return;
        }
        glm::vec3 change =
            glm::vec3(mEye - mCenter) * event.yoffset * ZoomSpeed;
        mCenter += change;
        mEye += change;
      });
}

EditorCamera::~EditorCamera() {
  mEventSystem.removeObserver(liquid::MouseButtonEvent::Pressed,
                              mMouseButtonPressHandler);
  mEventSystem.removeObserver(liquid::MouseButtonEvent::Released,
                              mMouseButtonReleaseHandler);
  mEventSystem.removeObserver(liquid::MouseCursorEvent::Moved,
                              mMouseCursorMoveHandler);
  mEventSystem.removeObserver(liquid::MouseScrollEvent::Scroll,
                              mMouseScrollHandler);

  mEntityDatabase.deleteEntity(mCameraEntity);
}

void EditorCamera::setCenter(const glm::vec3 &center) { mCenter = center; }

void EditorCamera::setEye(const glm::vec3 &eye) { mEye = eye; }

void EditorCamera::setUp(const glm::vec3 &up) { mUp = up; }

void EditorCamera::update() {
  if (mInputState == InputState::Pan) {
    pan();
  } else if (mInputState == InputState::Rotate) {
    rotate();
  } else if (mInputState == InputState::Zoom) {
    zoom();
  }

  auto &camera = mEntityDatabase.get<liquid::Camera>(mCameraEntity);
  auto &lens = mEntityDatabase.get<liquid::PerspectiveLens>(mCameraEntity);

  camera.projectionMatrix = glm::perspective(
      glm::radians(lens.fovY), lens.aspectRatio, lens.near, lens.far);

  camera.viewMatrix = glm::lookAt(mEye, mCenter, mUp);
  camera.projectionViewMatrix = camera.projectionMatrix * camera.viewMatrix;
}

void EditorCamera::reset() {
  mEye = DefaultEye;
  mCenter = DefaultCenter;
  mUp = DefaultUp;

  if (!mEntityDatabase.exists(mCameraEntity)) {
    mCameraEntity = mEntityDatabase.create();
  }
  mEntityDatabase.set<liquid::PerspectiveLens>(
      mCameraEntity, {DefaultFOV, DefaultFar, DefaultNear});
  mEntityDatabase.set<liquid::Camera>(mCameraEntity, {});
}

glm::vec2 EditorCamera::scaleToViewport(const glm::vec2 &pos) const {
  float scaleX = static_cast<float>(mWindow.getFramebufferSize().x) / mWidth;
  float scaleY = static_cast<float>(mWindow.getFramebufferSize().y) / mHeight;

  float rX = pos.x - mX;
  float rY = pos.y - mY;

  return glm::vec2(rX * scaleX, rY * scaleY);
}

void EditorCamera::pan() {
  static constexpr float PanSpeed = 0.03f;

  glm::vec2 mousePos = mWindow.getCurrentMousePosition();
  glm::vec3 right = glm::normalize(glm::cross(glm::vec3(mEye - mCenter), mUp));

  glm::vec2 mousePosDiff =
      glm::vec4((mousePos - mPrevMousePos) * PanSpeed, 0.0f, 0.0f);

  glm::vec3 change = mUp * mousePosDiff.y + right * mousePosDiff.x;
  mEye += change;
  mCenter += change;
  mPrevMousePos = mousePos;
}

void EditorCamera::rotate() {
  static constexpr float TwoPi = 2.0f * glm::pi<float>();

  glm::vec2 mousePos = mWindow.getCurrentMousePosition();
  const auto &size = mWindow.getFramebufferSize();

  glm ::vec2 screenToSphere{// horizontal = 2pi
                            (TwoPi / static_cast<float>(size.x)),
                            // vertical = pi
                            (glm::pi<float>() / static_cast<float>(size.y))};

  // Convert mouse position difference to angle
  // difference for arcball
  glm::vec2 angleDiff = (mousePos - mPrevMousePos) * screenToSphere;
  glm::vec3 direction = glm::vec3(mEye - mCenter);
  glm::vec3 right = glm::normalize(glm::cross(direction, mUp));
  mUp = glm::normalize(glm::cross(right, direction));

  glm::mat4 rotationX = glm::rotate(glm::mat4{1.0f}, -angleDiff.x, mUp);
  glm::mat4 rotationY = glm::rotate(glm::mat4{1.0f}, angleDiff.y, right);

  mEye =
      glm::vec3(rotationY * (rotationX * glm::vec4(direction, 0.0f))) + mCenter;

  mPrevMousePos = mousePos;
}

void EditorCamera::zoom() {
  glm::vec2 mousePos = mWindow.getCurrentMousePosition();
  float zoomFactor = (mousePos.y - mPrevMousePos.y) * ZoomSpeed;

  glm::vec3 change = glm::vec3(mEye - mCenter) * zoomFactor;
  mCenter += change;
  mEye += change;
  mPrevMousePos = mousePos;
}

void EditorCamera::setViewport(float x, float y, float width, float height,
                               bool captureMouse) {
  mX = x;
  mY = y;
  mWidth = width;
  mHeight = height;

  getPerspectiveLens().aspectRatio = mWidth / mHeight;
  mCaptureMouse = captureMouse;
}

} // namespace liquidator
