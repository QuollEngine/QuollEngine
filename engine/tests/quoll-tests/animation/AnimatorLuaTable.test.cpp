#include "quoll/core/Base.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll/animation/Animator.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class AnimatorLuaTableTest : public LuaScriptingInterfaceTestBase {
public:
};

TEST_F(AnimatorLuaTableTest, TriggerAddsAnimatorEventComponent) {
  auto entity = entityDatabase.create();
  call(entity, "animator_trigger");

  EXPECT_TRUE(entityDatabase.has<quoll::AnimatorEvent>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::AnimatorEvent>(entity).eventName, "Move");
}

TEST_F(AnimatorLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "animator_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Animator>(entity));
}

TEST_F(AnimatorLuaTableTest, DeleteRemovesAnimatorSourceComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Animator>(entity, {});

  call(entity, "animator_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Animator>(entity));
}
