#include "liquid/core/Base.h"
#include "EditorCamera.h"

#include <GLFW/glfw3.h>

using liquid::Camera;
using liquid::GLFWWindow;
using liquid::VulkanRenderer;

namespace liquidator {

EditorCamera::EditorCamera(liquid::EntityContext &context_,
                           liquid::VulkanRenderer *renderer,
                           liquid::GLFWWindow *window_)
    : context(context_), window(window_),
      camera(new Camera(&renderer->getRegistry())) {

  mouseButtonHandler = window->addMouseButtonHandler(
      [this](int button, int action, int mods) mutable {
        if (button != GLFW_MOUSE_BUTTON_MIDDLE) {
          return;
        }

        if (action == GLFW_RELEASE) {
          inputState = InputState::None;
          return;
        }

        const auto &cursorPos = window->getCurrentMousePosition();

        // Do not trigger action if cursor is outside
        // Imgui window viewport
        if (cursorPos.x < x || cursorPos.x > x + width || cursorPos.y < y ||
            cursorPos.y > y + height) {
          return;
        }

        if (window->isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
          inputState = InputState::Pan;
          prevMousePos = cursorPos;
        } else if (window->isKeyPressed(GLFW_KEY_LEFT_CONTROL)) {
          inputState = InputState::Zoom;
          prevMousePos = cursorPos;
        } else {
          inputState = InputState::Rotate;
          prevMousePos = cursorPos;
        }
      });

  // Out of bounds handler
  mouseMoveHandler =
      window->addMouseMoveHandler([this](double xpos, double ypos) mutable {
        if (inputState == InputState::None) {
          return;
        }

        constexpr float MIN_OOB_THRESHOLD = 2.0f;
        const auto &size = window->getWindowSize();

        float minX = 0;
        float maxX = static_cast<float>(size.x);
        float minY = 0;
        float maxY = static_cast<float>(size.y);

        glm::vec2 newPos{xpos, ypos};
        bool outOfBounds = false;

        if (xpos <= minX) {
          newPos.x = maxX - MIN_OOB_THRESHOLD;
          outOfBounds = true;
        } else if (xpos >= maxX) {
          newPos.x = minX + MIN_OOB_THRESHOLD;
          outOfBounds = true;
        }

        if (ypos <= minY) {
          newPos.y = maxY - MIN_OOB_THRESHOLD;
          outOfBounds = true;
        } else if (ypos >= maxY) {
          newPos.y = minY + MIN_OOB_THRESHOLD;
          outOfBounds = true;
        }

        if (outOfBounds) {
          prevMousePos = newPos;
          window->setMousePosition(newPos);
        }
      });

  scrollWheelHandler =
      window->addScrollWheelHandler([this](double xoffset, double yoffset) {
        if (inputState != InputState::None) {
          return;
        }
        glm::vec3 change =
            glm::vec3(eye - center) * static_cast<float>(yoffset) * ZOOM_SPEED;
        center += change;
        eye += change;
      });
}

EditorCamera::~EditorCamera() {
  window->removeMouseButtonHandler(mouseButtonHandler);
  window->removeMouseMoveHandler(mouseMoveHandler);
  window->removeScrollWheelHandler(scrollWheelHandler);

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
  camera->setPerspective(
      fov, static_cast<float>(width) / static_cast<float>(height), near, far);
}

void EditorCamera::reset() {
  eye = DEFAULT_EYE;
  center = DEFAULT_CENTER;
  up = DEFAULT_UP;
  fov = DEFAULT_FOV;
  near = DEFAULT_NEAR;
  far = DEFAULT_FAR;

  if (!context.hasEntity(cameraEntity)) {
    cameraEntity = context.createEntity();
    context.setComponent<liquid::CameraComponent>(cameraEntity, {camera});
  }
}

void EditorCamera::pan() {
  constexpr float PAN_SPEED = 0.03f;

  glm::vec2 mousePos = window->getCurrentMousePosition();
  glm::vec3 right = glm::normalize(glm::cross(glm::vec3(eye - center), up));

  glm::vec2 mousePosDiff =
      glm::vec4((mousePos - prevMousePos) * PAN_SPEED, 0.0f, 0.0f);

  glm::vec3 change = up * mousePosDiff.y + right * mousePosDiff.x;
  eye += change;
  center += change;
  prevMousePos = mousePos;
}

void EditorCamera::rotate() {
  constexpr float TWO_PI = 2.0f * glm::pi<float>();

  glm::vec2 mousePos = window->getCurrentMousePosition();

  const auto &size = window->getFramebufferSize();

  glm ::vec2 screenToSphere{// horizontal = 2pi
                            (TWO_PI / static_cast<float>(size.x)),
                            // vertical = pi
                            (glm::pi<float>() / static_cast<float>(size.y))};

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
  glm::vec2 mousePos = window->getCurrentMousePosition();
  float zoomFactor = (mousePos.y - prevMousePos.y) * ZOOM_SPEED;

  glm::vec3 change = glm::vec3(eye - center) * zoomFactor;
  center += change;
  eye += change;
  prevMousePos = mousePos;
}

void EditorCamera::setViewport(float x_, float y_, float width_,
                               float height_) {
  x = x_;
  y = y_;
  width = width_;
  height = height_;
}

void EditorCamera::updatePerspective(float aspectRatio) {
  camera->setPerspective(fov, aspectRatio, near, far);
}

} // namespace liquidator
