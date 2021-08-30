#include <gtest/gtest.h>
#include <glm/gtx/string_cast.hpp>
#include "core/Base.h"

#include "scene/Light.h"

TEST(Light, TypeColorAndPowerIsSet) {
  liquid::Light light(liquid::Light::DIRECTIONAL, {2.0, 2.5, -1.5},
                      {1.0, 0.0, 1.0, 1.0}, 500);

  EXPECT_TRUE(light.getColor() == glm::vec4(1.0f, 0.0, 1.0, 1.0));
  EXPECT_TRUE(light.getDirection() == glm::vec3(2.0, 2.5, -1.5));
  EXPECT_EQ(light.getIntensity(), 500);
  EXPECT_EQ(light.getType(), liquid::Light::DIRECTIONAL);
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
      "mat4x4((-0.029872, -0.028386, -0.005664, 0.000000), (0.000000, "
      "0.035313, -0.007080, 0.000000), (-0.040096, 0.021148, 0.004219, "
      "0.000000), (1.255006, -0.661922, 0.609502, 1.000000))";

  EXPECT_EQ(glm::to_string(light.getProjectionViewMatrix()), expectedString);
}
