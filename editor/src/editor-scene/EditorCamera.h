#pragma once

#include "liquid/renderer/Renderer.h"
#include "liquid/window/Window.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/events/EventSystem.h"

namespace liquidator {

class EditorCamera {
public:
  enum class InputState { None = 0, Pan = 1, Rotate = 2, Zoom = 3 };

  static constexpr float ZOOM_SPEED = 0.03f;
  static constexpr float DEFAULT_FOV = 70.0f;
  static constexpr float DEFAULT_NEAR = 0.001f;
  static constexpr float DEFAULT_FAR = 1000.0f;
  static constexpr glm::vec3 DEFAULT_EYE{0.0f, 5.0f, -10.0f};
  static constexpr glm::vec3 DEFAULT_CENTER{0.0f, 0.0f, 0.0f};
  static constexpr glm::vec3 DEFAULT_UP{0.0f, 1.0f, 0.0f};

public:
  /**
   * @brief Create editor camera
   *
   * @param entityContext Entity context
   * @param eventSystem Event system
   * @param renderer Renderer
   * @param window Window
   */
  EditorCamera(liquid::EntityContext &entityContext,
               liquid::EventSystem &eventSystem, liquid::Renderer &renderer,
               liquid::Window &window);

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
  inline void setFOV(float fov) { mFov = fov; }

  /**
   * @brief Set near plane
   *
   * @param near Near plane
   */
  inline void setNear(float near) { mNear = near; }

  /**
   * @brief Set far plane
   *
   * @param Far plane
   */
  inline void setFar(float far) { mFar = far; }

  /**
   * @brief Get field of view
   *
   * @return Field of view
   */
  inline float getFOV() { return mFov; }

  /**
   * @brief Get near place
   *
   * @return Near plane
   */
  inline float getNear() { return mNear; }

  /**
   * @brief Get far plane
   *
   * @return Far plane
   */
  inline float getFar() { return mFar; }

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
   */
  void setViewport(float x, float y, float width, float height);

  /**
   * @brief Get input state
   *
   * @return Input state
   */
  inline const InputState &getInputState() const { return mInputState; }

private:
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
  float mFov = DEFAULT_FOV;
  float mNear = DEFAULT_NEAR;
  float mFar = DEFAULT_FAR;

  float mX = 0.0f;
  float mY = 0.0f;
  float mWidth = 0.0f;
  float mHeight = 0.0f;

  InputState mInputState = InputState::None;
  glm::vec2 mPrevMousePos{};

  glm::vec3 mEye = DEFAULT_EYE;
  glm::vec3 mCenter = DEFAULT_CENTER;
  glm::vec3 mUp = DEFAULT_UP;

  liquid::EventObserverId mMouseButtonPressHandler = 0;
  liquid::EventObserverId mMouseButtonReleaseHandler = 0;
  liquid::EventObserverId mMouseCursorMoveHandler = 0;
  liquid::EventObserverId mMouseScrollHandler = 0;

  liquid::Window &mWindow;
  liquid::EntityContext &mEntityContext;
  liquid::EventSystem &mEventSystem;
  liquid::Entity mCameraEntity = liquid::ENTITY_MAX;
};

} // namespace liquidator
