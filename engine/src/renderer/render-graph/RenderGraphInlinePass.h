#pragma once

#include "RenderGraphPassBase.h"

namespace liquid {
template <class TScope>
class RenderGraphInlinePass : public RenderGraphPassBase {
public:
  using BuilderFn =
      std::function<void(RenderGraphBuilder &builder, TScope &scope)>;
  using ExecutorFn =
      std::function<void(RenderCommandList &commandList, TScope &scope,
                         RenderGraphRegistry &registry)>;

public:
  /**
   * @brief Create render graph pass
   *
   * @param name Pass name
   * @param renderPassId Render pass resource ID
   * @param builderFn_ Builder function
   * @param executorFn_ Executor function
   */
  RenderGraphInlinePass(const String &name, GraphResourceId renderPassId,
                        const BuilderFn &builderFn_,
                        const ExecutorFn &executorFn_)
      : RenderGraphPassBase(name, renderPassId), builderFn(builderFn_),
        executorFn(executorFn_) {}

  /**
   * @brief Build pass
   *
   * @param builder Graph builder
   */
  void buildInternal(RenderGraphBuilder &builder) override {
    builderFn(builder, scope);
  }

  /**
   * @brief Execute pass
   *
   * @param commandList Render command list
   * @param registry Render graph registry
   */
  void execute(RenderCommandList &commandList,
               RenderGraphRegistry &registry) override {
    executorFn(commandList, scope, registry);
  }

private:
  BuilderFn builderFn;
  ExecutorFn executorFn;
  TScope scope{};
};

} // namespace liquid
