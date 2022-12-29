#pragma once

#include "liquid/renderer/Renderer.h"
#include "liquid/window/Window.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/events/EventSystem.h"

namespace liquidator {

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
  enum class InputState { None = 0, Pan = 1, Rotate = 2, Zoom = 3 };

  /**
   * Zoom speed when scrolling
   */
  static constexpr float ZoomSpeed = 0.03f;

  /**
   * Default Field of view value
   */
  static constexpr float DefaultFOV = 70.0f;

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
   * @param entityDatabase Entity database
   * @param eventSystem Event system
   * @param window Window
   */
  EditorCamera(liquid::EntityDatabase &entityDatabase,
               liquid::EventSystem &eventSystem, liquid::Window &window);

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
   * @brief Set field of view
   *
   * @param fov Field of view
   */
  void setFOV(float fov) { getPerspectiveLens().fovY = fov; }

  /**
   * @brief Set near plane
   *
   * @param near Near plane
   */
  void setNear(float near) { getPerspectiveLens().near = near; }

  /**
   * @brief Set far plane
   *
   * @param far Far plane
   */
  void setFar(float far) { getPerspectiveLens().far = far; }

  /**
   * @brief Get field of view
   *
   * @return Field of view
   */
  inline float getFOV() const { return getPerspectiveLens().fovY; }

  /**
   * @brief Get near place
   *
   * @return Near plane
   */
  inline float getNear() const { return getPerspectiveLens().near; }

  /**
   * @brief Get far plane
   *
   * @return Far plane
   */
  inline float getFar() const { return getPerspectiveLens().far; }

  /**
   * @brief Get aspect ratio
   *
   * @return Aspect ratio
   */
  inline float getAspectRatio() const {
    return getPerspectiveLens().aspectRatio;
  }

  /**
   * @brief Get position
   *
   * @return Camera position
   */
  inline const glm::vec3 &getEye() const { return mEye; }

  /**
   * @brief Get camera center
   *
   * @return Camera center
   */
  inline const glm::vec3 &getCenter() const { return mCenter; }

  /**
   * @brief Get camera up vector
   *
   * @return Camera up vector
   */
  inline const glm::vec3 &getUp() const { return mUp; }

  /**
   * @brief Get camera
   *
   * @return Camera
   */
  inline const liquid::Entity getCamera() const { return mCameraEntity; }

  /**
   * @brief Set camera center
   *
   * @param center Camera center
   */
  void setCenter(const glm::vec3 &center);

  /**
   * @brief Set camera eye position
   *
   * @param eye Eye vector
   */
  void setEye(const glm::vec3 &eye);

  /**
   * @brief Set camera up vector
   *
   * @param up Up vector
   */
  void setUp(const glm::vec3 &up);

  /**
   * @brief Update camera
   */
  void update();

  /**
   * @brief Reset camera to defaults
   */
  void reset();

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

private:
  /**
   * @brief Get perspective lens
   *
   * @return Perspective lens
   */
  inline liquid::PerspectiveLens &getPerspectiveLens() {
    return mEntityDatabase.get<liquid::PerspectiveLens>(mCameraEntity);
  }

  /**
   * @brief Get perspective lens
   *
   * @return Perspective lens
   */
  inline const liquid::PerspectiveLens &getPerspectiveLens() const {
    return mEntityDatabase.get<liquid::PerspectiveLens>(mCameraEntity);
  }

  /**
   * @brief Pan camera using mouse movement
   */
  void pan();

  /**
   * @brief Rotate camera using mouse movement
   */
  void rotate();

  /**
   * @brief Zoom camera using mouse movement
   */
  void zoom();

private:
  float mX = 0.0f;
  float mY = 0.0f;
  float mWidth = 0.0f;
  float mHeight = 0.0f;
  bool mCaptureMouse = false;

  InputState mInputState = InputState::None;
  glm::vec2 mPrevMousePos{};

  glm::vec3 mEye = DefaultEye;
  glm::vec3 mCenter = DefaultCenter;
  glm::vec3 mUp = DefaultUp;

  liquid::EventObserverId mMouseButtonPressHandler = 0;
  liquid::EventObserverId mMouseButtonReleaseHandler = 0;
  liquid::EventObserverId mMouseCursorMoveHandler = 0;
  liquid::EventObserverId mMouseScrollHandler = 0;

  liquid::Window &mWindow;
  liquid::EntityDatabase &mEntityDatabase;
  liquid::EventSystem &mEventSystem;
  liquid::Entity mCameraEntity = liquid::EntityNull;
};

} // namespace liquidator
