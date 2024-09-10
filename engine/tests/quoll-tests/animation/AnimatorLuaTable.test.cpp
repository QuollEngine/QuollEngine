#include "quoll/core/Base.h"
#include "quoll/animation/Animator.h"
#include "quoll/animation/AnimatorEvent.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

using AnimatorLuaTableTest = LuaScriptingInterfaceTestBase;

TEST_F(AnimatorLuaTableTest, TriggerAddsAnimatorEventComponent) {
  auto entity = entityDatabase.create();
  call(entity, "animatorTrigger");

  EXPECT_TRUE(entityDatabase.has<quoll::AnimatorEvent>(entity));
  EXPECT_EQ(entityDatabase.get<quoll::AnimatorEvent>(entity).eventName, "Move");
}

TEST_F(AnimatorLuaTableTest, PropertiesReturnAnimatorDataIfAnimatorExists) {
  quoll::AnimatorAsset animatorAsset{};
  animatorAsset.states.push_back({.name = "StateA"});
  animatorAsset.states.push_back({.name = "StateB"});

  auto asset = createAsset<quoll::AnimatorAsset>(
      {.states = {{.name = "StateA"}, {.name = "StateB"}}});

  auto entity = entityDatabase.create();
  quoll::Animator animator{};
  animator.asset = asset;
  animator.normalizedTime = 0.4f;
  animator.currentState = 1;
  entityDatabase.set(entity, animator);

  call(entity, "animatorPropertiesValid");
}

TEST_F(AnimatorLuaTableTest, PropertiesReturnNilIfAnimatorDoesNotExists) {
  auto entity = entityDatabase.create();

  call(entity, "animatorPropertiesInvalid");
}

TEST_F(AnimatorLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  call(entity, "animatorDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::Animator>(entity));
}

TEST_F(AnimatorLuaTableTest, DeleteRemovesAllAnimatorComponentsFromEntity) {
  auto entity = entityDatabase.create();
  entityDatabase.set<quoll::AnimatorAssetRef>(entity, {});
  entityDatabase.set<quoll::AnimatorCurrentAsset>(entity, {});
  entityDatabase.set<quoll::Animator>(entity, {});

  call(entity, "animatorDelete");
  EXPECT_FALSE(entityDatabase.has<quoll::AnimatorAssetRef>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::AnimatorCurrentAsset>(entity));
  EXPECT_FALSE(entityDatabase.has<quoll::Animator>(entity));
}
