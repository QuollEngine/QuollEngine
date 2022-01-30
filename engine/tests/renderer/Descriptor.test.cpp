#include "liquid/core/Base.h"
#include "liquid/renderer/Descriptor.h"
#include "liquid/renderer/Texture.h"
#include "liquid/renderer/HardwareBuffer.h"

#include "../mocks/TestBuffer.h"

#include <gtest/gtest.h>

class DescriptorTest : public ::testing::Test {
public:
  using TextureArray = std::vector<liquid::SharedPtr<liquid::Texture>>;

public:
  liquid::StatsManager statsManager;
};

using DescriptorDeathTest = DescriptorTest;

TEST_F(DescriptorTest, AddsTextureBinding) {
  liquid::Descriptor descriptor;

  const auto &tex1 = std::make_shared<liquid::Texture>(nullptr, 250, 25, 10, 1,
                                                       0, statsManager);
  const auto &tex2 = std::make_shared<liquid::Texture>(nullptr, 350, 35, 10, 1,
                                                       0, statsManager);
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
  const auto &buffer1 = std::make_shared<TestBuffer>(
      liquid::HardwareBuffer::Uniform, 250, statsManager);
  descriptor.bind(2, buffer1, liquid::DescriptorType::UniformBuffer);

  EXPECT_EQ(descriptor.getBindings().at(2).type,
            liquid::DescriptorType::UniformBuffer);
  const auto &data = std::get<liquid::SharedPtr<liquid::HardwareBuffer>>(
      descriptor.getBindings().at(2).data);
  EXPECT_EQ(data, buffer1);
}

TEST_F(DescriptorTest, CreatesHashFromBindings) {
  liquid::Descriptor descriptor;
  const auto &tex1 = std::make_shared<liquid::Texture>(nullptr, 250, 25, 10, 1,
                                                       0, statsManager);
  const auto &tex2 = std::make_shared<liquid::Texture>(nullptr, 350, 35, 10, 1,
                                                       0, statsManager);
  const auto &buffer1 = std::make_shared<TestBuffer>(
      liquid::HardwareBuffer::Uniform, 250, statsManager);
  const auto &buffer2 = std::make_shared<TestBuffer>(
      liquid::HardwareBuffer::Uniform, 350, statsManager);
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
  const auto &buffer = std::make_shared<TestBuffer>(
      liquid::HardwareBuffer::Uniform, 250, statsManager);

  liquid::Descriptor descriptor;
  EXPECT_DEATH(
      {
        descriptor.bind(0, buffer,
                        liquid::DescriptorType::CombinedImageSampler);
      },
      ".*");
}

TEST_F(DescriptorDeathTest, FailsIfBufferIsUsedForSampler) {
  const auto &tex = std::make_shared<liquid::Texture>(nullptr, 250, 25, 10, 1,
                                                      0, statsManager);
  liquid::Descriptor descriptor;
  EXPECT_DEATH(
      { descriptor.bind(0, {tex}, liquid::DescriptorType::UniformBuffer); },
      ".*");
}
