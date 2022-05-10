#pragma once

namespace liquid {

/**
 * @brief Vertex information
 *
 * Stores position, normal, and color,
 * and texture coordinates for a vertex
 */
struct Vertex {
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
};

} // namespace liquid
