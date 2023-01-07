#include "liquid/core/Base.h"
#include "liquid/renderer/Material.h"
#include "liquid/rhi/NativeBuffer.h"

#include "liquid-tests/mocks/MockRenderDevice.h"
#include "liquid-tests/Testing.h"

class MaterialTest : public ::testing::Test {
public:
  MaterialTest() : renderStorage(&device) {}

  MockRenderDevice device;
  liquid::RenderStorage renderStorage;
};

TEST_F(MaterialTest, SetsBuffersAndTextures) {
  std::vector<liquid::rhi::TextureHandle> textures{
      liquid::rhi::TextureHandle(1)};

  liquid::Material material(
      textures,
      {
          {"specular", liquid::Property(glm::vec3(0.5, 0.2, 0.3))},
          {"diffuse", liquid::Property(glm::vec4(1.0, 1.0, 1.0, 1.0))},
      },
      renderStorage);

  EXPECT_EQ(material.getTextures(), textures);
  EXPECT_EQ(material.hasTextures(), true);
  EXPECT_TRUE(liquid::rhi::isHandleValid(material.getBuffer()));

  const auto &buffer = device.getBuffer(material.getBuffer());

  // Memory alignment
  EXPECT_EQ(buffer.getSize(), sizeof(glm::vec4) * 2);

  EXPECT_EQ(material.getDescriptor().getBindings().at(0).type,
            liquid::rhi::DescriptorType::UniformBuffer);

  const char *data = static_cast<const char *>(buffer.getData());

  auto specularVal = *reinterpret_cast<const glm::vec3 *>(data);
  auto diffuseVal =
      *reinterpret_cast<const glm::vec4 *>(data + sizeof(glm::vec4));

  EXPECT_TRUE(specularVal == glm::vec3(0.5, 0.2, 0.3));
  EXPECT_TRUE(diffuseVal == glm::vec4(1.0, 1.0, 1.0, 1.0));
}

TEST_F(MaterialTest, DoesNotCreateBuffersIfEmptyProperties) {
  std::vector<liquid::rhi::TextureHandle> textures{
      liquid::rhi::TextureHandle(1)};

  liquid::Material material(textures, {}, renderStorage);

  EXPECT_EQ(material.getTextures(), textures);
  EXPECT_EQ(material.hasTextures(), true);
  EXPECT_FALSE(liquid::rhi::isHandleValid(material.getBuffer()));
  EXPECT_EQ(material.getDescriptor().getBindings().find(0),
            material.getDescriptor().getBindings().end());
}

TEST_F(MaterialTest, DoesNotSetTexturesIfNoTexture) {
  liquid::Material material({}, {}, renderStorage);

  EXPECT_EQ(material.getTextures().size(), 0);
  EXPECT_EQ(material.hasTextures(), false);
  EXPECT_FALSE(liquid::rhi::isHandleValid(material.getBuffer()));
}

TEST_F(MaterialTest, DoesNotUpdatePropertyIfPropertyDoesNotExist) {
  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  liquid::Material material({},
                            {
                                {"specular", liquid::Property(testVec3)},
                                {"diffuse", liquid::Property(testReal)},
                            },
                            renderStorage);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    const auto &buffer = device.getBuffer(material.getBuffer());

    EXPECT_EQ(buffer.getSize(), sizeof(glm::vec3) * 2);
    const auto *data = static_cast<const char *>(buffer.getData());

    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  material.updateProperty("non-existent-property", liquid::Property(1.0f));

  {
    const auto &buffer = device.getBuffer(material.getBuffer());

    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);
    EXPECT_EQ(buffer.getSize(), sizeof(glm::vec3) * 2);
    const auto *data = static_cast<const char *>(buffer.getData());

    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }
}

TEST_F(MaterialTest, DoesNotUpdatePropertyIfNewPropertyTypeIsDifferent) {
  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  liquid::Material material({},
                            {
                                {"specular", liquid::Property(testVec3)},
                                {"diffuse", liquid::Property(testReal)},
                            },
                            renderStorage);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    const auto &buffer = device.getBuffer(material.getBuffer());

    EXPECT_EQ(buffer.getSize(), sizeof(glm::vec3) * 2);
    const auto *data = static_cast<const char *>(buffer.getData());

    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  material.updateProperty("specular", liquid::Property(1.0f));

  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    const auto &buffer = device.getBuffer(material.getBuffer());

    EXPECT_EQ(buffer.getSize(), sizeof(glm::vec3) * 2);
    const auto *data = static_cast<const char *>(buffer.getData());

    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }
}

TEST_F(MaterialTest, UpdatesPropertyIfNameAndTypeMatch) {
  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  liquid::Material material({},
                            {
                                {"specular", liquid::Property(testVec3)},
                                {"diffuse", liquid::Property(testReal)},
                            },
                            renderStorage);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    const auto &buffer = device.getBuffer(material.getBuffer());

    EXPECT_EQ(buffer.getSize(), sizeof(glm::vec3) * 2);
    const auto *data = static_cast<const char *>(buffer.getData());

    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  glm::vec3 newTestVec3{2.6, 0.1, 5.3};
  float newTestReal = 78.2f;
  material.updateProperty("specular", liquid::Property(newTestVec3));
  material.updateProperty("diffuse", liquid::Property(newTestReal));

  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == newTestVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == newTestReal);

    const auto &buffer = device.getBuffer(material.getBuffer());

    EXPECT_EQ(buffer.getSize(), sizeof(glm::vec3) * 2);
    const auto *data = static_cast<const char *>(buffer.getData());

    EXPECT_TRUE(*reinterpret_cast<const glm::vec3 *>(data) == newTestVec3);
    EXPECT_TRUE(*reinterpret_cast<const float *>(data + sizeof(glm::vec3)) ==
                newTestReal);
  }
}
