#include "EditorCamera.h"

using liquid::Camera;
using liquid::GLFWWindow;
using liquid::VulkanRenderer;

namespace liquidator {

EditorCamera::EditorCamera(liquid::EntityContext &context_,
                           liquid::VulkanRenderer *renderer,
                           liquid::GLFWWindow *window_)
    : context(context_), window(window_),
      camera(new Camera(renderer->getResourceAllocator())) {

  updatePerspectiveBasedOnFramebuffer();
  resizeHandler = window->addResizeHandler([this](uint32_t width,
                                                  uint32_t height) {
    updatePerspective(static_cast<float>(width) / static_cast<float>(height));
  });

  mouseButtonHandler = window->addMouseButtonHandler(
      [this](int button, int action, int mods) mutable {
        if (button != GLFW_MOUSE_BUTTON_MIDDLE) {
          return;
        }

        if (action == GLFW_RELEASE) {
          inputState = InputState::None;
        } else if (window->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
          inputState = InputState::Pan;
          prevMousePos = window->getCurrentMousePosition();
        } else if (window->isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
          inputState = InputState::Zoom;
          prevMousePos = window->getCurrentMousePosition();
        } else {
          inputState = InputState::Rotate;
          prevMousePos = window->getCurrentMousePosition();
        }
      });

  mouseMoveHandler =
      window->addMouseMoveHandler([this](double xpos, double ypos) mutable {
        if (inputState == InputState::None) {
          return;
        }

        const auto &size = window->getFramebufferSize();

        glm::vec2 newPos{xpos, ypos};
        bool outOfBounds = false;

        if (xpos <= 0) {
          newPos.x = static_cast<float>(size.width);
          outOfBounds = true;
        } else if (xpos >= size.width) {
          newPos.x = 0;
          outOfBounds = true;
        }

        if (ypos <= 0) {
          newPos.y = static_cast<float>(size.height);
          outOfBounds = true;
        } else if (ypos >= size.height) {
          newPos.y = 0;
          outOfBounds = true;
        }

        if (outOfBounds) {
          prevMousePos = newPos;
          window->setMousePosition(newPos);
        }
      });
}

EditorCamera::~EditorCamera() {
  window->removeResizeHandler(resizeHandler);
  window->removeMouseButtonHandler(mouseButtonHandler);
  window->removeMouseMoveHandler(mouseMoveHandler);

  context.deleteComponent<liquid::CameraComponent>(cameraEntity);
}

void EditorCamera::setCenter(const glm::vec3 &center_) { center = center_; }

void EditorCamera::setEye(const glm::vec3 &eye_) { eye = eye_; }

void EditorCamera::update() {
  if (inputState == InputState::Pan) {
    pan();
  } else if (inputState == InputState::Rotate) {
    rotate();
  } else if (inputState == InputState::Zoom) {
    zoom();
  }

  camera->lookAt(eye, center, up);
}

void EditorCamera::reset() {
  eye = DEFAULT_EYE;
  center = DEFAULT_CENTER;
  up = DEFAULT_UP;
  fov = DEFAULT_FOV;
  near = DEFAULT_NEAR;
  far = DEFAULT_FAR;

  cameraEntity = context.createEntity();
  context.setComponent<liquid::CameraComponent>(cameraEntity, {camera});
}

void EditorCamera::pan() {
  constexpr float panSpeed = 0.03f;

  glm::vec2 mousePos = window->getCurrentMousePosition();
  glm::vec3 right = glm::normalize(glm::cross(glm::vec3(eye - center), up));

  glm::vec2 mousePosDiff =
      glm::vec4((mousePos - prevMousePos) * panSpeed, 0.0f, 0.0f);

  glm::vec3 change = up * mousePosDiff.y + right * mousePosDiff.x;
  eye += change;
  center += change;
  prevMousePos = mousePos;
}

void EditorCamera::rotate() {
  constexpr float TWO_PI = 2.0f * glm::pi<float>();

  glm::vec2 mousePos = window->getCurrentMousePosition();

  const auto &size = window->getFramebufferSize();

  glm ::vec2 screenToSphere{
      // horizontal = 2pi
      (TWO_PI / static_cast<float>(size.width)),
      // vertical = pi
      (glm::pi<float>() / static_cast<float>(size.height))};

  // Convert mouse position difference to angle
  // difference for arcball
  glm::vec2 angleDiff = (mousePos - prevMousePos) * screenToSphere;
  glm::vec3 direction = glm::vec3(eye - center);
  glm::vec3 right = glm::normalize(glm::cross(direction, up));
  up = glm::normalize(glm::cross(right, direction));

  glm::mat4 rotationX = glm::rotate(glm::mat4{1.0f}, -angleDiff.x, up);
  glm::mat4 rotationY = glm::rotate(glm::mat4{1.0f}, angleDiff.y, right);

  eye =
      glm::vec3(rotationY * (rotationX * glm::vec4(direction, 0.0f))) + center;

  prevMousePos = mousePos;
}

void EditorCamera::zoom() {
  constexpr float zoomSpeed = 0.03f;

  glm::vec2 mousePos = window->getCurrentMousePosition();
  float zoomFactor = (mousePos.y - prevMousePos.y) * zoomSpeed;

  glm::vec3 change = glm::vec3(eye - center) * zoomFactor;
  center += change;
  eye += change;
  prevMousePos = mousePos;
}

void EditorCamera::updatePerspectiveBasedOnFramebuffer() {
  const auto &fbSize = window->getFramebufferSize();
  updatePerspective(static_cast<float>(fbSize.width) /
                    static_cast<float>(fbSize.height));
}

void EditorCamera::updatePerspective(float aspectRatio) {
  camera->setPerspective(fov, aspectRatio, near, far);
}

} // namespace liquidator
