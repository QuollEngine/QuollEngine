#pragma once

#include "liquid/scene/Camera.h"
#include "liquid/renderer/Renderer.h"
#include "liquid/window/Window.h"
#include "liquid/entity/EntityContext.h"

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
   * @param renderer Renderer
   * @param window Window
   */
  EditorCamera(liquid::EntityContext &entityContext, liquid::Renderer &renderer,
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
   * @brief Get camera
   *
   * @return Camera
   */
  inline const liquid::Entity getCamera() const { return mCameraEntity; }

  /**
   * @brief Set camera center
   *
   * @param center Center vector
   */
  void setCenter(const glm::vec3 &center);

  /**
   * @brief Set camera eye position
   *
   * @param eye Eye vector
   */
  void setEye(const glm::vec3 &eye);

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

  /**
   * @brief Update perspective
   *
   * @param aspectRatio Aspect ratio
   */
  void updatePerspective(float aspectRatio);

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

  uint32_t mMouseButtonHandler = 0;
  uint32_t mMouseMoveHandler = 0;
  uint32_t mScrollWheelHandler = 0;

  liquid::Window &mWindow;
  liquid::SharedPtr<liquid::Camera> mCamera;
  liquid::EntityContext &mEntityContext;
  liquid::Entity mCameraEntity = liquid::ENTITY_MAX;
};

} // namespace liquidator
