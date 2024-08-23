#pragma once

#include "quoll/animation/AnimationSystemView.h"
#include "quoll/audio/AudioSystemView.h"
#include "quoll/input/InputMapSystemView.h"
#include "quoll/lua-scripting/LuaScriptingSystemView.h"
#include "quoll/physx/PhysxSystemView.h"
#include "quoll/scene/Scene.h"
#include "quoll/scene/SceneUpdaterSystemView.h"
#include "quoll/scene/SkeletonUpdaterSystemView.h"
#include "quoll/ui/UICanvasUpdaterSystemView.h"

namespace quoll {

struct SystemView {
  Scene *scene = nullptr;
  LuaScriptingSystemView luaScripting;
  AudioSystemView audio;
  PhysxSystemView physx;
  SkeletonUpdaterSystemView skeletonUpdater;
  SceneUpdaterSystemView sceneUpdater;
  UICanvasUpdaterSystemView uiCanvasUpdater;
  InputMapSystemView inputMap;
  AnimationSystemView animation;
};

} // namespace quoll
