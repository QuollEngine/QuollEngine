#include "quoll/core/Base.h"
#include "quoll/renderer/Material.h"
#include "quoll/rhi-mock/MockRenderDevice.h"

#include "quoll-tests/Testing.h"

class MaterialTest : public ::testing::Test {
public:
  MaterialTest() : renderStorage(&device) {}

  quoll::rhi::MockRenderDevice device;
  quoll::RenderStorage renderStorage;
};

TEST_F(MaterialTest, SetsBuffersAndTextures) {
  std::vector<quoll::rhi::TextureHandle> textures{quoll::rhi::TextureHandle(1)};

  quoll::Material material(
      "test", textures,
      {
          {"specular", quoll::Property(glm::vec3(0.5, 0.2, 0.3))},
          {"diffuse", quoll::Property(glm::vec4(1.0, 1.0, 1.0, 1.0))},
      },
      renderStorage);

  EXPECT_EQ(material.getTextures(), textures);
  EXPECT_EQ(material.hasTextures(), true);
  EXPECT_TRUE(quoll::rhi::isHandleValid(material.getBuffer()));

  auto *buffer = device.getBuffer(material.getBuffer());
  const char *data = static_cast<const char *>(buffer->map());

  auto specularVal = *reinterpret_cast<const glm::vec3 *>(data);
  auto diffuseVal =
      *reinterpret_cast<const glm::vec4 *>(data + sizeof(glm::vec4));

  EXPECT_EQ(buffer->getDescription().size, sizeof(glm::vec4) * 2);
  EXPECT_TRUE(specularVal == glm::vec3(0.5, 0.2, 0.3));
  EXPECT_TRUE(diffuseVal == glm::vec4(1.0, 1.0, 1.0, 1.0));
}

TEST_F(MaterialTest, DoesNotCreateBuffersIfEmptyProperties) {
  std::vector<quoll::rhi::TextureHandle> textures{quoll::rhi::TextureHandle(1)};

  quoll::Material material("test", textures, {}, renderStorage);

  EXPECT_EQ(material.getTextures(), textures);
  EXPECT_EQ(material.hasTextures(), true);
  EXPECT_FALSE(quoll::rhi::isHandleValid(material.getBuffer()));
}

TEST_F(MaterialTest, DoesNotSetTexturesIfNoTexture) {
  quoll::Material material("test", {}, {}, renderStorage);

  EXPECT_EQ(material.getTextures().size(), 0);
  EXPECT_EQ(material.hasTextures(), false);
  EXPECT_FALSE(quoll::rhi::isHandleValid(material.getBuffer()));
}

TEST_F(MaterialTest, DoesNotUpdatePropertyIfPropertyDoesNotExist) {
  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  quoll::Material material("test", {},
                           {
                               {"specular", quoll::Property(testVec3)},
                               {"diffuse", quoll::Property(testReal)},
                           },
                           renderStorage);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    auto *buffer = device.getBuffer(material.getBuffer());
    const char *data = static_cast<const char *>(buffer->map());

    EXPECT_EQ(buffer->getDescription().size, sizeof(glm::vec3) * 2);
    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  material.updateProperty("non-existent-property", quoll::Property(1.0f));

  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    auto *buffer = device.getBuffer(material.getBuffer());
    const char *data = static_cast<const char *>(buffer->map());

    EXPECT_EQ(buffer->getDescription().size, sizeof(glm::vec3) * 2);
    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }
}

TEST_F(MaterialTest, DoesNotUpdatePropertyIfNewPropertyTypeIsDifferent) {
  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  quoll::Material material("test", {},
                           {
                               {"specular", quoll::Property(testVec3)},
                               {"diffuse", quoll::Property(testReal)},
                           },
                           renderStorage);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    auto *buffer = device.getBuffer(material.getBuffer());
    const char *data = static_cast<const char *>(buffer->map());

    EXPECT_EQ(buffer->getDescription().size, sizeof(glm::vec3) * 2);
    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  material.updateProperty("specular", quoll::Property(1.0f));

  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    auto *buffer = device.getBuffer(material.getBuffer());
    const char *data = static_cast<const char *>(buffer->map());

    EXPECT_EQ(buffer->getDescription().size, sizeof(glm::vec3) * 2);
    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }
}

TEST_F(MaterialTest, UpdatesPropertyIfNameAndTypeMatch) {
  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  quoll::Material material("test", {},
                           {
                               {"specular", quoll::Property(testVec3)},
                               {"diffuse", quoll::Property(testReal)},
                           },
                           renderStorage);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    auto *buffer = device.getBuffer(material.getBuffer());
    const char *data = static_cast<const char *>(buffer->map());

    EXPECT_EQ(buffer->getDescription().size, sizeof(glm::vec3) * 2);
    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  glm::vec3 newTestVec3{2.6, 0.1, 5.3};
  float newTestReal = 78.2f;
  material.updateProperty("specular", quoll::Property(newTestVec3));
  material.updateProperty("diffuse", quoll::Property(newTestReal));

  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == newTestVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == newTestReal);

    auto *buffer = device.getBuffer(material.getBuffer());
    const char *data = static_cast<const char *>(buffer->map());

    EXPECT_EQ(buffer->getDescription().size, sizeof(glm::vec3) * 2);
    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == newTestVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                newTestReal);
  }
}
