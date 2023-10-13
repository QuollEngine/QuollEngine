#include "quoll/core/Base.h"
#include "UICanvasLuaTable.h"
#include "UICanvas.h"
#include "UICanvasRenderRequest.h"

namespace quoll {

UICanvasLuaTable::UICanvasLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mScriptGlobals(scriptGlobals) {}

void UICanvasLuaTable::render(UIView view) {
  if (mScriptGlobals.entityDatabase.has<UICanvas>(mEntity)) {
    mScriptGlobals.entityDatabase.set<UICanvasRenderRequest>(mEntity, {view});
  }
}

void UICanvasLuaTable::create(sol::usertype<UICanvasLuaTable> usertype) {
  usertype["render"] = &UICanvasLuaTable::render;
}

} // namespace quoll
