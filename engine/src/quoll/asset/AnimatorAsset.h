#pragma once

#include "Asset.h"

namespace quoll {

enum class AnimationLoopMode { None = 0, Linear = 1 };

struct AnimationStateTransition {
  String eventName;

  usize target;
};

struct AnimationState {
  String name;

  AnimationAssetHandle animation = AnimationAssetHandle::Null;

  f32 speed = 1.0f;

  AnimationLoopMode loopMode = AnimationLoopMode::None;

  std::vector<AnimationStateTransition> transitions;
};

struct AnimatorAsset {
  usize initialState = 0;

  std::vector<AnimationState> states;
};

} // namespace quoll
