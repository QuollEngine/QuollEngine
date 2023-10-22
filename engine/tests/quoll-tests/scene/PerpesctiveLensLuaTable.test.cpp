#include "quoll/core/Base.h"
#include "quoll-tests/Testing.h"

#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class PerspectiveLensLuaTableTest : public LuaScriptingInterfaceTestBase {};

TEST_F(PerspectiveLensLuaTableTest, GetReturnsNilIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();
  auto state = call(entity, "perspective_lens_get");

  EXPECT_TRUE(state["pr_near"].is<sol::nil_t>());
  EXPECT_TRUE(state["pr_far"].is<sol::nil_t>());
  EXPECT_TRUE(state["pr_sensor_width"].is<sol::nil_t>());
  EXPECT_TRUE(state["pr_sensor_height"].is<sol::nil_t>());
  EXPECT_TRUE(state["pr_focal_length"].is<sol::nil_t>());
  EXPECT_TRUE(state["pr_aperture"].is<sol::nil_t>());
  EXPECT_TRUE(state["pr_shutter_speed"].is<sol::nil_t>());
  EXPECT_TRUE(state["pr_sensitivity"].is<sol::nil_t>());
}

TEST_F(PerspectiveLensLuaTableTest, GetReturnsComponentValues) {
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

  auto state = call(entity, "perspective_lens_get");

  EXPECT_EQ(state["pr_near"].get<f32>(), lens.near);
  EXPECT_EQ(state["pr_far"].get<f32>(), lens.far);
  EXPECT_EQ(state["pr_sensor_width"].get<f32>(), lens.sensorSize.x);
  EXPECT_EQ(state["pr_sensor_height"].get<f32>(), lens.sensorSize.y);
  EXPECT_EQ(state["pr_focal_length"].get<f32>(), lens.focalLength);
  EXPECT_EQ(state["pr_aperture"].get<f32>(), lens.aperture);
  EXPECT_FLOAT_EQ(state["pr_shutter_speed"].get<f32>(),
                  1.0f / lens.shutterSpeed);
  EXPECT_EQ(state["pr_sensitivity"].get<f32>(), lens.sensitivity);
}

TEST_F(PerspectiveLensLuaTableTest, SetCreatesComponentIfItDoesNotExist) {
  auto entity = entityDatabase.create();
  EXPECT_FALSE(entityDatabase.has<quoll::PerspectiveLens>(entity));

  auto state = call(entity, "perspective_lens_set");

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

TEST_F(PerspectiveLensLuaTableTest, SetStoresProvidedValuesInComponent) {
  auto entity = entityDatabase.create();
  entityDatabase.set(entity, quoll::PerspectiveLens{});

  auto state = call(entity, "perspective_lens_set");

  const auto &lens = entityDatabase.get<quoll::PerspectiveLens>(entity);

  EXPECT_EQ(lens.near, 0.004f);
  EXPECT_EQ(lens.far, 4000.0f);
  EXPECT_EQ(lens.sensorSize, glm::vec2(200.0f, 200.0f));
  EXPECT_EQ(lens.focalLength, 50.0f);
  EXPECT_EQ(lens.aperture, 65.0f);
  EXPECT_FLOAT_EQ(lens.shutterSpeed, 1.0f / 2200.0f);
  EXPECT_EQ(lens.sensitivity, 4000);
}

TEST_F(PerspectiveLensLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.create();

  auto state = call(entity, "perspective_lens_delete");

  EXPECT_FALSE(entityDatabase.has<quoll::PerspectiveLens>(entity));
}

TEST_F(PerspectiveLensLuaTableTest, DeleteRemovesComponentIfComponentExists) {
  auto entity = entityDatabase.create();
  entityDatabase.set(entity, quoll::PerspectiveLens{});

  auto state = call(entity, "perspective_lens_delete");

  EXPECT_FALSE(entityDatabase.has<quoll::PerspectiveLens>(entity));
}
