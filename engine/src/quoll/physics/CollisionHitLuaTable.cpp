#include "quoll/core/Base.h"
#include "CollisionHitLuaTable.h"

namespace quoll {

void CollisionHitLuaTable::create(sol::state_view state) {
  auto usertype = state.new_usertype<CollisionHit>(
      "CollisionHit", sol::no_constructor, "normal", &CollisionHit::normal);
}

} // namespace quoll
