#include "liquid/core/Base.h"
#include "liquid/rhi/Descriptor.h"

#include <gtest/gtest.h>

using TextureArray = std::vector<liquid::rhi::TextureHandle>;

class DescriptorTest : public ::testing::Test {
public:
};

using DescriptorDeathTest = DescriptorTest;

TEST_F(DescriptorTest, AddsTextureBinding) {
  liquid::rhi::Descriptor descriptor;

  liquid::rhi::TextureHandle tex1{1}, tex2{2};

  descriptor.bind(2, {tex1, tex2},
                  liquid::rhi::DescriptorType::CombinedImageSampler);
  EXPECT_EQ(descriptor.getBindings().at(2).type,
            liquid::rhi::DescriptorType::CombinedImageSampler);
  const auto &data =
      std::get<TextureArray>(descriptor.getBindings().at(2).data);
  EXPECT_EQ(data.at(0), tex1);
  EXPECT_EQ(data.at(1), tex2);
}

TEST_F(DescriptorTest, AddsUniformBufferBinding) {
  liquid::rhi::Descriptor descriptor;

  liquid::rhi::BufferHandle buffer{2};
  descriptor.bind(2, buffer, liquid::rhi::DescriptorType::UniformBuffer);

  EXPECT_EQ(descriptor.getBindings().at(2).type,
            liquid::rhi::DescriptorType::UniformBuffer);
  auto data =
      std::get<liquid::rhi::BufferHandle>(descriptor.getBindings().at(2).data);
  EXPECT_EQ(data, buffer);
}

TEST_F(DescriptorTest, CreatesHashFromBindings) {
  liquid::rhi::Descriptor descriptor;
  liquid::rhi::TextureHandle tex1{1}, tex2{2};
  liquid::rhi::BufferHandle buffer1{1}, buffer2{2};

  descriptor.bind(0, {tex1, tex2},
                  liquid::rhi::DescriptorType::CombinedImageSampler);
  descriptor.bind(1, buffer1, liquid::rhi::DescriptorType::UniformBuffer);
  descriptor.bind(2, buffer2, liquid::rhi::DescriptorType::UniformBuffer);

  std::stringstream ss;
  ss << "b:0;t:1;d:" << 1 << ";d:" << 2 << ";|b:1;t:0;d:" << 1
     << "|b:2;t:0;d:" << 2 << "|";

  EXPECT_EQ(descriptor.getHashCode(), ss.str());
}

TEST_F(DescriptorDeathTest, FailsIfBufferIsUsedForCombinedImageSampler) {
  liquid::rhi::BufferHandle buffer{1};
  liquid::rhi::Descriptor descriptor;
  EXPECT_DEATH(
      {
        descriptor.bind(0, buffer,
                        liquid::rhi::DescriptorType::CombinedImageSampler);
      },
      ".*");
}

TEST_F(DescriptorDeathTest, FailsIfTextureIsUsedForUniformBuffer) {
  liquid::rhi::TextureHandle texture{1};
  liquid::rhi::Descriptor descriptor;
  EXPECT_DEATH(
      {
        descriptor.bind(0, TextureArray{texture},
                        liquid::rhi::DescriptorType::UniformBuffer);
      },
      ".*");
}
