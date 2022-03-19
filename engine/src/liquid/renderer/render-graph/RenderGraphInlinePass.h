#pragma once

#include "RenderGraphPassBase.h"

namespace liquid {
template <class TScope>
class RenderGraphInlinePass : public RenderGraphPassBase {
public:
  using BuilderFn =
      std::function<void(RenderGraphBuilder &builder, TScope &scope)>;
  using ExecutorFn =
      std::function<void(rhi::RenderCommandList &commandList, TScope &scope,
                         RenderGraphRegistry &registry)>;

public:
  /**
   * @brief Create render graph pass
   *
   * @param name Pass name
   * @param renderPassId Render pass resource ID
   * @param builderFn Builder function
   * @param executorFn Executor function
   */
  RenderGraphInlinePass(const String &name, GraphResourceId renderPassId,
                        const BuilderFn &builderFn,
                        const ExecutorFn &executorFn)
      : RenderGraphPassBase(name, renderPassId), mBuilderFn(builderFn),
        mExecutorFn(executorFn) {}

  /**
   * @brief Build pass
   *
   * @param builder Graph builder
   */
  void buildInternal(RenderGraphBuilder &builder) override {
    mBuilderFn(builder, mScope);
  }

  /**
   * @brief Execute pass
   *
   * @param commandList Render command list
   * @param registry Render graph registry
   */
  void execute(rhi::RenderCommandList &commandList,
               RenderGraphRegistry &registry) override {
    mExecutorFn(commandList, mScope, registry);
  }

private:
  BuilderFn mBuilderFn;
  ExecutorFn mExecutorFn;
  TScope mScope{};
};

} // namespace liquid
