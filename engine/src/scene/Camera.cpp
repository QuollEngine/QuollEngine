#include "Camera.h"

namespace liquid {

Camera::Camera(ResourceAllocator *resourceAllocator) {
  uniformBuffer = resourceAllocator->createUniformBuffer(sizeof(CameraData));
}

void Camera::setPerspective(float fovY, float aspectRatio, float near,
                            float far) {
  data.projectionMatrix =
      glm::perspective(glm::radians(fovY), aspectRatio, near, far);

  data.projectionMatrix[1][1] *= -1;

  updateProjectionViewMatrix();
  uniformBuffer->update(&data);
}

void Camera::lookAt(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
  data.viewMatrix = glm::lookAt(position, direction, up);
  updateProjectionViewMatrix();
  uniformBuffer->update(&data);
}

void Camera::updateProjectionViewMatrix() {
  data.projectionViewMatrix = data.projectionMatrix * data.viewMatrix;
}

} // namespace liquid
