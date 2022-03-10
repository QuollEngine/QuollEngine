#include "liquid/core/Base.h"
#include "liquid/renderer/MaterialPBR.h"

#include "../mocks/TestShader.h"
#include <gtest/gtest.h>

class MaterialPBRTest : public ::testing::Test {
public:
  liquid::experimental::ResourceRegistry registry;
};

TEST_F(MaterialPBRTest, GetsTextures) {
  liquid::MaterialPBR::Properties properties;
  EXPECT_EQ(properties.getTextures().size(), 0);

  properties.baseColorTexture = 1;
  EXPECT_EQ(properties.getTextures().size(), 1);
  EXPECT_EQ(properties.getTextures()[0], 1);

  properties.metallicRoughnessTexture = 2;
  EXPECT_EQ(properties.getTextures().size(), 2);
  EXPECT_EQ(properties.getTextures()[0], 1);
  EXPECT_EQ(properties.getTextures()[1], 2);

  properties.normalTexture = 3;
  EXPECT_EQ(properties.getTextures().size(), 3);
  EXPECT_EQ(properties.getTextures()[0], 1);
  EXPECT_EQ(properties.getTextures()[1], 2);
  EXPECT_EQ(properties.getTextures()[2], 3);

  properties.occlusionTexture = 4;
  EXPECT_EQ(properties.getTextures().size(), 4);
  EXPECT_EQ(properties.getTextures()[0], 1);
  EXPECT_EQ(properties.getTextures()[1], 2);
  EXPECT_EQ(properties.getTextures()[2], 3);
  EXPECT_EQ(properties.getTextures()[3], 4);

  properties.emissiveTexture = 5;
  EXPECT_EQ(properties.getTextures().size(), 5);
  EXPECT_EQ(properties.getTextures()[0], 1);
  EXPECT_EQ(properties.getTextures()[1], 2);
  EXPECT_EQ(properties.getTextures()[2], 3);
  EXPECT_EQ(properties.getTextures()[3], 4);
  EXPECT_EQ(properties.getTextures()[4], 5);
}

TEST_F(MaterialPBRTest, GetsProperties) {
  liquid::MaterialPBR::Properties properties{0,
                                             0,
                                             {1.0f, 0.2f, 0.3f, 0.4f},
                                             0,
                                             0,
                                             0.2f,
                                             0.6f,
                                             0,
                                             1,
                                             0.7f,
                                             0,
                                             0,
                                             0.3f,
                                             0,
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

  properties.baseColorTexture = 1;
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);

  properties.normalTexture = 2;
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);
  EXPECT_PROP_EQ(7, "normalTexture", int, 1);

  properties.occlusionTexture = 3;
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);
  EXPECT_PROP_EQ(7, "normalTexture", int, 1);
  EXPECT_PROP_EQ(10, "occlusionTexture", int, 2);

  properties.metallicRoughnessTexture = 4;
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);
  EXPECT_PROP_EQ(3, "metallicRoughnessTexture", int, 1);
  EXPECT_PROP_EQ(7, "normalTexture", int, 2);
  EXPECT_PROP_EQ(10, "occlusionTexture", int, 3);

  properties.emissiveTexture = 5;
  EXPECT_PROP_EQ(0, "baseColorTexture", int, 0);
  EXPECT_PROP_EQ(3, "metallicRoughnessTexture", int, 1);
  EXPECT_PROP_EQ(7, "normalTexture", int, 2);
  EXPECT_PROP_EQ(10, "occlusionTexture", int, 3);
  EXPECT_PROP_EQ(13, "emissiveTexture", int, 4);

#undef EXPECT_PROP_EQ
}

TEST_F(MaterialPBRTest, SetsShadersPropertiesAndTextures) {
  liquid::MaterialPBR::Properties properties{1,
                                             0,
                                             {1.0f, 0.2f, 0.3f, 0.4f},
                                             0,
                                             0,
                                             0.2f,
                                             0.6f,
                                             2,
                                             1,
                                             0.7f,
                                             0,
                                             0,
                                             0.3f,
                                             3,
                                             0,
                                             glm::vec3(1.0f, 0.2f, 0.4f)};

  liquid::MaterialPBR material(properties, registry);

  EXPECT_EQ(material.getTextures().size(), 3);
  EXPECT_EQ(
      registry.getBufferMap().getDescription(material.getUniformBuffer()).size,
      16 * sizeof(glm::vec4));
}
