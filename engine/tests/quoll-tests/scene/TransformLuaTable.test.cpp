#include "quoll/core/Base.h"
#include "quoll/scene/LocalTransform.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class TransformLuaTableTest : public LuaScriptingInterfaceTestBase {};

// Position
TEST_F(TransformLuaTableTest,
       GetPositionReturnsNilIfNoLocalTransformComponent) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "localTransformPositionGet");

  EXPECT_TRUE(state["localTransformPosition"].is<sol::nil_t>());
}

TEST_F(TransformLuaTableTest, GetsPositionValue) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({glm::vec3(2.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformPositionGet");

  EXPECT_EQ(state["localTransformPosition"].get<glm::vec3>(),
            glm::vec3(2.5f, 0.2f, 0.5f));
}

TEST_F(TransformLuaTableTest,
       SetPositionCreatesLocalTransformIfItDoesNotExist) {
  auto entity = entityDatabase.entity();
  EXPECT_FALSE(entity.has<quoll::LocalTransform>());
  call(entity, "localTransformPositionSet");

  EXPECT_TRUE(entity.has<quoll::LocalTransform>());
  EXPECT_EQ(entity.get_ref<quoll::LocalTransform>()->localPosition,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(TransformLuaTableTest, SetsPositionValue) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformPositionSet");

  EXPECT_EQ(entity.get_ref<quoll::LocalTransform>()->localPosition,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(TransformLuaTableTest, SetPositionValuesInvidiually) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformPositionSetIndividual");

  EXPECT_EQ(entity.get_ref<quoll::LocalTransform>()->localPosition,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

// Scale
TEST_F(TransformLuaTableTest, GetScaleReturnsNilIfNoLocalTransformComponent) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "localTransformScaleGet");

  EXPECT_TRUE(state["localTransformScale"].is<sol::nil_t>());
}

TEST_F(TransformLuaTableTest, GetsScaleValue) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({{}, {}, glm::vec3(2.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformScaleGet");

  EXPECT_EQ(state["localTransformScale"].get<glm::vec3>(),
            glm::vec3(2.5f, 0.2f, 0.5f));
}

TEST_F(TransformLuaTableTest, SetScaleCreatesLocalTransformIfItDoesNotExist) {
  auto entity = entityDatabase.entity();
  EXPECT_FALSE(entity.has<quoll::LocalTransform>());
  call(entity, "localTransformScaleSet");

  EXPECT_TRUE(entity.has<quoll::LocalTransform>());
  EXPECT_EQ(entity.get_ref<quoll::LocalTransform>()->localScale,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(TransformLuaTableTest, SetsScaleValue) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformScaleSet");

  EXPECT_EQ(entity.get_ref<quoll::LocalTransform>()->localScale,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(TransformLuaTableTest, SetsScaleValuesIndividually) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto state = call(entity, "localTransformScaleSetIndividual");

  EXPECT_EQ(entity.get_ref<quoll::LocalTransform>()->localScale,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

// XYZ -> Roll-Pitch-Yaw
constexpr glm::quat TestQuat{0.835327f, 0.350225f, 0.078362f, 0.416451f};

TEST_F(TransformLuaTableTest,
       GetRotationReturnsNilIfNoLocalTransformComponent) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "localTransformRotationGet");

  EXPECT_TRUE(state["localTransformRotation"].is<sol::nil_t>());
}

TEST_F(TransformLuaTableTest, GetRotationReturnsRotationQuaternion) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({{}, TestQuat, {}});

  auto state = call(entity, "localTransformRotationGet");

  auto actual = state["localTransformRotation"].get<glm::quat>();
  auto expected = TestQuat;

  EXPECT_NEAR(actual.x, expected.x, 0.0001f);
  EXPECT_NEAR(actual.y, expected.y, 0.0001f);
  EXPECT_NEAR(actual.z, expected.z, 0.0001f);
}

TEST_F(TransformLuaTableTest,
       SetRotationCreatesLocalTransformIfItDoesNotExist) {
  auto entity = entityDatabase.entity();
  EXPECT_FALSE(entity.has<quoll::LocalTransform>());
  call(entity, "localTransformRotationSet");

  EXPECT_TRUE(entity.has<quoll::LocalTransform>());
  auto actual = entity.get_ref<quoll::LocalTransform>()->localRotation;
  auto expected = TestQuat;

  EXPECT_NEAR(actual.x, expected.x, 0.0001f);
  EXPECT_NEAR(actual.y, expected.y, 0.0001f);
  EXPECT_NEAR(actual.z, expected.z, 0.0001f);
  EXPECT_NEAR(actual.w, expected.w, 0.0001f);
}

TEST_F(TransformLuaTableTest, SetRotationValue) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({{}, {}, {}});

  auto state = call(entity, "localTransformRotationSet");

  auto actual = entity.get_ref<quoll::LocalTransform>()->localRotation;
  auto expected = TestQuat;

  EXPECT_NEAR(actual.x, expected.x, 0.0001f);
  EXPECT_NEAR(actual.y, expected.y, 0.0001f);
  EXPECT_NEAR(actual.z, expected.z, 0.0001f);
  EXPECT_NEAR(actual.w, expected.w, 0.0001f);
}

TEST_F(TransformLuaTableTest, SetRotationValueIndividually) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({{}, {}, {}});

  auto state = call(entity, "localTransformRotationSetIndividual");

  auto actual = entity.get_ref<quoll::LocalTransform>()->localRotation;
  auto expected = TestQuat;

  EXPECT_NEAR(actual.x, expected.x, 0.0001f);
  EXPECT_NEAR(actual.y, expected.y, 0.0001f);
  EXPECT_NEAR(actual.z, expected.z, 0.0001f);
  EXPECT_NEAR(actual.w, expected.w, 0.0001f);
}

TEST_F(TransformLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.entity();

  call(entity, "localTransformDelete");
  EXPECT_FALSE(entity.has<quoll::LocalTransform>());
}

TEST_F(TransformLuaTableTest, DeleteRemovesLocalTransformComponentFromEntity) {
  auto entity = entityDatabase.entity();
  entity.set<quoll::LocalTransform>({});

  call(entity, "localTransformDelete");
  EXPECT_FALSE(entity.has<quoll::LocalTransform>());
}
