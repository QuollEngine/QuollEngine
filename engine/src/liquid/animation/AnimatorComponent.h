#pragma once

namespace liquid {

struct AnimatorComponent {
  uint32_t currentAnimation = 0;
  bool loop = false;
  float currentTime = 0.0f;
  bool playing = false;

  std::vector<String> animations;
};

} // namespace liquid
