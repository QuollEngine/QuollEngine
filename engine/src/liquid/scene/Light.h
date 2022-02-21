#pragma once

namespace liquid {

class Light {
public:
  enum LightType { DIRECTIONAL = 0 };

public:
  /**
   * @brief Create light
   *
   * @param type Light type
   * @param direction Light direction
   * @param color Light color
   * @param intensity Light intensity
   */
  Light(LightType type, glm::vec3 direction, glm::vec4 color, float intensity);

  /**
   * @brief Get projection view matrix of light
   *
   * @return Projection view matrix
   */
  const glm::mat4 getProjectionViewMatrix() const;

  /**
   * @brief Set light position
   *
   * @param position Light position
   */
  void setPosition(glm::vec3 position);

  /**
   * @brief Set light direction
   *
   * @param direction Light direction
   */
  void setDirection(glm::vec3 direction);

  /**
   * @brief Set light color
   *
   * @param color Light color
   */
  void setColor(glm::vec4 color);

  /**
   * @brief Set light intensity
   *
   * @param color Light intensity
   */
  void setIntensity(float intensity);

  /**
   * @brief Get light color
   *
   * @return Light color
   */
  inline const glm::vec4 &getColor() { return color; }

  /**
   * @brief Get light intensity
   *
   * Scale of this value can be different
   * based on the type of light
   *
   * @return Light intensity
   */
  inline float getIntensity() { return intensity; }

  /**
   * @brief Get light direction
   *
   * This value is used by directional light
   *
   * @return Light direction
   */
  inline const glm::vec3 &getDirection() { return direction; }

  /**
   * @brief Get light type
   *
   * @return Light type
   */
  inline LightType getType() { return type; }

  /**
   * @brief Get light type name
   *
   * @return Light type name
   */
  const String getTypeName() const;

  /**
   * @brief Get light position
   *
   * @return Light position
   */
  inline const glm::vec3 &getPosition() const { return position; }

private:
  glm::vec3 position{0.0f};
  glm::vec4 color;
  glm::vec3 direction;
  float intensity;
  LightType type;
};

} // namespace liquid
