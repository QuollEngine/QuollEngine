#pragma once

namespace quoll {

/**
 * @brief Animation state loop mode
 *
 * - None = No looping
 * - Linear = Loops in one direction
 */
enum class AnimationLoopMode { None = 0, Linear = 1 };

/**
 * @brief Animation state transition
 */
struct AnimationStateTransition {
  /**
   * Event name
   */
  String eventName;

  /**
   * Index to animation state
   */
  usize target;
};

/**
 * @brief Animation state
 *
 * Single state in the finite state machine
 */
struct AnimationState {
  /**
   * Animation state name
   */
  String name;

  /**
   * Animation corresponding to the state
   */
  AnimationAssetHandle animation = AnimationAssetHandle::Null;

  /**
   * Animation speed
   *
   * Speed is used as a multiplier
   *
   * - 1.0 is the normal speed
   * - < 1.0 slows down the animation
   * - > 1.0 speeds up the animation
   */
  f32 speed = 1.0f;

  /**
   * Animation loop mode
   */
  AnimationLoopMode loopMode = AnimationLoopMode::None;

  /**
   * Transitions to other states
   *
   * First parameter of the pair is the event name
   * Second parameter of the pair is index to another state
   */
  std::vector<AnimationStateTransition> transitions;
};

/**
 * Animator asset
 *
 * Animator is a state machine that defines
 * animation states and transitions from
 * one state to another
 */
struct AnimatorAsset {
  /**
   * Initial state
   */
  usize initialState = 0;

  /**
   * Animation states
   */
  std::vector<AnimationState> states;
};

} // namespace quoll
