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
   * @param color Light color
   * @param intensity Light intensity
   */
  Light(LightType type, glm::vec4 color, float intensity);

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
  inline const glm::vec4 &getColor() const { return mColor; }

  /**
   * @brief Get light intensity
   *
   * Scale of this value can be different
   * based on the type of light
   *
   * @return Light intensity
   */
  inline float getIntensity() const { return mIntensity; }

  /**
   * @brief Get light direction
   *
   * This value is used by directional light
   *
   * @return Light direction
   */
  inline const glm::vec3 &getDirection() const { return mDirection; }

  /**
   * @brief Get light type
   *
   * @return Light type
   */
  inline LightType getType() const { return mType; }

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
  inline const glm::vec3 &getPosition() const { return mPosition; }

private:
  glm::vec3 mPosition{0.0f};
  glm::vec4 mColor;
  glm::vec3 mDirection{0.0f};
  float mIntensity;
  LightType mType;
};

} // namespace liquid
