#pragma once

namespace qui {

class View;

struct HitTestResult {
  std::vector<View *> path;
};

} // namespace qui
