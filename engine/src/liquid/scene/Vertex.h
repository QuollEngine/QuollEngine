#pragma once

namespace liquid {

/**
 * @brief Vertex information
 *
 * Stores position, normal, and color,
 * and texture coordinates for a vertex
 */
struct Vertex {
  float x, y, z;
  float nx, ny, nz;
  float tx, ty, tz, tw;
  float r, g, b;
  float u0, v0;
  float u1, v1;
};

} // namespace liquid
