#pragma once

#include "liquid/rhi/ResourceRegistry.h"

namespace liquid {

struct CameraData {
  glm::mat4 projectionMatrix{1.0};
  glm::mat4 viewMatrix{1.0};
  glm::mat4 projectionViewMatrix{1.0};
};

class Camera {
public:
  /**
   * @brief Create camera
   *
   * Creates camera and uniform buffer for renderer
   *
   * @param registry Resource registry
   */
  Camera(rhi::ResourceRegistry *registry);

  /**
   * @brief Sets prespective projection
   *
   * @param fovY Field of View (Y-Axis) in degrees
   * @param aspectRatio Aspect Ratio
   * @param near Near Distance
   * @param far Far Distance
   */
  void setPerspective(float fovY, float aspectRatio, float near, float far);

  /**
   * @brief Sets camera orientation
   *
   * @param eye Camera position
   * @param center Camera direction
   * @param up Camera up vector
   */
  void lookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);

  /**
   * @brief Get Projection View Matrix
   *
   * @return Projection View matrix
   */
  inline const glm::mat4 &getProjectionViewMatrix() {
    return mData.projectionViewMatrix;
  }

  /**
   * @brief Get Projection Matrix
   *
   * @return Projection matrix
   */
  inline const glm::mat4 &getProjectionMatrix() {
    return mData.projectionMatrix;
  }

  /**
   * @brief Get View Matrix
   *
   * @return View matrix
   */
  inline const glm::mat4 &getViewMatrix() { return mData.viewMatrix; }

  /**
   * @brief Get uniform buffer
   *
   * @return Uniform buffer
   */
  inline rhi::BufferHandle getBuffer() { return mBuffer; }

  /**
   * @brief Get camera data
   *
   * @return Camera data
   */
  inline const CameraData &getCameraData() { return mData; }

private:
  /**
   * @brief Updates projection view matrix
   */
  void updateProjectionViewMatrix();

private:
  CameraData mData;
  rhi::BufferHandle mBuffer;
  rhi::ResourceRegistry *mRegistry;
};

} // namespace liquid
