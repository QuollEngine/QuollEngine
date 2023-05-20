#include "liquid/core/Base.h"

#include "liquid-tests/Testing.h"
#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

class AnimatorLuaScriptingInterfaceTest : public LuaScriptingInterfaceTestBase {
public:
};

TEST_F(AnimatorLuaScriptingInterfaceTest,
       TriggerDoesNothingIfEventNameIsInvalid) {
  auto entity = entityDatabase.create();
  call(entity, "animator_trigger_invalid");

  EXPECT_FALSE(entityDatabase.has<liquid::AnimatorEvent>(entity));
}

TEST_F(AnimatorLuaScriptingInterfaceTest, TriggerAddsAnimatorEventComponent) {
  auto entity = entityDatabase.create();
  call(entity, "animator_trigger");

  EXPECT_TRUE(entityDatabase.has<liquid::AnimatorEvent>(entity));
  EXPECT_EQ(entityDatabase.get<liquid::AnimatorEvent>(entity).eventName,
            "Move");
}

TEST_F(AnimatorLuaScriptingInterfaceTest,
       DeleteDoesNothingIfProvidedArgumentIsInvalid) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Animator>(entity, {});

  call(entity, "animator_delete_invalid");
  EXPECT_TRUE(entityDatabase.has<liquid::Animator>(entity));
}

TEST_F(AnimatorLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "animator_delete");
  EXPECT_FALSE(entityDatabase.has<liquid::Animator>(entity));
}

TEST_F(AnimatorLuaScriptingInterfaceTest,
       DeleteRemovesAnimatorSourceComponentFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<liquid::Animator>(entity, {});

  call(entity, "animator_delete");
  EXPECT_FALSE(entityDatabase.has<liquid::Animator>(entity));
}
