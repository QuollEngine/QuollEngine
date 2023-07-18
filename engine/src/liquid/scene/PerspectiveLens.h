#pragma once

namespace liquid {

/**
 * @brief Perspective lens component
 */
struct PerspectiveLens {
  /**
   * Default sensor size
   *
   * Based on APS-C
   */
  static constexpr glm::vec2 DefaultSensorSize{23.6f, 15.6f};

  /**
   * Default focal length
   */
  static constexpr float DefaultFocalLength = 23.0f;

  /**
   * Default aperture
   */
  static constexpr float DefaultAperture = 22.0f;

  /**
   * Default shutter speed
   */
  static constexpr float DefaultShutterSpeed = 1.0f / 125.0f;

  /**
   * Default sensitivity
   */
  static constexpr uint32_t DefaultSensitivity = 100;

  /**
   * Near plane
   *
   * Measured in meters
   */
  float near = 0.1f;

  /**
   * Far plane
   *
   * Measured in meters
   */
  float far = 1000.0f;

  /**
   * Aspect ratio
   */
  float aspectRatio = 1.0f;

  /**
   * Sensor size
   *
   * Measured in millimeters
   */
  glm::vec2 sensorSize = DefaultSensorSize;

  /**
   * Lens focal length
   *
   * Measured in millimeters
   */
  float focalLength = DefaultFocalLength;

  /**
   * Lens aperture
   *
   * Measured in f-stops
   *
   * Example: f3.5, f1.6, f22
   */
  float aperture = DefaultAperture;

  /**
   * Camera shutter speed
   *
   * A more accurate description
   * of this term is shutter time
   * since it is measured in seconds;
   * however, the term shutter speed
   * is widely used in photography
   *
   * Example: 1/125s, 1/1000s, 1s
   */
  float shutterSpeed = DefaultShutterSpeed;

  /**
   * Camera sensitivity to light
   *
   * Also referred to as camera
   * ISO and is measured in ISO values.
   *
   * Example: ISO 100, ISO 400, ISO 1600
   */
  uint32_t sensitivity = DefaultSensitivity;
};

} // namespace liquid
