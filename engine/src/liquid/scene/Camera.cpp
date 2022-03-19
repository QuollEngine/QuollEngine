#include "liquid/core/Base.h"
#include "Camera.h"

namespace liquid {

Camera::Camera(rhi::ResourceRegistry *registry) : mRegistry(registry) {
  mBuffer =
      mRegistry->setBuffer({rhi::BufferType::Uniform, sizeof(CameraData)});
}

void Camera::setPerspective(float fovY, float aspectRatio, float near,
                            float far) {
  mData.projectionMatrix =
      glm::perspective(glm::radians(fovY), aspectRatio, near, far);

  mData.projectionMatrix[1][1] *= -1;

  updateProjectionViewMatrix();

  mRegistry->setBuffer({rhi::BufferType::Uniform, sizeof(CameraData), &mData},
                       mBuffer);
}

void Camera::lookAt(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
  mData.viewMatrix = glm::lookAt(position, direction, up);
  updateProjectionViewMatrix();

  mRegistry->setBuffer({rhi::BufferType::Uniform, sizeof(CameraData), &mData},
                       mBuffer);
}

void Camera::updateProjectionViewMatrix() {
  mData.projectionViewMatrix = mData.projectionMatrix * mData.viewMatrix;
}

} // namespace liquid
