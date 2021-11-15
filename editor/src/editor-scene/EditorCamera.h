#pragma once

#include "scene/Camera.h"
#include "renderer/vulkan/VulkanRenderer.h"
#include "window/glfw/GLFWWindow.h"

namespace liquidator {

class EditorCamera {
  enum class InputState { None = 0, Pan = 1, Rotate = 2, Zoom = 3 };

  static constexpr float DEFAULT_FOV = 70.0f;
  static constexpr float DEFAULT_NEAR = 0.001f;
  static constexpr float DEFAULT_FAR = 1000.0f;
  static constexpr glm::vec3 DEFAULT_EYE{0.0f, 0.0f, -10.0f};

public:
  /**
   * @brief Create editor camera
   *
   * @param renderer Renderer
   * @param window Window
   */
  EditorCamera(liquid::VulkanRenderer *renderer, liquid::GLFWWindow *window);

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
  inline void setFOV(float fov_) { fov = fov_; }

  /**
   * @brief Set near plane
   *
   * @param near Near plane
   */
  inline void setNear(float near_) { near = near_; }

  /**
   * @brief Set far plane
   *
   * @param Far plane
   */
  inline void setFar(float far_) { far = far_; }

  /**
   * @brief Get field of view
   *
   * @return Field of view
   */
  inline float getFOV() { return fov; }

  /**
   * @brief Get near place
   *
   * @return Near plane
   */
  inline float getNear() { return near; }

  /**
   * @brief Get far plane
   *
   * @return Far plane
   */
  inline float getFar() { return far; }

  /**
   * @brief Get camera
   *
   * @return Camera
   */
  inline const liquid::SharedPtr<liquid::Camera> &getCamera() { return camera; }

  /**
   * @brief Update camera
   */
  void update();

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
   * @brief Update perspective based on framebuffer size
   */
  void updatePerspectiveBasedOnFramebuffer();

  /**
   * @brief Update perspective
   *
   * @param aspectRatio Aspect ratio
   */
  void updatePerspective(float aspectRatio);

private:
  float fov = DEFAULT_FOV;
  float near = DEFAULT_NEAR;
  float far = DEFAULT_FAR;

  InputState inputState = InputState::None;
  glm::vec2 prevMousePos{};

  glm::vec3 eye = DEFAULT_EYE;
  glm::vec3 center{0.0f, 0.0f, 0.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};

  uint32_t resizeHandler;
  uint32_t mouseButtonHandler;
  uint32_t mouseMoveHandler;

  liquid::GLFWWindow *window;
  liquid::SharedPtr<liquid::Camera> camera;
};

} // namespace liquidator
