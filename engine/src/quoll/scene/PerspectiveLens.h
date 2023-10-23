#pragma once

namespace quoll {

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
  static constexpr f32 DefaultFocalLength = 23.0f;

  /**
   * Default aperture
   */
  static constexpr f32 DefaultAperture = 16.0f;

  /**
   * Default shutter speed
   */
  static constexpr f32 DefaultShutterSpeed = 1.0f / 250.0f;

  /**
   * Default sensitivity
   */
  static constexpr u32 DefaultSensitivity = 200;

  /**
   * Default near plane
   */
  static constexpr f32 DefaultNearPlane = 0.1f;

  /**
   * Default far plane
   */
  static constexpr f32 DefaultFarPlane = 1000.0f;

  /**
   * Near plane
   *
   * Measured in meters
   */
  f32 near = DefaultNearPlane;

  /**
   * Far plane
   *
   * Measured in meters
   */
  f32 far = DefaultFarPlane;

  /**
   * Aspect ratio
   */
  f32 aspectRatio = 1.0f;

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
  f32 focalLength = DefaultFocalLength;

  /**
   * Lens aperture
   *
   * Measured in f-stops
   *
   * Example: f3.5, f1.6, f22
   */
  f32 aperture = DefaultAperture;

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
  f32 shutterSpeed = DefaultShutterSpeed;

  /**
   * Camera sensitivity to light
   *
   * Also referred to as camera
   * ISO and is measured in ISO values.
   *
   * Example: ISO 100, ISO 400, ISO 1600
   */
  u32 sensitivity = DefaultSensitivity;
};

} // namespace quoll
