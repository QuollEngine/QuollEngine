#include "liquid/core/Base.h"
#include "Camera.h"

namespace liquid {

Camera::Camera(rhi::ResourceRegistry *registry_) : registry(registry_) {
  uniformBuffer =
      registry->setBuffer({rhi::BufferType::Uniform, sizeof(CameraData)});
}

void Camera::setPerspective(float fovY, float aspectRatio, float near,
                            float far) {
  data.projectionMatrix =
      glm::perspective(glm::radians(fovY), aspectRatio, near, far);

  data.projectionMatrix[1][1] *= -1;

  updateProjectionViewMatrix();

  registry->setBuffer({rhi::BufferType::Uniform, sizeof(CameraData), &data},
                      uniformBuffer);
}

void Camera::lookAt(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
  data.viewMatrix = glm::lookAt(position, direction, up);
  updateProjectionViewMatrix();

  registry->setBuffer({rhi::BufferType::Uniform, sizeof(CameraData), &data},
                      uniformBuffer);
}

void Camera::updateProjectionViewMatrix() {
  data.projectionViewMatrix = data.projectionMatrix * data.viewMatrix;
}

} // namespace liquid
