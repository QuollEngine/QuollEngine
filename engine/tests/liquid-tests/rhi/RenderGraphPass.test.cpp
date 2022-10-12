#include "liquid/core/Base.h"
#include "liquid/rhi/RenderGraphPass.h"

#include "liquid-tests/Testing.h"

class RenderGraphPassTest : public ::testing::Test {
public:
};

TEST_F(RenderGraphPassTest, SetsNameOnConstruct) {
  liquid::rhi::RenderGraphPass pass("Test");
  EXPECT_EQ(pass.getName(), "Test");
}

TEST_F(RenderGraphPassTest, AddsHandleToOutputOnWrite) {
  liquid::rhi::RenderGraphPass pass("Test");
  liquid::rhi::TextureHandle handle{2};

  pass.write(handle, glm::vec4());
  EXPECT_EQ(pass.getOutputs().size(), 1);
  EXPECT_EQ(pass.getOutputs().at(0).texture, handle);
}

TEST_F(RenderGraphPassTest, AddsClearValueToAttachmentDataOnWrite) {
  liquid::rhi::RenderGraphPass pass("Test");
  liquid::rhi::TextureHandle handle{2};

  pass.write(handle, glm::vec4(2.0f));
  EXPECT_EQ(pass.getAttachments().size(), 1);
  EXPECT_EQ(std::get<glm::vec4>(pass.getAttachments().at(0).clearValue),
            glm::vec4(2.0f));

  // Defaults
  EXPECT_EQ(pass.getAttachments().at(0).loadOp,
            liquid::rhi::AttachmentLoadOp::DontCare);
  EXPECT_EQ(pass.getAttachments().at(0).storeOp,
            liquid::rhi::AttachmentStoreOp::DontCare);
}

TEST_F(RenderGraphPassTest, AddsHandleToInputOnRead) {
  liquid::rhi::RenderGraphPass pass("Test");
  liquid::rhi::TextureHandle handle{2};

  pass.read(handle);
  EXPECT_EQ(pass.getAttachments().size(), 0);
  EXPECT_EQ(pass.getOutputs().size(), 0);
  EXPECT_EQ(pass.getInputs().size(), 1);
  EXPECT_EQ(pass.getInputs().at(0).texture, handle);
}
