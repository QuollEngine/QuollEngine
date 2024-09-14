#include "quoll/core/Base.h"
#include "quoll/scene/Camera.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll/editor/core/CameraLookAt.h"
#include "EditorCamera.h"
#include <GLFW/glfw3.h>

namespace quoll::editor {

Entity EditorCamera::createDefaultCamera(EntityDatabase &entityDatabase) {
  auto camera = entityDatabase.create();

  PerspectiveLens lens{};
  lens.near = DefaultNear;
  lens.far = DefaultFar;
  lens.sensorSize = DefaultSensorSize;
  lens.focalLength = DefaultFocalLength;

  entityDatabase.set(camera, lens);
  entityDatabase.set<Camera>(camera, {});
  entityDatabase.set<CameraLookAt>(camera,
                                   {DefaultEye, DefaultCenter, DefaultUp});

  return camera;
}

EditorCamera::EditorCamera(Window &window) : mWindow(window) {
  mOnMouseReleaseSlot = mWindow.getSignals().onMouseRelease().connect(
      [this](const auto &data) { mInputState = InputState::None; });

  mOnMousePressSlot =
      mWindow.getSignals().onMousePress().connect([this](const auto &data) {
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

  mOnMouseMoveSlot =
      mWindow.getSignals().onMouseMove().connect([this](const auto &data) {
        if (mInputState == InputState::None ||
            mInputState == InputState::ZoomWheel) {
          return;
        }

        static constexpr f32 MinOOBThreshold = 2.0f;
        const auto &size = mWindow.getFramebufferSize();

        f32 minX = 0;
        f32 maxX = static_cast<f32>(size.x);
        f32 minY = 0;
        f32 maxY = static_cast<f32>(size.y);

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

  mOnMouseScrollSlot =
      mWindow.getSignals().onMouseScroll().connect([this](const auto &event) {
        if (!mCaptureMouse)
          return;

        const auto &pos = mWindow.getCurrentMousePosition();
        if (pos.x < mX || pos.x > mX + mWidth || pos.y < mY ||
            pos.y > mY + mHeight) {
          return;
        }

        if (mInputState != InputState::None &&
            mInputState != InputState::ZoomWheel) {
          return;
        }

        mInputState = InputState::ZoomWheel;
        mWheelOffset = event.yoffset;
      });
}

EditorCamera::~EditorCamera() {
  mOnMousePressSlot.disconnect();
  mOnMouseReleaseSlot.disconnect();
  mOnMouseMoveSlot.disconnect();
  mOnMouseScrollSlot.disconnect();
}

void EditorCamera::update(WorkspaceState &state) {
  auto &scene = state.scene;

  auto &lookAt = scene.entityDatabase.get<CameraLookAt>(state.camera);

  if (mInputState == InputState::Pan) {
    pan(lookAt);
  } else if (mInputState == InputState::Rotate) {
    rotate(lookAt);
  } else if (mInputState == InputState::Zoom) {
    zoom(lookAt);
  } else if (mInputState == InputState::ZoomWheel) {
    zoomWheel(lookAt);
  }

  auto &camera = scene.entityDatabase.get<Camera>(state.camera);
  auto &lens = scene.entityDatabase.get<PerspectiveLens>(state.camera);

  lens.aspectRatio = mWidth / mHeight;

  const f32 fovY = 2.0f * atanf(lens.sensorSize.y / (2.0f * lens.focalLength));

  camera.projectionMatrix =
      glm::perspective(fovY, lens.aspectRatio, lens.near, lens.far);

  camera.viewMatrix = glm::lookAt(lookAt.eye, lookAt.center, lookAt.up);
  camera.projectionViewMatrix = camera.projectionMatrix * camera.viewMatrix;

  static constexpr f32 Ev100ForOneExposure = -0.263034f;
  camera.exposure.x = Ev100ForOneExposure;
}

glm::vec2 EditorCamera::scaleToViewport(const glm::vec2 &pos) const {
  f32 scaleX = static_cast<f32>(mWindow.getFramebufferSize().x) / mWidth;
  f32 scaleY = static_cast<f32>(mWindow.getFramebufferSize().y) / mHeight;

  f32 rX = pos.x - mX;
  f32 rY = pos.y - mY;

  return glm::vec2(rX * scaleX, rY * scaleY);
}

void EditorCamera::pan(CameraLookAt &lookAt) {
  static constexpr f32 PanSpeed = 0.03f;

  glm::vec2 mousePos = mWindow.getCurrentMousePosition();
  glm::vec3 right = glm::normalize(
      glm::cross(glm::vec3(lookAt.eye - lookAt.center), lookAt.up));

  glm::vec2 mousePosDiff =
      glm::vec4((mousePos - mPrevMousePos) * PanSpeed, 0.0f, 0.0f);

  glm::vec3 change = lookAt.up * mousePosDiff.y + right * mousePosDiff.x;
  lookAt.eye += change;
  lookAt.center += change;
  mPrevMousePos = mousePos;
}

void EditorCamera::rotate(CameraLookAt &lookAt) {
  static constexpr f32 TwoPi = 2.0f * glm::pi<f32>();

  glm::vec2 mousePos = mWindow.getCurrentMousePosition();
  const auto &size = mWindow.getFramebufferSize();

  glm ::vec2 screenToSphere{// horizontal = 2pi
                            (TwoPi / static_cast<f32>(size.x)),
                            // vertical = pi
                            (glm::pi<f32>() / static_cast<f32>(size.y))};

  // Convert mouse position difference to angle
  // difference for arcball
  glm::vec2 angleDiff = (mousePos - mPrevMousePos) * screenToSphere;
  glm::vec3 direction = glm::vec3(lookAt.eye - lookAt.center);
  glm::vec3 right = glm::normalize(glm::cross(direction, lookAt.up));
  lookAt.up = glm::normalize(glm::cross(right, direction));

  glm::mat4 rotationX = glm::rotate(glm::mat4{1.0f}, -angleDiff.x, lookAt.up);
  glm::mat4 rotationY = glm::rotate(glm::mat4{1.0f}, angleDiff.y, right);

  lookAt.eye = glm::vec3(rotationY * (rotationX * glm::vec4(direction, 0.0f))) +
               lookAt.center;

  mPrevMousePos = mousePos;
}

void EditorCamera::zoom(CameraLookAt &lookAt) {
  glm::vec2 mousePos = mWindow.getCurrentMousePosition();
  f32 zoomFactor = (mousePos.y - mPrevMousePos.y) * ZoomSpeed;

  glm::vec3 change = glm::vec3(lookAt.eye - lookAt.center) * zoomFactor;
  lookAt.center += change;
  lookAt.eye += change;
  mPrevMousePos = mousePos;
}

void EditorCamera::zoomWheel(CameraLookAt &lookAt) {
  glm::vec3 change =
      glm::vec3(lookAt.eye - lookAt.center) * mWheelOffset * ZoomSpeed;
  lookAt.center += change;
  lookAt.eye += change;

  mWheelOffset = 0.0f;
  mInputState = InputState::None;
}

void EditorCamera::setViewport(f32 x, f32 y, f32 width, f32 height,
                               bool captureMouse) {
  mX = x;
  mY = y;
  mWidth = width;
  mHeight = height;
  mCaptureMouse = captureMouse;
}

} // namespace quoll::editor
