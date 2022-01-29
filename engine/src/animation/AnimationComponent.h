#pragma once

namespace liquid {

struct AnimationComponent {
  String animation;
  bool loop = false;
  float currentTime = 0.0f;
  bool playing = false;
};

} // namespace liquid
