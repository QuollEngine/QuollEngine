#include "quoll/core/Base.h"
#include "quoll/entity/EntityLuaTable.h"
#include "CollisionHitLuaTable.h"

namespace quoll {

void CollisionHitLuaTable::create(sol::state_view state,
                                  ScriptGlobals scriptGlobals) {
  auto usertype = state.new_usertype<CollisionHit>(
      "CollisionHit", sol::no_constructor, "normal", &CollisionHit::normal,
      "distance", &CollisionHit::distance);

  usertype["entity"] = sol::property([scriptGlobals](CollisionHit &hit) {
    auto entity = hit.entity;
    return EntityLuaTable(hit.entity, scriptGlobals);
  });
}

} // namespace quoll
