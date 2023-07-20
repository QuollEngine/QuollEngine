#pragma once

#include "liquid/renderer/Renderer.h"
#include "liquid/window/Window.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/events/EventSystem.h"
#include "liquidator/core/CameraLookAt.h"
#include "liquidator/state/WorkspaceState.h"

namespace liquid::editor {

/**
 * @brief Editor camera
 *
 * Used as a controller that handles
 * mouse an keyboard events for panning,
 * rotating, and zooming.
 */
class EditorCamera {
public:
  /**
   * Current camera input state
   */
  enum class InputState {
    None = 0,
    Pan = 1,
    Rotate = 2,
    Zoom = 3,
    ZoomWheel = 4
  };

  /**
   * Zoom speed when scrolling
   */
  static constexpr float ZoomSpeed = 0.03f;

  /**
   * Default sensor size
   */
  static constexpr glm::vec2 DefaultSensorSize{16.0f, 16.0f};

  /**
   * Default focal length
   */
  static constexpr float DefaultFocalLength = 16.0f;

  /**
   * Default near perspective plane
   */
  static constexpr float DefaultNear = 0.1f;

  /**
   * Default far perspective plane
   */
  static constexpr float DefaultFar = 1000.0f;

  /**
   * Default camera position
   */
  static constexpr glm::vec3 DefaultEye{0.0f, 5.0f, -10.0f};

  /**
   * Default camera center
   */
  static constexpr glm::vec3 DefaultCenter{0.0f, 0.0f, 0.0f};

  /**
   * Default camera up vector
   */
  static constexpr glm::vec3 DefaultUp{0.0f, 1.0f, 0.0f};

public:
  /**
   * @brief Create editor camera
   *
   * @param eventSystem Event system
   * @param window Window
   */
  EditorCamera(EventSystem &eventSystem, Window &window);

  EditorCamera(const EditorCamera &) = delete;
  EditorCamera &operator=(const EditorCamera &) = delete;
  EditorCamera(EditorCamera &&) = delete;
  EditorCamera &operator=(EditorCamera &&) = delete;

  /**
   * @brief Destroy editor camera
   *
   * Cleans up all handlers
   */
  ~EditorCamera();

  /**
   * @brief Update camera
   *
   * @param state Workspace state
   */
  void update(WorkspaceState &state);

  /**
   * @brief Set viewport
   *
   * @param x Viewport x position
   * @param y Viewport y position
   * @param width Viewport width
   * @param height Viewport height
   * @param captureMouse Capture mouse for camera controls
   */
  void setViewport(float x, float y, float width, float height,
                   bool captureMouse);

  /**
   * @brief Check if position is within viewport
   *
   * @param pos Position
   * @retval true Position is within viewport
   * @retval false Position is outside viewport
   */
  inline bool isWithinViewport(const glm::vec2 &pos) const {
    return (pos.x >= mX && pos.x <= mX + mWidth && pos.y >= mY &&
            pos.y <= mY + mHeight);
  }

  /**
   * @brief Scale position to viewport
   *
   * @param pos Position
   * @return Scaled position
   */
  glm::vec2 scaleToViewport(const glm::vec2 &pos) const;

  /**
   * @brief Get input state
   *
   * @return Input state
   */
  inline const InputState &getInputState() const { return mInputState; }

  /**
   * @brief Create default camera
   *
   * @param entityDatabase Entity database
   * @return Camera entity
   */
  Entity createDefaultCamera(EntityDatabase &entityDatabase);

private:
  /**
   * @brief Pan camera using mouse movement
   *
   * @param lookAt Camera look at
   */
  void pan(CameraLookAt &lookAt);

  /**
   * @brief Rotate camera using mouse movement
   *
   * @param lookAt Camera look at
   */
  void rotate(CameraLookAt &lookAt);

  /**
   * @brief Zoom camera using mouse movement
   *
   * @param lookAt Camera look at
   */
  void zoom(CameraLookAt &lookAt);

  /**
   * @brief Zoom camera using mouse wheel
   *
   * @param lookAt Camera look at
   */
  void zoomWheel(CameraLookAt &lookAt);

private:
  float mX = 0.0f;
  float mY = 0.0f;
  float mWidth = 1.0f;
  float mHeight = 1.0f;
  float mWheelOffset = 0.0f;
  bool mCaptureMouse = false;

  InputState mInputState = InputState::None;
  glm::vec2 mPrevMousePos{};

  EventObserverId mMouseButtonPressHandler = 0;
  EventObserverId mMouseButtonReleaseHandler = 0;
  EventObserverId mMouseCursorMoveHandler = 0;
  EventObserverId mMouseScrollHandler = 0;

  Window &mWindow;
  EventSystem &mEventSystem;
};

} // namespace liquid::editor
