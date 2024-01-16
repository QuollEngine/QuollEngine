#pragma once

#include "quoll/animation/AnimatorLuaTable.h"
#include "quoll/audio/AudioLuaTable.h"
#include "quoll/entity/Entity.h"
#include "quoll/input/InputMapLuaTable.h"
#include "quoll/lua-scripting/LuaUserTypeBase.h"
#include "quoll/lua-scripting/ScriptLuaTable.h"
#include "quoll/physics/CollidableLuaTable.h"
#include "quoll/physics/RigidBodyLuaTable.h"
#include "quoll/scene/PerspectiveLensLuaTable.h"
#include "quoll/scene/TransformLuaTable.h"
#include "quoll/text/TextLuaTable.h"
#include "quoll/ui/UICanvasLuaTable.h"

namespace quoll {

class EntityLuaTable {
public:
  EntityLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  static void create(sol::state_view state);

  inline Entity getEntity() { return mEntity; }

  inline ScriptGlobals &getScriptGlobals() { return mScriptGlobals; }

  inline bool operator==(const EntityLuaTable &rhs) const {
    return mEntity == rhs.mEntity;
  }

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;

  TransformLuaTable mTransform;
  PerspectiveLensLuaTable mPerspectiveLens;
  RigidBodyLuaTable mRigidBody;
  CollidableLuaTable mCollidable;
  AudioLuaTable mAudio;
  TextLuaTable mText;
  AnimatorLuaTable mAnimator;
  InputMapLuaTable mInputMap;
  UICanvasLuaTable mUICanvas;
  ScriptLuaTable mScript;
};

} // namespace quoll
