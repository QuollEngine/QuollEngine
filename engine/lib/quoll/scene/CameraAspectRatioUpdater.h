#pragma once

namespace quoll {

struct SystemView;

/**
 * Updates camera with auto aspect ratio
 * based on Window aspect ratio
 */
class CameraAspectRatioUpdater {
public:
  void update(SystemView &view) const;

  void setViewportSize(glm::uvec2 size);

private:
  glm::uvec2 mSize{};
};

} // namespace quoll
