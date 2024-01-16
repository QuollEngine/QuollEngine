#include "quoll/core/Base.h"
#include "quoll/core/Property.h"
#include "quoll-tests/Testing.h"

TEST(PropertyTest, SetsInt32Value) {
  quoll::Property property(-232);

  EXPECT_EQ(property.getType(), quoll::Property::INT32);
  EXPECT_EQ(property.getSize(), sizeof(i32));
  EXPECT_EQ(property.getValue<i32>(), -232);
  EXPECT_EQ(property.toString(), "-232");
}

TEST(PropertyTest, SetsUint32Value) {
  quoll::Property property(232u);

  EXPECT_EQ(property.getType(), quoll::Property::UINT32);
  EXPECT_EQ(property.getSize(), sizeof(u32));
  EXPECT_EQ(property.getValue<u32>(), 232);
  EXPECT_EQ(property.toString(), "232");
}

TEST(PropertyTest, SetsUint64Value) {
  u64 value = 232;
  quoll::Property property(value);

  EXPECT_EQ(property.getType(), quoll::Property::UINT64);
  EXPECT_EQ(property.getSize(), sizeof(u64));
  EXPECT_EQ(property.getValue<u64>(), 232);
  EXPECT_EQ(property.toString(), "232");
}

TEST(PropertyTest, SetsRealValue) {
  quoll::Property property(12.0f);

  EXPECT_EQ(property.getType(), quoll::Property::REAL);
  EXPECT_EQ(property.getSize(), sizeof(f32));
  EXPECT_EQ(property.getValue<f32>(), 12.0f);
  EXPECT_EQ(property.toString(), "12.00");
}

TEST(PropertyTest, SetsVector2Value) {
  quoll::Property property(glm::vec2(1.0f, 0.4f));

  EXPECT_EQ(property.getType(), quoll::Property::VECTOR2);
  EXPECT_EQ(property.getSize(), sizeof(glm::vec2));
  EXPECT_TRUE(property.getValue<glm::vec2>() == glm::vec2(1.0f, 0.4f));
  EXPECT_EQ(property.toString(), "(1.00, 0.40)");
}

TEST(PropertyTest, SetsVector3Value) {
  quoll::Property property(glm::vec3(1.0f, 0.4f, 0.2f));

  EXPECT_EQ(property.getType(), quoll::Property::VECTOR3);
  EXPECT_EQ(property.getSize(), sizeof(glm::vec3));
  EXPECT_TRUE(property.getValue<glm::vec3>() == glm::vec3(1.0f, 0.4f, 0.2f));
  EXPECT_EQ(property.toString(), "(1.00, 0.40, 0.20)");
}

TEST(PropertyTest, SetsVector4Value) {
  quoll::Property property(glm::vec4(1.0f, 0.45f, 0.2f, 1.0f));

  EXPECT_EQ(property.getType(), quoll::Property::VECTOR4);
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

  quoll::Property property(testMatrix);

  EXPECT_EQ(property.getType(), quoll::Property::MATRIX4);
  EXPECT_EQ(property.getSize(), sizeof(glm::mat4));
  EXPECT_TRUE(property.getValue<glm::mat4>() == testMatrix);
  EXPECT_EQ(property.toString(),
            "[[1.00, 0.45, 0.20, 1.00], [-1.00, 0.45, -0.20, 1.00], [1.00, "
            "-0.45, 0.20, -1.00], [-1.00, -0.45, -0.20, -1.00]]");
}

TEST(PropertyDeathTest, UndefinedTypeThrowsError) {
  quoll::Property property(12.0f);
  EXPECT_DEATH(property.getValue<std::string>(), ".*");
}

TEST(PropertyDeathTest, CastingToWrongTypeThrowsError) {
  quoll::Property p1(12.0f);

  EXPECT_DEATH(p1.getValue<i32>(), ".*");
  EXPECT_DEATH(p1.getValue<u32>(), ".*");
  EXPECT_DEATH(p1.getValue<u64>(), ".*");
  EXPECT_DEATH(p1.getValue<glm::vec2>(), ".*");
  EXPECT_DEATH(p1.getValue<glm::vec3>(), ".*");
  EXPECT_DEATH(p1.getValue<glm::vec4>(), ".*");
  EXPECT_DEATH(p1.getValue<glm::mat4>(), ".*");

  quoll::Property p2(glm::vec2{1.0f, 1.0f});
  EXPECT_DEATH(p2.getValue<f32>(), ".*");
}
