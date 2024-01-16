#pragma once

#include "quoll/lua-scripting/LuaUserTypeBase.h"

namespace quoll {

class AudioLuaTable {
public:
  AudioLuaTable(Entity entity, ScriptGlobals scriptGlobals);

  void play();

  bool isPlaying();

  void deleteThis();

  static const String getName() { return "audio"; }

  static void create(sol::usertype<AudioLuaTable> usertype,
                     sol::state_view state);

private:
  Entity mEntity;
  ScriptGlobals mScriptGlobals;
};

} // namespace quoll
