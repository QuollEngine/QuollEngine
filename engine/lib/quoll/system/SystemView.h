#pragma once

#include "quoll/audio/AudioSystemView.h"
#include "quoll/lua-scripting/LuaScriptingSystemView.h"
#include "quoll/physx/PhysxSystemView.h"
#include "quoll/scene/Scene.h"

namespace quoll {

struct SystemView {
  Scene *scene = nullptr;
  LuaScriptingSystemView luaScripting;
  AudioSystemView audio;
  PhysxSystemView physx;
};

} // namespace quoll
