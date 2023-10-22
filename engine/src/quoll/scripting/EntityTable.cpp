#include "quoll/core/Base.h"
#include "EntityTable.h"

namespace quoll {

template <class TFieldName, class TFieldType>
void setToStruct(sol::state_view state, sol::usertype<EntityTable> entityTable,
                 TFieldType TFieldName::*field) {
  auto usertype = state.new_usertype<TFieldType>(
      "Entity_" + TFieldType::getName(), sol::no_constructor);
  TFieldType::create(usertype);

  entityTable[TFieldType::getName()] = field;
}

EntityTable::EntityTable(Entity entity, ScriptGlobals &scriptGlobals)
    : mEntity(entity), mName(entity, scriptGlobals),
      mTransform(entity, scriptGlobals),
      mPerspectiveLens(entity, scriptGlobals),
      mRigidBody(entity, scriptGlobals), mCollidable(entity, scriptGlobals),
      mAudio(entity, scriptGlobals), mText(entity, scriptGlobals),
      mAnimator(entity, scriptGlobals), mInputMap(entity, scriptGlobals),
      mUICanvas(entity, scriptGlobals) {}

void EntityTable::create(sol::state_view state) {
  auto entityTable =
      state.new_usertype<EntityTable>("Entity", sol::no_constructor);

  setToStruct(state, entityTable, &EntityTable::mName);
  setToStruct(state, entityTable, &EntityTable::mTransform);
  setToStruct(state, entityTable, &EntityTable::mPerspectiveLens);
  setToStruct(state, entityTable, &EntityTable::mRigidBody);
  setToStruct(state, entityTable, &EntityTable::mCollidable);
  setToStruct(state, entityTable, &EntityTable::mAudio);
  setToStruct(state, entityTable, &EntityTable::mText);
  setToStruct(state, entityTable, &EntityTable::mAnimator);
  setToStruct(state, entityTable, &EntityTable::mInputMap);
  setToStruct(state, entityTable, &EntityTable::mUICanvas);
}

} // namespace quoll
