#pragma once

#include "quoll/entity/Entity.h"

#include "quoll/core/NameScriptingInterface.h"
#include "quoll/physics/RigidBodyScriptingInterface.h"
#include "quoll/physics/CollidableScriptingInterface.h"
#include "quoll/audio/AudioScriptingInterface.h"
#include "quoll/scene/TransformScriptingInterface.h"
#include "quoll/scene/PerspectiveLensScriptingInterface.h"
#include "quoll/text/TextScriptingInterface.h"
#include "quoll/animation/AnimatorScriptingInterface.h"
#include "quoll/input/InputMapScriptingInterface.h"

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

  NameScriptingInterface::LuaInterface mName;
  TransformScriptingInterface::LuaInterface mTransform;
  PerspectiveLensScriptingInterface::LuaInterface mPerspectiveLens;
  RigidBodyScriptingInterface::LuaInterface mRigidBody;
  CollidableScriptingInterface::LuaInterface mCollidable;
  AudioScriptingInterface::LuaInterface mAudio;
  TextScriptingInterface::LuaInterface mText;
  AnimatorScriptingInterface::LuaInterface mAnimator;
  InputMapScriptingInterface::LuaInterface mInputMap;
};

} // namespace quoll
