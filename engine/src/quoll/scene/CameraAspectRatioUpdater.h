#pragma once

namespace quoll {

class EntityDatabase;

/**
 * Updates camera with auto aspect ratio
 * based on Window aspect ratio
 */
class CameraAspectRatioUpdater {
public:
  void update(EntityDatabase &entityDatabase);

  void setViewportSize(glm::uvec2 size);

private:
  glm::uvec2 mSize{};
};

} // namespace quoll
