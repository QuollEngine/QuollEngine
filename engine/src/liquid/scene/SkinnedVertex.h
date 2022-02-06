#pragma once

namespace liquid {

/**
 * @brief Skinned vertex
 *
 * Stores all vertex attributes; plus,
 * joints and weights per vertex
 */
struct SkinnedVertex {
  float x, y, z;
  float nx, ny, nz;
  float tx, ty, tz, tw;
  float r, g, b;
  float u0, v0;
  float u1, v1;
  uint32_t j0, j1, j2, j3;
  float w0, w1, w2, w3;
};

} // namespace liquid
