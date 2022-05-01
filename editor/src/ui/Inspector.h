#pragma once

namespace liquidator {

class Inspector {
public:
  void render(std::function<void()> &&fn);

private:
  bool mOpen = false;
};

} // namespace liquidator
