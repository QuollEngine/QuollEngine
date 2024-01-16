#pragma once

namespace quoll {

/**
 * Animator event component is used
 * as an event mechanism to trigger
 * various animator states in entity's
 * animator state machine.
 */
struct AnimatorEvent {
  String eventName;
};

} // namespace quoll
