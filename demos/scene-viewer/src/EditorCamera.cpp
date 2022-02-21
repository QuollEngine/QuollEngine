#include "liquid/core/Base.h"
#include "EditorCamera.h"

using liquid::Camera;
using liquid::GLFWWindow;
using liquid::VulkanRenderer;

EditorCamera::EditorCamera(liquid::EntityContext &entityContext_,
                           VulkanRenderer *renderer, GLFWWindow *window_)
    : entityContext(entityContext_), window(window_),
      camera(new Camera(renderer->getResourceAllocator())) {

  initEntity();

  updatePerspective();
  window->addResizeHandler([this](uint32_t width, uint32_t height) {
    updatePerspective(width, height);
  });
}

void EditorCamera::move(float speed) { moveSpeed = speed; }

void EditorCamera::strafe(float speed) { strafeSpeed = speed; }

void EditorCamera::yaw(float speed) { yawSpeed = speed; }

void EditorCamera::pitch(float speed) { pitchSpeed = speed; }

void EditorCamera::initEntity() {
  cameraEntity = entityContext.createEntity();
  entityContext.setComponent<liquid::CameraComponent>(cameraEntity, {camera});
}

void EditorCamera::update() {
  rotation += glm::vec2{yawSpeed, pitchSpeed};
  rotation.y = rotation.y >= 89.9f ? 89.9f : rotation.y;
  rotation.y = rotation.y <= -89.9f ? -89.9f : rotation.y;

  glm::vec3 direction =
      glm::vec3{cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y)),
                sin(glm::radians(rotation.y)),
                sin(glm::radians(rotation.x)) * cos(glm::radians(rotation.y))};

  glm::vec3 up{0.0f, 1.0f, 0.0f};
  position += direction * moveSpeed;

  glm::vec3 right = glm::cross(direction, up);
  position += right * strafeSpeed;

  camera->lookAt(position, position + direction, up);
}

void EditorCamera::updatePerspective() {
  const auto &fbSize = window->getFramebufferSize();
  updatePerspective(fbSize.width, fbSize.height);
}

void EditorCamera::updatePerspective(uint32_t width, uint32_t height) {
  camera->setPerspective(fov, static_cast<float>(width) / height, near, far);
}
