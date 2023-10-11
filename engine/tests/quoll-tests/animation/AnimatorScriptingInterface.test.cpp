#include "quoll/core/Base.h"

#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class AnimatorLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {
public:
};

TEST_F(AnimatorLuaScriptingInterfaceTest, TriggerAddsAnimatorEventComponent) {
  auto entity = entityDatabase.create();
  call(entity, "animator_trigger");

  EXPECT_TRUE(entityDatabase.has<quoll::AnimatorEvent>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::AnimatorEvent>(entity).eventName, "Move");
}

TEST_F(AnimatorLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "animator_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Animator>(entity));
}

TEST_F(AnimatorLuaScriptingInterfaceTest,
       DeleteRemovesAnimatorSourceComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::Animator>(entity, {});

  call(entity, "animator_delete");
  EXPECT_FALSE(entityDatabase.has<quoll::Animator>(entity));
}
