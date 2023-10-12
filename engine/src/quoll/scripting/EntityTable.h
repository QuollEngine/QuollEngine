#pragma once

#include "quoll/entity/Entity.h"

#include "quoll/core/NameLuaTable.h"
#include "quoll/physics/RigidBodyLuaTable.h"
#include "quoll/physics/CollidableLuaTable.h"
#include "quoll/audio/AudioLuaTable.h"
#include "quoll/scene/TransformLuaTable.h"
#include "quoll/scene/PerspectiveLensLuaTable.h"
#include "quoll/text/TextLuaTable.h"
#include "quoll/animation/AnimatorLuaTable.h"
#include "quoll/input/InputMapLuaTable.h"

#include "ScriptGlobals.h"
#include "LuaHeaders.h"

namespace quoll {

/**
 * @brief Entity table
 */
class EntityTable {
public:
  /**
   * @brief Create entity table
   *
   * @param entity Entity
   * @param scriptGlobals Script globals
   */
  EntityTable(Entity entity, ScriptGlobals &scriptGlobals);

  /**
   * @brief Create interface
   *
   * @param state Sol state
   */
  static void create(sol::state_view state);

  /**
   * @brief Get entity
   *
   * @return Entity
   */
  inline Entity getEntity() { return mEntity; }

private:
  Entity mEntity;

  NameLuaTable mName;
  TransformLuaTable mTransform;
  PerspectiveLensLuaTable mPerspectiveLens;
  RigidBodyLuaTable mRigidBody;
  CollidableLuaTable mCollidable;
  AudioLuaTable mAudio;
  TextLuaTable mText;
  AnimatorLuaTable mAnimator;
  InputMapLuaTable mInputMap;
};

} // namespace quoll
