#include "liquid/core/Base.h"
#include "liquid/scene/Light.h"

#include <glm/gtx/string_cast.hpp>
#include <gtest/gtest.h>

TEST(Light, TypeColorAndPowerIsSet) {
  liquid::Light light(liquid::Light::DIRECTIONAL, {2.0, 2.5, -1.5},
                      {1.0, 0.0, 1.0, 1.0}, 500);

  EXPECT_TRUE(light.getColor() == glm::vec4(1.0f, 0.0, 1.0, 1.0));
  EXPECT_TRUE(light.getDirection() == glm::vec3(2.0, 2.5, -1.5));
  EXPECT_EQ(light.getIntensity(), 500);
  EXPECT_EQ(light.getType(), liquid::Light::DIRECTIONAL);
  EXPECT_EQ(light.getTypeName(), "Directional");
}

TEST(Light, PositionSetter) {
  liquid::Light light(liquid::Light::DIRECTIONAL, {2.0, 2.5, -1.5},
                      {1.0, 0.0, 1.0, 1.0}, 500);

  EXPECT_TRUE(light.getPosition() == glm::vec3(0.0f, 0.0f, 0.0f));

  light.setPosition({1.2f, 1.0f, 1.0});
  EXPECT_TRUE(light.getPosition() == glm::vec3(1.2f, 1.0f, 1.0f));
}

TEST(Light, DirectionSetter) {
  liquid::Light light(liquid::Light::DIRECTIONAL, {2.0, 2.5, -1.5},
                      {1.0, 0.0, 1.0, 1.0}, 500);

  EXPECT_TRUE(light.getDirection() == glm::vec3(2.0, 2.5, -1.5));

  light.setDirection({1.2f, 1.0f, 1.0});
  EXPECT_TRUE(light.getDirection() == glm::vec3(1.2f, 1.0f, 1.0f));
}

TEST(Light, ColorSetter) {
  liquid::Light light(liquid::Light::DIRECTIONAL, {2.0, 2.5, -1.5},
                      {1.0, 0.0, 1.0, 1.0}, 500);

  EXPECT_TRUE(light.getColor() == glm::vec4(1.0, 0.0, 1.0, 1.0));

  light.setColor({0.3f, 1.0f, 0.2, 1.0});
  EXPECT_TRUE(light.getColor() == glm::vec4({0.3f, 1.0f, 0.2, 1.0}));
}

TEST(Light, IntensitySetter) {
  liquid::Light light(liquid::Light::DIRECTIONAL, {2.0, 2.5, -1.5},
                      {1.0, 0.0, 1.0, 1.0}, 500);

  EXPECT_TRUE(light.getIntensity() == 500.0f);

  light.setIntensity(250.f);
  EXPECT_TRUE(light.getIntensity() == 250.f);
}

TEST(Light, CalculatesProjectionViewMatrix) {
  liquid::Light light(liquid::Light::DIRECTIONAL, {2.0, 2.5, -1.5},
                      {1.0, 0.0, 1.0, 1.0}, 500);

  auto expectedString =
      "mat4x4((0.030128, -0.028182, 0.005650, 0.000000), (0.000000, 0.035398, "
      "0.007063, 0.000000), (0.039904, 0.021278, -0.004266, 0.000000), "
      "(1.264958, 0.674506, 0.608126, 1.000000))";

  EXPECT_EQ(glm::to_string(light.getProjectionViewMatrix()), expectedString);
}
