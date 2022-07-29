#include "liquid/core/Base.h"
#include "liquid-tests/Testing.h"

#include "liquid-tests/test-utils/ScriptingInterfaceTestBase.h"

class TransformLuaScriptingInterfaceTest
    : public LuaScriptingInterfaceTestBase {};

using TransformLuaScriptingInterfaceDeathTest =
    TransformLuaScriptingInterfaceTest;

TEST_F(TransformLuaScriptingInterfaceDeathTest,
       GetPositionFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_position_get"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest,
       GetPositionReturnsNilIfValuesAreInvalid) {
  auto entity = entityDatabase.createEntity();
  auto &scope = call(entity, "local_transform_position_get_invalid");

  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_position_x"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_position_y"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_position_z"));
}

TEST_F(TransformLuaScriptingInterfaceTest, GetsPositionValue) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {glm::vec3(2.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_position_get");

  EXPECT_EQ(scope.getGlobal<float>("local_position_x"), 2.5f);
  EXPECT_EQ(scope.getGlobal<float>("local_position_y"), 0.2f);
  EXPECT_EQ(scope.getGlobal<float>("local_position_z"), 0.5f);
}

TEST_F(TransformLuaScriptingInterfaceDeathTest,
       SetPositionFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_position_set"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest, SetsPositionValue) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_position_set");

  EXPECT_EQ(entityDatabase.getComponent<liquid::LocalTransformComponent>(entity)
                .localPosition,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(TransformLuaScriptingInterfaceTest,
       DoesNothingIfSetPositionArgumentsAreInvalid) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_position_set_invalid");

  EXPECT_EQ(entityDatabase.getComponent<liquid::LocalTransformComponent>(entity)
                .localScale,
            glm::vec3(1.5f, 0.2f, 0.5f));
}

TEST_F(TransformLuaScriptingInterfaceDeathTest,
       GetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_scale_get"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest,
       GetScaleReturnsNilIfValuesAreInvalid) {
  auto entity = entityDatabase.createEntity();
  auto &scope = call(entity, "local_transform_scale_get_invalid");

  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_scale_x"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_scale_y"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("local_scale_z"));
}

TEST_F(TransformLuaScriptingInterfaceTest, GetsScaleValue) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(2.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_scale_get");

  EXPECT_EQ(scope.getGlobal<float>("local_scale_x"), 2.5f);
  EXPECT_EQ(scope.getGlobal<float>("local_scale_y"), 0.2f);
  EXPECT_EQ(scope.getGlobal<float>("local_scale_z"), 0.5f);
}

TEST_F(TransformLuaScriptingInterfaceDeathTest,
       SetScaleFailsIfComponentDoesNotExist) {
  auto entity = entityDatabase.createEntity();
  EXPECT_DEATH(call(entity, "local_transform_scale_set"), ".*");
}

TEST_F(TransformLuaScriptingInterfaceTest, SetsScaleValue) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_scale_set");

  EXPECT_EQ(entityDatabase.getComponent<liquid::LocalTransformComponent>(entity)
                .localScale,
            glm::vec3(2.5f, 3.5f, 0.2f));
}

TEST_F(TransformLuaScriptingInterfaceTest,
       DoesNothingIfSetScaleArgumentsAreNotNumbers) {
  auto entity = entityDatabase.createEntity();
  entityDatabase.setComponent<liquid::LocalTransformComponent>(
      entity, {{}, {}, glm::vec3(1.5f, 0.2f, 0.5f)});

  auto &scope = call(entity, "local_transform_scale_set_invalid");

  EXPECT_EQ(entityDatabase.getComponent<liquid::LocalTransformComponent>(entity)
                .localScale,
            glm::vec3(1.5f, 0.2f, 0.5f));
}
