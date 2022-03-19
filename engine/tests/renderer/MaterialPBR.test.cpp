#include "liquid/core/Base.h"
#include "liquid/renderer/MaterialPBR.h"

#include <gtest/gtest.h>

class MaterialPBRTest : public ::testing::Test {
public:
  liquid::rhi::ResourceRegistry registry;
};

TEST_F(MaterialPBRTest, GetsTextures) {
  liquid::MaterialPBR::Properties properties;
  EXPECT_EQ(properties.getTextures().size(), 0);

  properties.baseColorTexture = liquid::rhi::TextureHandle(1);
  EXPECT_EQ(properties.getTextures().size(), 1);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);

  properties.metallicRoughnessTexture = liquid::rhi::TextureHandle(2);
  EXPECT_EQ(properties.getTextures().size(), 2);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);
  EXPECT_EQ(properties.getTextures()[1], properties.metallicRoughnessTexture);

  properties.normalTexture = liquid::rhi::TextureHandle(2);
  EXPECT_EQ(properties.getTextures().size(), 3);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);
  EXPECT_EQ(properties.getTextures()[1], properties.metallicRoughnessTexture);
  EXPECT_EQ(properties.getTextures()[2], properties.normalTexture);

  properties.occlusionTexture = liquid::rhi::TextureHandle(4);
  EXPECT_EQ(properties.getTextures().size(), 4);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);
  EXPECT_EQ(properties.getTextures()[1], properties.metallicRoughnessTexture);
  EXPECT_EQ(properties.getTextures()[2], properties.normalTexture);
  EXPECT_EQ(properties.getTextures()[3], properties.occlusionTexture);

  properties.emissiveTexture = liquid::rhi::TextureHandle(5);
  EXPECT_EQ(properties.getTextures().size(), 5);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);
  EXPECT_EQ(properties.getTextures()[1], properties.metallicRoughnessTexture);
  EXPECT_EQ(properties.getTextures()[2], properties.normalTexture);
  EXPECT_EQ(properties.getTextures()[3], properties.occlusionTexture);
  EXPECT_EQ(properties.getTextures()[4], properties.emissiveTexture);
}

TEST_F(MaterialPBRTest, GetsProperties) {
  liquid::MaterialPBR::Properties properties{
      liquid::rhi::TextureHandle::Invalid,
      0,
      {1.0f, 0.2f, 0.3f, 0.4f},
      liquid::rhi::TextureHandle::Invalid,
      0,
      0.2f,
      0.6f,
      liquid::rhi::TextureHandle::Invalid,
      1,
      0.7f,
      liquid::rhi::TextureHandle::Invalid,
      0,
      0.3f,
      liquid::rhi::TextureHandle::Invalid,
      0,
      glm::vec3(1.0f, 0.2f, 0.4f)};

#define EXPECT_PROP_EQ(idx, key, type, value)                                  \
  EXPECT_EQ(properties.getProperties()[idx].first, key);                       \
  EXPECT_TRUE(properties.getProperties()[idx].second.getValue<type>() == value);

  EXPECT_PROP_EQ(0, "baseColorTexture", int, -1);
  EXPECT_PROP_EQ(1, "baseColorTextureCoord", int, 0);
  EXPECT_PROP_EQ(2, "baseColorFactor", glm::vec4,
                 glm::vec4(1.0, 0.2, 0.3, 0.4));

  EXPECT_PROP_EQ(3, "metallicRoughnessTexture", int, -1);
  EXPECT_PROP_EQ(4, "metallicRoughnessTextureCoord", int, 0);
  EXPECT_PROP_EQ(5, "metallicFactor", float, 0.2f);
  EXPECT_PROP_EQ(6, "roughnessFactor", float, 0.6f);

  EXPECT_PROP_EQ(7, "normalTexture", int, -1);
  EXPECT_PROP_EQ(8, "normalTextureCoord", int, 1);
  EXPECT_PROP_EQ(9, "normalScale", float, 0.7f);

  EXPECT_PROP_EQ(10, "occlusionTexture", int, -1);
  EXPECT_PROP_EQ(11, "occlusionTextureCoord", int, 0);
  EXPECT_PROP_EQ(12, "occlusionStrength", float, 0.3f);

  EXPECT_PROP_EQ(13, "emissiveTexture", int, -1);
  EXPECT_PROP_EQ(14, "emissiveTextureCoord", int, 0);
  EXPECT_PROP_EQ(15, "emissiveFactor", glm::vec3, glm::vec3(1.0, 0.2, 0.4));

  properties.baseColorTexture = liquid::rhi::TextureHandle(1);
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);

  properties.normalTexture = liquid::rhi::TextureHandle(2);
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);
  EXPECT_PROP_EQ(7, "normalTexture", int, 1);

  properties.occlusionTexture = liquid::rhi::TextureHandle(3);
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);
  EXPECT_PROP_EQ(7, "normalTexture", int, 1);
  EXPECT_PROP_EQ(10, "occlusionTexture", int, 2);

  properties.metallicRoughnessTexture = liquid::rhi::TextureHandle(4);
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);
  EXPECT_PROP_EQ(3, "metallicRoughnessTexture", int, 1);
  EXPECT_PROP_EQ(7, "normalTexture", int, 2);
  EXPECT_PROP_EQ(10, "occlusionTexture", int, 3);

  properties.emissiveTexture = liquid::rhi::TextureHandle(5);
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);
  EXPECT_PROP_EQ(3, "metallicRoughnessTexture", int, 1);
  EXPECT_PROP_EQ(7, "normalTexture", int, 2);
  EXPECT_PROP_EQ(10, "occlusionTexture", int, 3);
  EXPECT_PROP_EQ(13, "emissiveTexture", int, 4);

#undef EXPECT_PROP_EQ
}

TEST_F(MaterialPBRTest, SetsShadersPropertiesAndTextures) {
  liquid::MaterialPBR::Properties properties{
      liquid::rhi::TextureHandle(1),
      0,
      {1.0f, 0.2f, 0.3f, 0.4f},
      liquid::rhi::TextureHandle::Invalid,
      0,
      0.2f,
      0.6f,
      liquid::rhi::TextureHandle(2),
      1,
      0.7f,
      liquid::rhi::TextureHandle::Invalid,
      0,
      0.3f,
      liquid::rhi::TextureHandle(3),
      0,
      glm::vec3(1.0f, 0.2f, 0.4f)};

  liquid::MaterialPBR material(properties, registry);

  EXPECT_EQ(material.getTextures().size(), 3);
  EXPECT_EQ(registry.getBufferMap().getDescription(material.getBuffer()).size,
            16 * sizeof(glm::vec4));
}
