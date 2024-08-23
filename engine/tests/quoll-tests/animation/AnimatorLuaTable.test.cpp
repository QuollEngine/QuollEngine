#include "quoll/core/Base.h"
#include "quoll/animation/Animator.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class AnimatorLuaTableTest : public LuaScriptingInterfaceTestBase {
public:
};

TEST_F(AnimatorLuaTableTest, TriggerAddsAnimatorEventComponent) {
  auto entity = entityDatabase.entity();
  call(entity, "animatorTrigger");

  EXPECT_TRUE(entity.has<quoll::AnimatorEvent>());
  EXPECT_EQ(entity.get_ref<quoll::AnimatorEvent>()->eventName, "Move");
}

TEST_F(AnimatorLuaTableTest, PropertiesReturnAnimatorDataIfAnimatorExists) {
  quoll::AnimatorAsset animatorAsset{};
  animatorAsset.states.push_back({.name = "StateA"});
  animatorAsset.states.push_back({.name = "StateB"});

  auto handle =
      assetCache.getRegistry().getAnimators().addAsset({.data = animatorAsset});

  auto entity = entityDatabase.entity();
  quoll::Animator animator{};
  animator.asset = handle;
  animator.normalizedTime = 0.4f;
  animator.currentState = 1;
  entity.set(animator);

  call(entity, "animatorPropertiesValid");
}

TEST_F(AnimatorLuaTableTest, PropertiesReturnNilIfAnimatorDoesNotExists) {
  auto entity = entityDatabase.entity();

  call(entity, "animatorPropertiesInvalid");
}

TEST_F(AnimatorLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.entity();

  call(entity, "animatorDelete");
  EXPECT_FALSE(entity.has<quoll::Animator>());
}

TEST_F(AnimatorLuaTableTest, DeleteRemovesAnimatorSourceComponentFromEntity) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::Animator>({});

  call(entity, "animatorDelete");
  EXPECT_FALSE(entity.has<quoll::Animator>());
}
