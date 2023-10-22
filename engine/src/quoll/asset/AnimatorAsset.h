#pragma once

namespace quoll {

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
