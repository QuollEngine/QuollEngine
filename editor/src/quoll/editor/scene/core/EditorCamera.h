#pragma once

#include "quoll/entity/EntityDatabase.h"
#include "quoll/window/Window.h"
#include "quoll/editor/core/CameraLookAt.h"
#include "quoll/editor/workspace/WorkspaceState.h"

namespace quoll::editor {

class EditorCamera {
public:
  enum class InputState {
    None = 0,
    Pan = 1,
    Rotate = 2,
    Zoom = 3,
    ZoomWheel = 4
  };

  static constexpr f32 ZoomSpeed = 0.03f;

  static constexpr glm::vec2 DefaultSensorSize{16.0f, 16.0f};

  static constexpr f32 DefaultFocalLength = 16.0f;

  static constexpr f32 DefaultNear = 0.1f;

  static constexpr f32 DefaultFar = 1000.0f;

  static constexpr glm::vec3 DefaultEye{0.0f, 5.0f, -10.0f};

  static constexpr glm::vec3 DefaultCenter{0.0f, 0.0f, 0.0f};

  static constexpr glm::vec3 DefaultUp{0.0f, 1.0f, 0.0f};

public:
  static Entity createDefaultCamera(EntityDatabase &entityDatabase);

public:
  EditorCamera(Window &window);

  EditorCamera(const EditorCamera &) = delete;
  EditorCamera &operator=(const EditorCamera &) = delete;
  EditorCamera(EditorCamera &&) = delete;
  EditorCamera &operator=(EditorCamera &&) = delete;

  ~EditorCamera();

  void update(WorkspaceState &state);

  void setViewport(f32 x, f32 y, f32 width, f32 height, bool captureMouse);

  inline bool isWithinViewport(const glm::vec2 &pos) const {
    return (pos.x >= mX && pos.x <= mX + mWidth && pos.y >= mY &&
            pos.y <= mY + mHeight);
  }

  glm::vec2 scaleToViewport(const glm::vec2 &pos) const;

  inline const InputState &getInputState() const { return mInputState; }

private:
  void pan(CameraLookAt &lookAt);

  void rotate(CameraLookAt &lookAt);

  void zoom(CameraLookAt &lookAt);

  void zoomWheel(CameraLookAt &lookAt);

private:
  f32 mX = 0.0f;
  f32 mY = 0.0f;
  f32 mWidth = 1.0f;
  f32 mHeight = 1.0f;
  f32 mWheelOffset = 0.0f;
  bool mCaptureMouse = false;

  InputState mInputState = InputState::None;
  glm::vec2 mPrevMousePos{};

  SignalSlot mOnMousePressSlot;
  SignalSlot mOnMouseReleaseSlot;
  SignalSlot mOnMouseMoveSlot;
  SignalSlot mOnMouseScrollSlot;

  Window &mWindow;
};

} // namespace quoll::editor
