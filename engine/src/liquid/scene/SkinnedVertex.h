#pragma once

namespace liquid {

/**
 * @brief Skinned vertex
 *
 * Stores all vertex attributes; plus,
 * joints and weights per vertex
 */
struct SkinnedVertex {
  /// @{
  /**
   * Position
   */
  float x, y, z;
  /// @}

  /// @{
  /**
   * Normal
   */
  float nx, ny, nz;
  /// @}

  /// @{
  /**
   * Tangent
   */
  float tx, ty, tz, tw;
  /// @}

  /// @{
  /**
   * Color
   */
  float r, g, b;
  /// @}

  /// @{
  /**
   * Texture coordinates index #0
   */
  float u0, v0;
  /// @}

  /// @{
  /**
   * Texture coordinates index #1
   */
  float u1, v1;
  /// @}

  /// @{
  /**
   * Joints
   */
  uint32_t j0, j1, j2, j3;
  /// @}

  /// @{
  /**
   * Weights
   */
  float w0, w1, w2, w3;
  /// @}
};

} // namespace liquid
