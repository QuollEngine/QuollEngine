#include "liquid/core/Base.h"
#include "liquid/renderer/render-graph/RenderGraphPassBase.h"
#include "liquid/renderer/render-graph/RenderGraph.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

class RenderGraphPassMock : public liquid::RenderGraphPassBase {
public:
  RenderGraphPassMock(const liquid::String &name,
                      liquid::GraphResourceId renderPassId)
      : RenderGraphPassBase(name, renderPassId) {}

  MOCK_METHOD(void, buildInternal, (liquid::RenderGraphBuilder &), (override));

  MOCK_METHOD(void, execute,
              (liquid::RenderCommandList &, liquid::RenderGraphRegistry &),
              (override));
};

class RenderGraphPassBaseTest : public ::testing::Test {
public:
  liquid::RenderGraph graph;
};

TEST_F(RenderGraphPassBaseTest, SetsDirtyFlagToFalseIfBuildIsCalled) {
  RenderGraphPassMock pass("Test", 1);
  EXPECT_CALL(pass, buildInternal).Times(1);
  EXPECT_TRUE(pass.isDirty());
  pass.build(liquid::RenderGraphBuilder(graph, &pass));
  EXPECT_FALSE(pass.isDirty());
}
TEST_F(RenderGraphPassBaseTest, CallsBuildInternalIfDirty) {
  RenderGraphPassMock pass("Test", 1);

  EXPECT_CALL(pass, buildInternal).Times(1);

  pass.build(liquid::RenderGraphBuilder(graph, &pass));
}

TEST_F(RenderGraphPassBaseTest, DoesNotCallBuildInternalIfNotDirty) {
  RenderGraphPassMock pass("Test", 1);

  EXPECT_CALL(pass, buildInternal).Times(1);

  pass.build(liquid::RenderGraphBuilder(graph, &pass));
  pass.build(liquid::RenderGraphBuilder(graph, &pass));
}
