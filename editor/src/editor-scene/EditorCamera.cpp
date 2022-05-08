#include "liquid/core/Base.h"
#include "EditorCamera.h"

#include <GLFW/glfw3.h>

using liquid::Renderer;
using liquid::Window;

namespace liquidator {

EditorCamera::EditorCamera(liquid::EntityContext &entityContext,
                           liquid::EventSystem &eventSystem,
                           liquid::Renderer &renderer, liquid::Window &window)
    : mEntityContext(entityContext), mEventSystem(eventSystem),
      mWindow(window) {

  mMouseButtonReleaseHandler = mEventSystem.observe(
      liquid::MouseButtonEvent::Released,
      [this](const auto &data) { mInputState = InputState::None; });

  mMouseButtonPressHandler = mEventSystem.observe(
      liquid::MouseButtonEvent::Pressed, [this](const auto &data) {
        if (data.button != GLFW_MOUSE_BUTTON_MIDDLE) {
          return;
        }

        const auto &cursorPos = mWindow.getCurrentMousePosition();

        // Do not trigger action if cursor is outside
        // Imgui window viewport
        if (cursorPos.x < mX || cursorPos.x > mX + mWidth || cursorPos.y < mY ||
            cursorPos.y > mY + mHeight) {
          return;
        }

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

        constexpr float MIN_OOB_THRESHOLD = 2.0f;
        const auto &size = mWindow.getWindowSize();

        float minX = 0;
        float maxX = static_cast<float>(size.x);
        float minY = 0;
        float maxY = static_cast<float>(size.y);

        bool outOfBounds = false;

        glm::vec2 newPos{data.xpos, data.ypos};

        if (data.xpos <= minX) {
          newPos.x = maxX - MIN_OOB_THRESHOLD;
          outOfBounds = true;
        } else if (data.xpos >= maxX) {
          newPos.x = minX + MIN_OOB_THRESHOLD;
          outOfBounds = true;
        }

        if (data.ypos <= minY) {
          newPos.y = maxY - MIN_OOB_THRESHOLD;
          outOfBounds = true;
        } else if (data.ypos >= maxY) {
          newPos.y = minY + MIN_OOB_THRESHOLD;
          outOfBounds = true;
        }

        if (outOfBounds) {
          mPrevMousePos = newPos;
          mWindow.setMousePosition(newPos);
        }
      });

  mMouseScrollHandler = mEventSystem.observe(
      liquid::MouseScrollEvent::Scroll, [this](const auto &event) {
        const auto &pos = mWindow.getCurrentMousePosition();
        if (pos.x < mX || pos.x > mX + mWidth || pos.y < mY ||
            pos.y > mY + mHeight) {
          return;
        }

        if (mInputState != InputState::None) {
          return;
        }
        glm::vec3 change =
            glm::vec3(mEye - mCenter) * event.yoffset * ZOOM_SPEED;
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

  mEntityContext.deleteComponent<liquid::CameraComponent>(mCameraEntity);
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

  auto &camera =
      mEntityContext.getComponent<liquid::CameraComponent>(mCameraEntity);

  camera.projectionMatrix = glm::perspective(
      mFov, static_cast<float>(mWidth) / static_cast<float>(mHeight), mNear,
      mFar);
  camera.projectionMatrix[1][1] *= -1;

  camera.viewMatrix = glm::lookAt(mEye, mCenter, mUp);
  camera.projectionViewMatrix = camera.projectionMatrix * camera.viewMatrix;
}

void EditorCamera::reset() {
  mEye = DEFAULT_EYE;
  mCenter = DEFAULT_CENTER;
  mUp = DEFAULT_UP;
  mFov = DEFAULT_FOV;
  mNear = DEFAULT_NEAR;
  mFar = DEFAULT_FAR;

  if (!mEntityContext.hasEntity(mCameraEntity)) {
    mCameraEntity = mEntityContext.createEntity();
  }
  mEntityContext.setComponent<liquid::CameraComponent>(mCameraEntity, {});
}

void EditorCamera::pan() {
  constexpr float PAN_SPEED = 0.03f;

  glm::vec2 mousePos = mWindow.getCurrentMousePosition();
  glm::vec3 right = glm::normalize(glm::cross(glm::vec3(mEye - mCenter), mUp));

  glm::vec2 mousePosDiff =
      glm::vec4((mousePos - mPrevMousePos) * PAN_SPEED, 0.0f, 0.0f);

  glm::vec3 change = mUp * mousePosDiff.y + right * mousePosDiff.x;
  mEye += change;
  mCenter += change;
  mPrevMousePos = mousePos;
}

void EditorCamera::rotate() {
  constexpr float TWO_PI = 2.0f * glm::pi<float>();

  glm::vec2 mousePos = mWindow.getCurrentMousePosition();
  const auto &size = mWindow.getFramebufferSize();

  glm ::vec2 screenToSphere{// horizontal = 2pi
                            (TWO_PI / static_cast<float>(size.x)),
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
  float zoomFactor = (mousePos.y - mPrevMousePos.y) * ZOOM_SPEED;

  glm::vec3 change = glm::vec3(mEye - mCenter) * zoomFactor;
  mCenter += change;
  mEye += change;
  mPrevMousePos = mousePos;
}

void EditorCamera::setViewport(float x, float y, float width, float height) {
  mX = x;
  mY = y;
  mWidth = width;
  mHeight = height;
}

} // namespace liquidator
