#pragma once

#include "liquid/scene/Camera.h"
#include "liquid/renderer/vulkan/VulkanRenderer.h"
#include "liquid/entity/EntityContext.h"
#include "liquid/window/glfw/GLFWWindow.h"

class EditorCamera {
public:
  EditorCamera(liquid::EntityContext &context, liquid::VulkanRenderer *renderer,
               liquid::GLFWWindow *window);

  inline void setFOV(float fov_) { fov = fov_; }

  inline void setNear(float near_) { near = near_; }

  inline void setFar(float far_) { far = far_; }

  inline float getFOV() { return fov; }

  inline float getNear() { return near; }

  inline float getFar() { return far; }

  inline liquid::Entity getCamera() { return cameraEntity; }

  inline void setPosition(glm::vec3 position_) { position = position_; }

  void move(float speed);

  void strafe(float speed);

  void update();

  void yaw(float speed);

  void pitch(float speed);

  void initEntity();

private:
  void updatePerspective();

  void updatePerspective(uint32_t width, uint32_t height);

private:
  float fov = 70.0f;
  float near = 0.001f;
  float far = 1000.0f;

  float moveSpeed = 0.0f;
  float strafeSpeed = 0.0f;

  float yawSpeed = 0.0f;
  float pitchSpeed = 0.0f;

  // yaw, pitch
  glm::vec2 rotation{0.0f, 0.0f};
  glm::vec3 position{5.0f, 1.0f, -10.0f};

  liquid::SharedPtr<liquid::Camera> camera;
  liquid::EntityContext &entityContext;
  liquid::Entity cameraEntity;
  uint32_t resizeHandler;
  liquid::GLFWWindow *window;
};
