#include "Light.h"

namespace liquid {

Light::Light(Light::LightType type_, glm::vec3 direction_, glm::vec4 color_,
             float intensity_)
    : type(type_), color(color_), intensity(intensity_), direction(direction_) {
}

const glm::mat4 Light::getProjectionViewMatrix() const {
  const float DIR_LIGHT_SIZE = 20.0f;
  const float DIR_LIGHT_NEAR = 0.001f;
  const float DIR_LIGHT_FAR = 100.0f;
  const float DIR_LIGHT_Z = 0.01f;
  glm::vec3 position{direction + direction * DIR_LIGHT_SIZE};
  position.z = DIR_LIGHT_Z;
  glm::mat4 lightProjectionMatrix =
      glm::ortho(-DIR_LIGHT_SIZE, DIR_LIGHT_SIZE, -DIR_LIGHT_SIZE,
                 DIR_LIGHT_SIZE, DIR_LIGHT_NEAR, DIR_LIGHT_FAR);
  glm::mat4 lightViewMatrix = glm::lookAt(
      position, position - direction - glm::vec3(0.0f, 0.0f, DIR_LIGHT_Z),
      {0.0f, 1.0f, 0.0f});
  return lightProjectionMatrix * lightViewMatrix;
}

void Light::setPosition(glm::vec3 position_) { position = position_; }

void Light::setDirection(glm::vec3 direction_) { direction = direction_; }

void Light::setColor(glm::vec4 color_) { color = color_; }

void Light::setIntensity(float intensity_) { intensity = intensity_; }

} // namespace liquid
