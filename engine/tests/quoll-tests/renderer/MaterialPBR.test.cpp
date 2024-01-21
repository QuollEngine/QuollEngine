#include "quoll/core/Base.h"
#include "quoll/profiler/MetricsCollector.h"
#include "quoll/renderer/MaterialPBR.h"
#include "quoll/renderer/RenderStorage.h"
#include "quoll/rhi-mock/MockRenderDevice.h"
#include "quoll-tests/Testing.h"

class MaterialPBRTest : public ::testing::Test {
public:
  MaterialPBRTest() : renderStorage(&device, metricsCollector) {}

  quoll::rhi::MockRenderDevice device;
  quoll::MetricsCollector metricsCollector;

  quoll::RenderStorage renderStorage;
};

TEST_F(MaterialPBRTest, GetsTextures) {
  quoll::MaterialPBR::Properties properties;
  EXPECT_EQ(properties.getTextures().size(), 0);

  properties.baseColorTexture = quoll::rhi::TextureHandle(1);
  EXPECT_EQ(properties.getTextures().size(), 1);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);

  properties.metallicRoughnessTexture = quoll::rhi::TextureHandle(2);
  EXPECT_EQ(properties.getTextures().size(), 2);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);
  EXPECT_EQ(properties.getTextures()[1], properties.metallicRoughnessTexture);

  properties.normalTexture = quoll::rhi::TextureHandle(2);
  EXPECT_EQ(properties.getTextures().size(), 3);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);
  EXPECT_EQ(properties.getTextures()[1], properties.metallicRoughnessTexture);
  EXPECT_EQ(properties.getTextures()[2], properties.normalTexture);

  properties.occlusionTexture = quoll::rhi::TextureHandle(4);
  EXPECT_EQ(properties.getTextures().size(), 4);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);
  EXPECT_EQ(properties.getTextures()[1], properties.metallicRoughnessTexture);
  EXPECT_EQ(properties.getTextures()[2], properties.normalTexture);
  EXPECT_EQ(properties.getTextures()[3], properties.occlusionTexture);

  properties.emissiveTexture = quoll::rhi::TextureHandle(5);
  EXPECT_EQ(properties.getTextures().size(), 5);
  EXPECT_EQ(properties.getTextures()[0], properties.baseColorTexture);
  EXPECT_EQ(properties.getTextures()[1], properties.metallicRoughnessTexture);
  EXPECT_EQ(properties.getTextures()[2], properties.normalTexture);
  EXPECT_EQ(properties.getTextures()[3], properties.occlusionTexture);
  EXPECT_EQ(properties.getTextures()[4], properties.emissiveTexture);
}

TEST_F(MaterialPBRTest, GetsProperties) {
  quoll::MaterialPBR::Properties properties{quoll::rhi::TextureHandle::Null,
                                            0,
                                            {1.0f, 0.2f, 0.3f, 0.4f},
                                            quoll::rhi::TextureHandle::Null,
                                            0,
                                            0.2f,
                                            0.6f,
                                            quoll::rhi::TextureHandle::Null,
                                            1,
                                            0.7f,
                                            quoll::rhi::TextureHandle::Null,
                                            0,
                                            0.3f,
                                            quoll::rhi::TextureHandle::Null,
                                            0,
                                            glm::vec3(1.0f, 0.2f, 0.4f)};

#define EXPECT_PROP_EQ(idx, key, type, value)                                  \
  EXPECT_EQ(properties.getProperties()[idx].first, key);                       \
  EXPECT_TRUE(properties.getProperties()[idx].second.getValue<type>() == value);

  EXPECT_PROP_EQ(0, "baseColorTexture", u32, 0);
  EXPECT_PROP_EQ(1, "baseColorTextureCoord", int, 0);
  EXPECT_PROP_EQ(2, "baseColorFactor", glm::vec4,
                 glm::vec4(1.0, 0.2, 0.3, 0.4));

  EXPECT_PROP_EQ(3, "metallicRoughnessTexture", u32, 0);
  EXPECT_PROP_EQ(4, "metallicRoughnessTextureCoord", int, 0);
  EXPECT_PROP_EQ(5, "metallicFactor", f32, 0.2f);
  EXPECT_PROP_EQ(6, "roughnessFactor", f32, 0.6f);

  EXPECT_PROP_EQ(7, "normalTexture", u32, 0);
  EXPECT_PROP_EQ(8, "normalTextureCoord", int, 1);
  EXPECT_PROP_EQ(9, "normalScale", f32, 0.7f);

  EXPECT_PROP_EQ(10, "occlusionTexture", u32, 0);
  EXPECT_PROP_EQ(11, "occlusionTextureCoord", int, 0);
  EXPECT_PROP_EQ(12, "occlusionStrength", f32, 0.3f);

  EXPECT_PROP_EQ(13, "emissiveTexture", u32, 0);
  EXPECT_PROP_EQ(14, "emissiveTextureCoord", int, 0);
  EXPECT_PROP_EQ(15, "emissiveFactor", glm::vec3, glm::vec3(1.0, 0.2, 0.4));

  properties.baseColorTexture = quoll::rhi::TextureHandle(1);
  EXPECT_PROP_EQ(0, "baseColorTexture", u32, 1);

  properties.normalTexture = quoll::rhi::TextureHandle(2);
  EXPECT_PROP_EQ(0, "baseColorTexture", u32, 1);
  EXPECT_PROP_EQ(7, "normalTexture", u32, 2);

  properties.occlusionTexture = quoll::rhi::TextureHandle(3);
  EXPECT_PROP_EQ(0, "baseColorTexture", u32, 1);
  EXPECT_PROP_EQ(7, "normalTexture", u32, 2);
  EXPECT_PROP_EQ(10, "occlusionTexture", u32, 3);

  properties.metallicRoughnessTexture = quoll::rhi::TextureHandle(4);
  EXPECT_PROP_EQ(0, "baseColorTexture", u32, 1);
  EXPECT_PROP_EQ(3, "metallicRoughnessTexture", u32, 4);
  EXPECT_PROP_EQ(7, "normalTexture", u32, 2);
  EXPECT_PROP_EQ(10, "occlusionTexture", u32, 3);

  properties.emissiveTexture = quoll::rhi::TextureHandle(5);
  EXPECT_PROP_EQ(0, "baseColorTexture", u32, 1);
  EXPECT_PROP_EQ(3, "metallicRoughnessTexture", u32, 4);
  EXPECT_PROP_EQ(7, "normalTexture", u32, 2);
  EXPECT_PROP_EQ(10, "occlusionTexture", u32, 3);
  EXPECT_PROP_EQ(13, "emissiveTexture", u32, 5);

#undef EXPECT_PROP_EQ
}

TEST_F(MaterialPBRTest, SetsShadersPropertiesAndTextures) {
  quoll::MaterialPBR::Properties properties{quoll::rhi::TextureHandle(1),
                                            0,
                                            {1.0f, 0.2f, 0.3f, 0.4f},
                                            quoll::rhi::TextureHandle::Null,
                                            0,
                                            0.2f,
                                            0.6f,
                                            quoll::rhi::TextureHandle(2),
                                            1,
                                            0.7f,
                                            quoll::rhi::TextureHandle::Null,
                                            0,
                                            0.3f,
                                            quoll::rhi::TextureHandle(3),
                                            0,
                                            glm::vec3(1.0f, 0.2f, 0.4f)};

  quoll::MaterialPBR material("test", properties, renderStorage);

  auto *buffer = device.getBuffer(material.getBuffer());

  EXPECT_EQ(material.getTextures().size(), 3);
  EXPECT_EQ(buffer->getDescription().size, 16 * sizeof(glm::vec4));
}
