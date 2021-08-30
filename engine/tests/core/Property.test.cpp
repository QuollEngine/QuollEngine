#include <gtest/gtest.h>

#include "core/Base.h"
#include "core/Property.h"

TEST(PropertyTest, SetsInt32Value) {
  liquid::Property property(-232);

  EXPECT_EQ(property.getType(), liquid::Property::INT32);
  EXPECT_EQ(property.getSize(), sizeof(int32_t));
  EXPECT_EQ(property.getValue<int32_t>(), -232);
  EXPECT_EQ(property.toString(), "-232");
}

TEST(PropertyTest, SetsUint32Value) {
  liquid::Property property(232u);

  EXPECT_EQ(property.getType(), liquid::Property::UINT32);
  EXPECT_EQ(property.getSize(), sizeof(uint32_t));
  EXPECT_EQ(property.getValue<uint32_t>(), 232);
  EXPECT_EQ(property.toString(), "232");
}

TEST(PropertyTest, SetsUint64Value) {
  uint64_t value = 232;
  liquid::Property property(value);

  EXPECT_EQ(property.getType(), liquid::Property::UINT64);
  EXPECT_EQ(property.getSize(), sizeof(uint64_t));
  EXPECT_EQ(property.getValue<uint64_t>(), 232);
  EXPECT_EQ(property.toString(), "232");
}

TEST(PropertyTest, SetsRealValue) {
  liquid::Property property(12.0f);

  EXPECT_EQ(property.getType(), liquid::Property::REAL);
  EXPECT_EQ(property.getSize(), sizeof(float));
  EXPECT_EQ(property.getValue<float>(), 12.0f);
  EXPECT_EQ(property.toString(), "12.00");
}

TEST(PropertyTest, SetsVector2Value) {
  liquid::Property property(glm::vec2(1.0f, 0.4f));

  EXPECT_EQ(property.getType(), liquid::Property::VECTOR2);
  EXPECT_EQ(property.getSize(), sizeof(glm::vec2));
  EXPECT_TRUE(property.getValue<glm::vec2>() == glm::vec2(1.0f, 0.4f));
  EXPECT_EQ(property.toString(), "(1.00, 0.40)");
}

TEST(PropertyTest, SetsVector3Value) {
  liquid::Property property(glm::vec3(1.0f, 0.4f, 0.2f));

  EXPECT_EQ(property.getType(), liquid::Property::VECTOR3);
  EXPECT_EQ(property.getSize(), sizeof(glm::vec3));
  EXPECT_TRUE(property.getValue<glm::vec3>() == glm::vec3(1.0f, 0.4f, 0.2f));
  EXPECT_EQ(property.toString(), "(1.00, 0.40, 0.20)");
}

TEST(PropertyTest, SetsVector4Value) {
  liquid::Property property(glm::vec4(1.0f, 0.45f, 0.2f, 1.0f));

  EXPECT_EQ(property.getType(), liquid::Property::VECTOR4);
  EXPECT_EQ(property.getSize(), sizeof(glm::vec4));
  EXPECT_TRUE(property.getValue<glm::vec4>() ==
              glm::vec4(1.0f, 0.45f, 0.2f, 1.0f));
  EXPECT_EQ(property.toString(), "(1.00, 0.45, 0.20, 1.00)");
}

TEST(PropertyTest, SetsMatrix4Value) {
  glm::mat4 testMatrix{
      {1.0f, 0.45f, 0.2f, 1.0f},
      {-1.0f, 0.45f, -0.2f, 1.0f},
      {1.0f, -0.45f, 0.2f, -1.0f},
      {-1.0f, -0.45f, -0.2f, -1.0f},
  };

  liquid::Property property(testMatrix);

  EXPECT_EQ(property.getType(), liquid::Property::MATRIX4);
  EXPECT_EQ(property.getSize(), sizeof(glm::mat4));
  EXPECT_TRUE(property.getValue<glm::mat4>() == testMatrix);
  EXPECT_EQ(property.toString(),
            "[[1.00, 0.45, 0.20, 1.00], [-1.00, 0.45, -0.20, 1.00], [1.00, "
            "-0.45, 0.20, -1.00], [-1.00, -0.45, -0.20, -1.00]]");
}

TEST(PropertyTest, UndefinedTypeThrowsError) {
  liquid::Property property(12.0f);
  EXPECT_THROW(property.getValue<std::string>(), std::invalid_argument);
}

TEST(PropertyTest, CastingToWrongTypeThrowsError) {
  liquid::Property p1(12.0f);

  EXPECT_THROW(p1.getValue<int32_t>(), std::runtime_error);
  EXPECT_THROW(p1.getValue<uint32_t>(), std::runtime_error);
  EXPECT_THROW(p1.getValue<uint64_t>(), std::runtime_error);
  EXPECT_THROW(p1.getValue<glm::vec2>(), std::runtime_error);
  EXPECT_THROW(p1.getValue<glm::vec3>(), std::runtime_error);
  EXPECT_THROW(p1.getValue<glm::vec4>(), std::runtime_error);
  EXPECT_THROW(p1.getValue<glm::mat4>(), std::runtime_error);

  liquid::Property p2(glm::vec2{1.0f, 1.0f});
  EXPECT_THROW(p2.getValue<float>(), std::runtime_error);
}
