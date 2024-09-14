#pragma once

namespace quoll {

/**
 * Deletes copy constructor,
 * move constructor, copy assignment
 * operator, and move assignment operator.
 *
 * When needing to define a non copyable
 * and non movable object, inherit this class
 * without a specifier:
 *
 *  class MyClass : NoCopyMove { ... }
 */
struct NoCopyMove {
  NoCopyMove() = default;
  NoCopyMove(NoCopyMove &&) = delete;
  NoCopyMove &operator=(NoCopyMove &&) = delete;
  NoCopyMove(const NoCopyMove &) = delete;
  NoCopyMove &operator=(const NoCopyMove &) = delete;
};

} // namespace quoll
