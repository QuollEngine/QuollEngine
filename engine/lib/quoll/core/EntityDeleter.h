#pragma once

namespace quoll {

struct SystemView;

class EntityDeleter {
public:
  void update(SystemView &view);
};

} // namespace quoll
