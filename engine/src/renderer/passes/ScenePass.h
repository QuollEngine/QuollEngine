#pragma once

#include "../render-graph/RenderGraph.h"
#include "../ShaderLibrary.h"
#include "../SceneRenderer.h"
#include "entity/EntityContext.h"
#include "../vulkan/VulkanRenderData.h"
#include "../vulkan/VulkanDescriptorManager.h"
#include "profiler/DebugManager.h"

namespace liquid {

class ScenePass : public RenderGraphPassBase {
public:
  /**
   * @brief Create scene pass
   *
   * @param name Pass name
   * @param renderPassId Pass resource ID
   * @param entityContext Entity context
   * @param shaderLibrary Shader library
   * @param descriptorManager Descriptor manager
   * @param renderData Render data
   * @param debugManager Debug manager
   */
  ScenePass(const String &name, GraphResourceId renderPassId,
            EntityContext &entityContext, ShaderLibrary *shaderLibrary,
            VulkanDescriptorManager *descriptorManager,
            const SharedPtr<VulkanRenderData> &renderData,
            const SharedPtr<DebugManager> &debugManager);

  /**
   * @brief Build pass
   *
   * @param builder Graph builder
   */
  void buildInternal(RenderGraphBuilder &builder) override;

  /**
   * @brief Execute pass
   *
   * @param commandList Render command list
   * @param registry Render graph registry
   */
  void execute(RenderCommandList &commandList,
               RenderGraphRegistry &registry) override;

private:
  ShaderLibrary *shaderLibrary;
  SceneRenderer sceneRenderer;

  GraphResourceId pipelineId = 0;
  GraphResourceId wireframePipelineId = 0;
  GraphResourceId shadowMapTextureId = 0;
  VkDescriptorSet sceneDescriptorSet = VK_NULL_HANDLE;
  VkDescriptorSet sceneDescriptorSetFragment = VK_NULL_HANDLE;

  VulkanDescriptorManager *descriptorManager;
  SharedPtr<VulkanRenderData> renderData;
  SharedPtr<DebugManager> debugManager;
};

} // namespace liquid
