#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class PerspectiveLensLuaScriptingInterfaceTest
    : public LuaScriptingInterfaceTestBase {};

TEST_F(PerspectiveLensLuaScriptingInterfaceTest,
       GetReturnsNilIfInvalidParamsAreProvided) {
  auto entity = entityDatabase.create();
  call(entity, "perspective_lens_get_invalid");
}

TEST_F(PerspectiveLensLuaScriptingInterfaceTest,
       GetReturnsNilIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "perspective_lens_get");

  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("pr_near"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("pr_far"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("pr_sensor_width"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("pr_sensor_height"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("pr_focal_length"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("pr_aperture"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("pr_shutter_speed"));
  EXPECT_TRUE(scope.isGlobal<std::nullptr_t>("pr_sensitivity"));
}

TEST_F(PerspectiveLensLuaScriptingInterfaceTest, GetReturnsComponentValues) {
  auto entity = entityDatabase.create();

  quoll::PerspectiveLens lens{};
  lens.near = 0.5f;
  lens.far = 500.0f;
  lens.sensorSize = glm::vec2(15.0f, 25.0f);
  lens.focalLength = 200.0f;
  lens.aperture = 99.0f;
  lens.shutterSpeed = 1.0f / 250.0f;
  lens.sensitivity = 3000;
  entityDatabase.set(entity, lens);

  auto &scope = call(entity, "perspective_lens_get");

  EXPECT_EQ(scope.getGlobal<float>("pr_near"), lens.near);
  EXPECT_EQ(scope.getGlobal<float>("pr_far"), lens.far);
  EXPECT_EQ(scope.getGlobal<float>("pr_sensor_width"), lens.sensorSize.x);
  EXPECT_EQ(scope.getGlobal<float>("pr_sensor_height"), lens.sensorSize.y);
  EXPECT_EQ(scope.getGlobal<float>("pr_focal_length"), lens.focalLength);
  EXPECT_EQ(scope.getGlobal<float>("pr_aperture"), lens.aperture);
  EXPECT_FLOAT_EQ(scope.getGlobal<float>("pr_shutter_speed"),
                  1.0f / lens.shutterSpeed);
  EXPECT_EQ(scope.getGlobal<float>("pr_sensitivity"), lens.sensitivity);
}

TEST_F(PerspectiveLensLuaScriptingInterfaceTest,
       SetDoesNothingIfInvalidParamsAreProvided) {
  auto entity = entityDatabase.create();
  auto &scope = call(entity, "perspective_lens_set_invalid");

  EXPECT_FALSE(entityDatabase.has<quoll::PerspectiveLens>(entity));
}

TEST_F(PerspectiveLensLuaScriptingInterfaceTest,
       SetCreatesComponentIfItDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::PerspectiveLens>(entity));

  auto &scope = call(entity, "perspective_lens_set");

  EXPECT_TRUE(entityDatabase.has<quoll::PerspectiveLens>(entity));
  const auto &lens = entityDatabase.get<quoll::PerspectiveLens>(entity);

  EXPECT_EQ(lens.near, 0.004f);
  EXPECT_EQ(lens.far, 4000.0f);
  EXPECT_EQ(lens.sensorSize, glm::vec2(200.0f, 200.0f));
  EXPECT_EQ(lens.focalLength, 50.0f);
  EXPECT_EQ(lens.aperture, 65.0f);
  EXPECT_FLOAT_EQ(lens.shutterSpeed, 1.0f / 2200.0f);
  EXPECT_EQ(lens.sensitivity, 4000);
}

TEST_F(PerspectiveLensLuaScriptingInterfaceTest,
       SetStoresProvidedValuesInComponent) {
  auto entity = entityDatabase.create();
  entityDatabase.set(entity, quoll::PerspectiveLens{});

  auto &scope = call(entity, "perspective_lens_set");

  const auto &lens = entityDatabase.get<quoll::PerspectiveLens>(entity);

  EXPECT_EQ(lens.near, 0.004f);
  EXPECT_EQ(lens.far, 4000.0f);
  EXPECT_EQ(lens.sensorSize, glm::vec2(200.0f, 200.0f));
  EXPECT_EQ(lens.focalLength, 50.0f);
  EXPECT_EQ(lens.aperture, 65.0f);
  EXPECT_FLOAT_EQ(lens.shutterSpeed, 1.0f / 2200.0f);
  EXPECT_EQ(lens.sensitivity, 4000);
}

TEST_F(PerspectiveLensLuaScriptingInterfaceTest,
       DeleteDoesNothingIfProvidedArgumentsAreInvalid) {
  auto entity = entityDatabase.create();
  entityDatabase.set(entity, quoll::PerspectiveLens{});

  auto &scope = call(entity, "perspective_lens_delete_invalid");

  EXPECT_TRUE(entityDatabase.has<quoll::PerspectiveLens>(entity));
}

TEST_F(PerspectiveLensLuaScriptingInterfaceTest,
       DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  auto &scope = call(entity, "perspective_lens_delete");

  EXPECT_FALSE(entityDatabase.has<quoll::PerspectiveLens>(entity));
}

TEST_F(PerspectiveLensLuaScriptingInterfaceTest,
       DeleteRemovesComponentIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set(entity, quoll::PerspectiveLens{});

  auto &scope = call(entity, "perspective_lens_delete");

  EXPECT_FALSE(entityDatabase.has<quoll::PerspectiveLens>(entity));
}
