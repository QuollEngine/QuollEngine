#include "quoll/core/Base.h"
#include "quoll/scene/PerspectiveLens.h"
#include "quoll-tests/Testing.h"
#include "quoll-tests/test-utils/ScriptingInterfaceTestBase.h"

class PerspectiveLensLuaTableTest : public LuaScriptingInterfaceTestBase {};

TEST_F(PerspectiveLensLuaTableTest, GetReturnsNilIfComponentDoesNotExist) {
  auto entity = entityDatabase.entity();
  auto state = call(entity, "perspectiveLensGet");

  EXPECT_TRUE(state["prNear"].is<sol::nil_t>());
  EXPECT_TRUE(state["prFar"].is<sol::nil_t>());
  EXPECT_TRUE(state["prSensorWidth"].is<sol::nil_t>());
  EXPECT_TRUE(state["prSensorHeight"].is<sol::nil_t>());
  EXPECT_TRUE(state["prFocalLength"].is<sol::nil_t>());
  EXPECT_TRUE(state["prAperture"].is<sol::nil_t>());
  EXPECT_TRUE(state["prShutterSpeed"].is<sol::nil_t>());
  EXPECT_TRUE(state["prSensitivity"].is<sol::nil_t>());
}

TEST_F(PerspectiveLensLuaTableTest, GetReturnsComponentValues) {
  auto entity = entityDatabase.entity();

  quoll::PerspectiveLens lens{};
  lens.near = 0.5f;
  lens.far = 500.0f;
  lens.sensorSize = glm::vec2(15.0f, 25.0f);
  lens.focalLength = 200.0f;
  lens.aperture = 99.0f;
  lens.shutterSpeed = 1.0f / 250.0f;
  lens.sensitivity = 3000;
  entity.set(lens);

  auto state = call(entity, "perspectiveLensGet");

  EXPECT_EQ(state["prNear"].get<f32>(), lens.near);
  EXPECT_EQ(state["prFar"].get<f32>(), lens.far);
  EXPECT_EQ(state["prSensorWidth"].get<f32>(), lens.sensorSize.x);
  EXPECT_EQ(state["prSensorHeight"].get<f32>(), lens.sensorSize.y);
  EXPECT_EQ(state["prFocalLength"].get<f32>(), lens.focalLength);
  EXPECT_EQ(state["prAperture"].get<f32>(), lens.aperture);
  EXPECT_FLOAT_EQ(state["prShutterSpeed"].get<f32>(), 1.0f / lens.shutterSpeed);
  EXPECT_EQ(state["prSensitivity"].get<f32>(), lens.sensitivity);
}

TEST_F(PerspectiveLensLuaTableTest, SetCreatesComponentIfItDoesNotExist) {
  auto entity = entityDatabase.entity();
  EXPECT_FALSE(entity.has<quoll::PerspectiveLens>());

  auto state = call(entity, "perspectiveLensSet");

  EXPECT_TRUE(entity.has<quoll::PerspectiveLens>());
  auto lens = entity.get_ref<quoll::PerspectiveLens>();

  EXPECT_EQ(lens->near, 0.004f);
  EXPECT_EQ(lens->far, 4000.0f);
  EXPECT_EQ(lens->sensorSize, glm::vec2(200.0f, 200.0f));
  EXPECT_EQ(lens->focalLength, 50.0f);
  EXPECT_EQ(lens->aperture, 65.0f);
  EXPECT_FLOAT_EQ(lens->shutterSpeed, 1.0f / 2200.0f);
  EXPECT_EQ(lens->sensitivity, 4000);
}

TEST_F(PerspectiveLensLuaTableTest, SetStoresProvidedValuesInComponent) {
  auto entity = entityDatabase.entity();
  entity.set(quoll::PerspectiveLens{});

  auto state = call(entity, "perspectiveLensSet");

  auto lens = entity.get_ref<quoll::PerspectiveLens>();

  EXPECT_EQ(lens->near, 0.004f);
  EXPECT_EQ(lens->far, 4000.0f);
  EXPECT_EQ(lens->sensorSize, glm::vec2(200.0f, 200.0f));
  EXPECT_EQ(lens->focalLength, 50.0f);
  EXPECT_EQ(lens->aperture, 65.0f);
  EXPECT_FLOAT_EQ(lens->shutterSpeed, 1.0f / 2200.0f);
  EXPECT_EQ(lens->sensitivity, 4000);
}

TEST_F(PerspectiveLensLuaTableTest, DeleteDoesNothingIfComponentDoesNotExist) {
  auto entity = entityDatabase.entity();

  auto state = call(entity, "perspectiveLensDelete");

  EXPECT_FALSE(entity.has<quoll::PerspectiveLens>());
}

TEST_F(PerspectiveLensLuaTableTest, DeleteRemovesComponentIfComponentExists) {
  auto entity = entityDatabase.entity();
  entity.set(quoll::PerspectiveLens{});

  auto state = call(entity, "perspectiveLensDelete");

  EXPECT_FALSE(entity.has<quoll::PerspectiveLens>());
}
