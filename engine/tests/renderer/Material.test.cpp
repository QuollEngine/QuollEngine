#include "liquid/core/Base.h"
#include "liquid/renderer/Material.h"

#include <gtest/gtest.h>

class MaterialTest : public ::testing::Test {
public:
  liquid::rhi::ResourceRegistry registry;
};

TEST_F(MaterialTest, SetsBuffersAndTextures) {
  std::vector<liquid::rhi::TextureHandle> textures{0};

  liquid::Material material(
      textures,
      {
          {"specular", liquid::Property(glm::vec3(0.5, 0.2, 0.3))},
          {"diffuse", liquid::Property(glm::vec4(1.0, 1.0, 1.0, 1.0))},
      },
      registry);

  const auto &description =
      registry.getBufferMap().getDescription(material.getUniformBuffer());

  EXPECT_EQ(material.getTextures(), textures);
  EXPECT_EQ(material.hasTextures(), true);
  EXPECT_NE(material.getUniformBuffer(), 0);
  // Memory alignment
  EXPECT_EQ(description.size, sizeof(glm::vec4) * 2);

  EXPECT_EQ(material.getDescriptor().getBindings().at(0).type,
            liquid::rhi::DescriptorType::UniformBuffer);
  EXPECT_EQ(material.getDescriptor().getBindings().at(1).type,
            liquid::rhi::DescriptorType::CombinedImageSampler);

  char *data = static_cast<char *>(description.data);

  auto specularVal = *reinterpret_cast<glm::vec3 *>(data);
  auto diffuseVal = *reinterpret_cast<glm::vec4 *>(data + sizeof(glm::vec4));

  EXPECT_TRUE(specularVal == glm::vec3(0.5, 0.2, 0.3));
  EXPECT_TRUE(diffuseVal == glm::vec4(1.0, 1.0, 1.0, 1.0));
}

TEST_F(MaterialTest, DoesNotCreateBuffersIfEmptyProperties) {
  std::vector<liquid::rhi::TextureHandle> textures{0};

  liquid::Material material(textures, {}, registry);

  EXPECT_EQ(material.getTextures(), textures);
  EXPECT_EQ(material.hasTextures(), true);
  EXPECT_EQ(material.getUniformBuffer(), 0);
  EXPECT_EQ(material.getDescriptor().getBindings().find(0),
            material.getDescriptor().getBindings().end());
  EXPECT_EQ(material.getDescriptor().getBindings().at(1).type,
            liquid::rhi::DescriptorType::CombinedImageSampler);
}

TEST_F(MaterialTest, DoesNotSetTexturesIfNoTexture) {
  liquid::Material material({}, {}, registry);

  EXPECT_EQ(material.getTextures().size(), 0);
  EXPECT_EQ(material.hasTextures(), false);
  EXPECT_EQ(material.getUniformBuffer(), 0);
}

TEST_F(MaterialTest, DoesNotUpdatePropertyIfPropertyDoesNotExist) {
  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  liquid::Material material({},
                            {
                                {"specular", liquid::Property(testVec3)},
                                {"diffuse", liquid::Property(testReal)},
                            },
                            registry);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    const auto &description =
        registry.getBufferMap().getDescription(material.getUniformBuffer());
    EXPECT_EQ(description.size, sizeof(glm::vec3) * 2);
    auto *data = static_cast<char *>(description.data);

    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  material.updateProperty("non-existent-property", liquid::Property(1.0f));

  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    const auto &description =
        registry.getBufferMap().getDescription(material.getUniformBuffer());
    EXPECT_EQ(description.size, sizeof(glm::vec3) * 2);
    auto *data = static_cast<char *>(description.data);

    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
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
                            registry);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    const auto &description =
        registry.getBufferMap().getDescription(material.getUniformBuffer());
    EXPECT_EQ(description.size, sizeof(glm::vec3) * 2);
    auto *data = static_cast<char *>(description.data);

    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  material.updateProperty("specular", liquid::Property(1.0f));

  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    const auto &description =
        registry.getBufferMap().getDescription(material.getUniformBuffer());
    EXPECT_EQ(description.size, sizeof(glm::vec3) * 2);
    auto *data = static_cast<char *>(description.data);

    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
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
                            registry);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);

    const auto &description =
        registry.getBufferMap().getDescription(material.getUniformBuffer());
    EXPECT_EQ(description.size, sizeof(glm::vec3) * 2);
    auto *data = static_cast<char *>(description.data);

    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
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

    const auto &description =
        registry.getBufferMap().getDescription(material.getUniformBuffer());
    EXPECT_EQ(description.size, sizeof(glm::vec3) * 2);
    auto *data = static_cast<char *>(description.data);

    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == newTestVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
                newTestReal);
  }
}
