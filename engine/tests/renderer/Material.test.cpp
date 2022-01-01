#include "core/Base.h"
#include "renderer/Texture.h"
#include "renderer/Material.h"

#include "../mocks/TestResourceAllocator.h"
#include <gtest/gtest.h>

using TexturePtr = liquid::SharedPtr<liquid::Texture>;

TEST(MaterialTest, SetsBuffersAndTextures) {
  std::vector<TexturePtr> textures{nullptr};

  TestResourceAllocator resourceAllocator;

  liquid::Material material(
      textures,
      {
          {"specular", liquid::Property(glm::vec3(0.5, 0.2, 0.3))},
          {"diffuse", liquid::Property(glm::vec4(1.0, 1.0, 1.0, 1.0))},
      },
      &resourceAllocator);

  EXPECT_EQ(material.getTextures(), textures);
  EXPECT_EQ(material.hasTextures(), true);
  EXPECT_NE(material.getUniformBuffer(), nullptr);
  // Memory alignment
  EXPECT_EQ(material.getUniformBuffer()->getBufferSize(),
            sizeof(glm::vec4) + sizeof(glm::vec4));

  EXPECT_EQ(material.getDescriptor().getBindings().at(0).type,
            liquid::DescriptorType::UniformBuffer);
  EXPECT_EQ(material.getDescriptor().getBindings().at(1).type,
            liquid::DescriptorType::CombinedImageSampler);

  auto *data = reinterpret_cast<char *>(
      (std::static_pointer_cast<TestBuffer>(material.getUniformBuffer()))
          ->data);

  auto specularVal = *reinterpret_cast<glm::vec3 *>(data);
  auto diffuseVal = *reinterpret_cast<glm::vec4 *>(data + sizeof(glm::vec4));

  EXPECT_TRUE(specularVal == glm::vec3(0.5, 0.2, 0.3));
  EXPECT_TRUE(diffuseVal == glm::vec4(1.0, 1.0, 1.0, 1.0));
}

TEST(MaterialTest, DoesNotCreateBuffersIfEmptyProperties) {
  std::vector<TexturePtr> textures{nullptr};

  TestResourceAllocator resourceAllocator;

  liquid::Material material(textures, {}, &resourceAllocator);

  EXPECT_EQ(material.getTextures(), textures);
  EXPECT_EQ(material.hasTextures(), true);
  EXPECT_EQ(material.getUniformBuffer(), nullptr);
  EXPECT_EQ(material.getDescriptor().getBindings().find(0),
            material.getDescriptor().getBindings().end());
  EXPECT_EQ(material.getDescriptor().getBindings().at(1).type,
            liquid::DescriptorType::CombinedImageSampler);
}

TEST(MaterialTest, DoesNotSetTexturesIfNoTexture) {
  TestResourceAllocator resourceAllocator;

  liquid::Material material({}, {}, &resourceAllocator);

  EXPECT_EQ(material.getTextures().size(), 0);
  EXPECT_EQ(material.hasTextures(), false);
  EXPECT_EQ(material.getUniformBuffer(), nullptr);
}

TEST(MaterialTest, DoesNotUpdatePropertyIfPropertyDoesNotExist) {
  TestResourceAllocator resourceAllocator;

  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  liquid::Material material({},
                            {
                                {"specular", liquid::Property(testVec3)},
                                {"diffuse", liquid::Property(testReal)},
                            },
                            &resourceAllocator);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);
    EXPECT_EQ(material.getUniformBuffer()->getBufferSize(),
              sizeof(glm::vec3) * 2);
    auto *data = reinterpret_cast<char *>(
        (std::static_pointer_cast<TestBuffer>(material.getUniformBuffer()))
            ->data);
    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  material.updateProperty("non-existent-property", liquid::Property(1.0f));

  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);
    EXPECT_EQ(material.getUniformBuffer()->getBufferSize(),
              sizeof(glm::vec3) * 2);
    auto *data = reinterpret_cast<char *>(
        (std::static_pointer_cast<TestBuffer>(material.getUniformBuffer()))
            ->data);
    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }
}

TEST(MaterialTest, DoesNotUpdatePropertyIfNewPropertyTypeIsDifferent) {
  TestResourceAllocator resourceAllocator;

  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  liquid::Material material({},
                            {
                                {"specular", liquid::Property(testVec3)},
                                {"diffuse", liquid::Property(testReal)},
                            },
                            &resourceAllocator);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);
    EXPECT_EQ(material.getUniformBuffer()->getBufferSize(),
              sizeof(glm::vec3) * 2);
    auto *data = reinterpret_cast<char *>(
        (std::static_pointer_cast<TestBuffer>(material.getUniformBuffer()))
            ->data);
    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }

  material.updateProperty("specular", liquid::Property(1.0f));

  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);
    EXPECT_EQ(material.getUniformBuffer()->getBufferSize(),
              sizeof(glm::vec3) * 2);
    auto *data = reinterpret_cast<char *>(
        (std::static_pointer_cast<TestBuffer>(material.getUniformBuffer()))
            ->data);
    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == testVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
                testReal);
  }
}

TEST(MaterialTest, UpdatesPropertyIfNameAndTypeMatch) {
  TestResourceAllocator resourceAllocator;

  glm::vec3 testVec3{1.0f, 0.2f, 3.6f};
  float testReal = 45.0f;

  liquid::Material material({},
                            {
                                {"specular", liquid::Property(testVec3)},
                                {"diffuse", liquid::Property(testReal)},
                            },
                            &resourceAllocator);

  const auto &properties = material.getProperties();
  {
    EXPECT_EQ(properties.size(), 2);
    EXPECT_TRUE(properties.at(0).getValue<glm::vec3>() == testVec3);
    EXPECT_TRUE(properties.at(1).getValue<float>() == testReal);
    EXPECT_EQ(material.getUniformBuffer()->getBufferSize(),
              sizeof(glm::vec3) * 2);
    auto *data = reinterpret_cast<char *>(
        (std::static_pointer_cast<TestBuffer>(material.getUniformBuffer()))
            ->data);
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
    EXPECT_EQ(material.getUniformBuffer()->getBufferSize(),
              sizeof(glm::vec3) * 2);
    auto *data = reinterpret_cast<char *>(
        (std::static_pointer_cast<TestBuffer>(material.getUniformBuffer()))
            ->data);
    EXPECT_TRUE(*reinterpret_cast<glm::vec3 *>(data) == newTestVec3);
    EXPECT_TRUE(*reinterpret_cast<float *>(data + sizeof(glm::vec3)) ==
                newTestReal);
  }
}
