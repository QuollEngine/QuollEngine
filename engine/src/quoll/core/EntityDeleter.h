#pragma once

namespace quoll {

struct Scene;

class EntityDeleter {
public:
  void update(Scene &scene);
};

} // namespace quoll
