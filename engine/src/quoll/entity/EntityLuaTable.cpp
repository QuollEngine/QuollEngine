#include "quoll/core/Base.h"
#include "EntityLuaTable.h"

namespace quoll {

template <class TFieldName, class TFieldType>
void setToStruct(sol::state_view state,
                 sol::usertype<EntityLuaTable> entityTable,
                 TFieldType TFieldName::*field) {
  auto usertype = state.new_usertype<TFieldType>(
      "Entity_" + TFieldType::getName(), sol::no_constructor);
  TFieldType::create(usertype, state);

  entityTable[TFieldType::getName()] = field;
}

EntityLuaTable::EntityLuaTable(Entity entity, ScriptGlobals scriptGlobals)
    : mEntity(entity), mName(entity, scriptGlobals),
      mTransform(entity, scriptGlobals),
      mPerspectiveLens(entity, scriptGlobals),
      mRigidBody(entity, scriptGlobals), mCollidable(entity, scriptGlobals),
      mAudio(entity, scriptGlobals), mText(entity, scriptGlobals),
      mAnimator(entity, scriptGlobals), mInputMap(entity, scriptGlobals),
      mUICanvas(entity, scriptGlobals), mScript(entity, scriptGlobals) {}

void EntityLuaTable::create(sol::state_view state) {
  auto entityTable =
      state.new_usertype<EntityLuaTable>("Entity", sol::no_constructor);

  setToStruct(state, entityTable, &EntityLuaTable::mName);
  setToStruct(state, entityTable, &EntityLuaTable::mTransform);
  setToStruct(state, entityTable, &EntityLuaTable::mPerspectiveLens);
  setToStruct(state, entityTable, &EntityLuaTable::mRigidBody);
  setToStruct(state, entityTable, &EntityLuaTable::mCollidable);
  setToStruct(state, entityTable, &EntityLuaTable::mAudio);
  setToStruct(state, entityTable, &EntityLuaTable::mText);
  setToStruct(state, entityTable, &EntityLuaTable::mAnimator);
  setToStruct(state, entityTable, &EntityLuaTable::mInputMap);
  setToStruct(state, entityTable, &EntityLuaTable::mUICanvas);
  setToStruct(state, entityTable, &EntityLuaTable::mScript);
}

} // namespace quoll
