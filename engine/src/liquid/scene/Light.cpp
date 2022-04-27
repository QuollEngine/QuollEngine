#include "liquid/core/Base.h"
#include "Light.h"

namespace liquid {

Light::Light(LightType type, glm::vec4 color, float intensity)
    : mType(type), mColor(color), mIntensity(intensity) {}

const glm::mat4 Light::getProjectionViewMatrix() const {
  const float DIR_LIGHT_SIZE = 20.0f;
  const float DIR_LIGHT_NEAR = 0.001f;
  const float DIR_LIGHT_FAR = 100.0f;
  const float DIR_LIGHT_Z = 0.01f;
  glm::vec3 mPosition{-mDirection - mDirection * DIR_LIGHT_SIZE};
  mPosition.z = DIR_LIGHT_Z;
  glm::mat4 lightProjectionMatrix =
      glm::ortho(-DIR_LIGHT_SIZE, DIR_LIGHT_SIZE, -DIR_LIGHT_SIZE,
                 DIR_LIGHT_SIZE, DIR_LIGHT_NEAR, DIR_LIGHT_FAR);
  glm::mat4 lightViewMatrix = glm::lookAt(
      mPosition, mPosition + mDirection - glm::vec3(0.0f, 0.0f, DIR_LIGHT_Z),
      {0.0f, 1.0f, 0.0f});
  return lightProjectionMatrix * lightViewMatrix;
}

void Light::setPosition(glm::vec3 position) { mPosition = position; }

void Light::setDirection(glm::vec3 direction) { mDirection = direction; }

void Light::setColor(glm::vec4 color) { mColor = color; }

void Light::setIntensity(float intensity) { mIntensity = intensity; }

const String Light::getTypeName() const {
  switch (mType) {
  case LightType::Directional:
    return "Directional";
  default:
    return "Unknown";
  }
}

} // namespace liquid
