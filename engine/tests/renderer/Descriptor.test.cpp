#include "liquid/core/Base.h"
#include "liquid/renderer/Descriptor.h"

#include <gtest/gtest.h>

using TextureArray = std::vector<liquid::TextureHandle>;

class DescriptorTest : public ::testing::Test {
public:
};

using DescriptorDeathTest = DescriptorTest;

TEST_F(DescriptorTest, AddsTextureBinding) {
  liquid::Descriptor descriptor;

  liquid::TextureHandle tex1 = 1, tex2 = 2;

  descriptor.bind(2, {tex1, tex2},
                  liquid::DescriptorType::CombinedImageSampler);
  EXPECT_EQ(descriptor.getBindings().at(2).type,
            liquid::DescriptorType::CombinedImageSampler);
  const auto &data =
      std::get<TextureArray>(descriptor.getBindings().at(2).data);
  EXPECT_EQ(data.at(0), tex1);
  EXPECT_EQ(data.at(1), tex2);
}

TEST_F(DescriptorTest, AddsUniformBufferBinding) {
  liquid::Descriptor descriptor;
  descriptor.bind(2, 2, liquid::DescriptorType::UniformBuffer);

  EXPECT_EQ(descriptor.getBindings().at(2).type,
            liquid::DescriptorType::UniformBuffer);
  auto data =
      std::get<liquid::BufferHandle>(descriptor.getBindings().at(2).data);
  EXPECT_EQ(data, 2);
}

TEST_F(DescriptorTest, CreatesHashFromBindings) {
  liquid::Descriptor descriptor;
  liquid::TextureHandle tex1 = 1, tex2 = 2;
  liquid::BufferHandle buffer1 = 1, buffer2 = 2;

  descriptor.bind(0, {tex1, tex2},
                  liquid::DescriptorType::CombinedImageSampler);
  descriptor.bind(1, buffer1, liquid::DescriptorType::UniformBuffer);
  descriptor.bind(2, buffer2, liquid::DescriptorType::UniformBuffer);

  std::stringstream ss;
  ss << "b:0;t:1;d:" << tex1 << ";d:" << tex2 << ";|b:1;t:0;d:" << buffer1
     << "|b:2;t:0;d:" << buffer2 << "|";

  EXPECT_EQ(descriptor.getHashCode(), ss.str());
}

TEST_F(DescriptorDeathTest, FailsIfBufferIsUsedForCombinedImageSampler) {
  liquid::BufferHandle buffer = 1;
  liquid::Descriptor descriptor;
  EXPECT_DEATH(
      {
        descriptor.bind(0, buffer,
                        liquid::DescriptorType::CombinedImageSampler);
      },
      ".*");
}

TEST_F(DescriptorDeathTest, FailsIfTextureIsUsedForUniformBuffer) {
  liquid::TextureHandle texture = 1;
  liquid::Descriptor descriptor;
  EXPECT_DEATH(
      {
        descriptor.bind(0, TextureArray{texture},
                        liquid::DescriptorType::UniformBuffer);
      },
      ".*");
}
