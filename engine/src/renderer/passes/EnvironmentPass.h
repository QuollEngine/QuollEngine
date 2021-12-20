#pragma once

#include "renderer/render-graph/RenderGraphPassBase.h"
#include "renderer/ShaderLibrary.h"
#include "entity/EntityContext.h"
#include "renderer/vulkan/VulkanRenderData.h"
#include "renderer/vulkan/VulkanDescriptorManager.h"

namespace liquid {

class EnvironmentPass : public RenderGraphPassBase {
public:
  /**
   * @brief Create environment pass
   *
   * @param name Pass name
   * @param renderPassId Render pass resource ID
   * @param entityContext Entity context
   * @param shaderLibrary Shader library
   * @param descriptorManager Descriptor manager
   * @param renderData Render data
   */
  EnvironmentPass(const String &name, GraphResourceId renderPassId,
                  EntityContext &entityContext, ShaderLibrary *shaderLibrary,
                  VulkanDescriptorManager *descriptorManager,
                  const SharedPtr<VulkanRenderData> &renderData);

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
  GraphResourceId pipelineId = 0;
  EntityContext &entityContext;
  ShaderLibrary *shaderLibrary;
  SharedPtr<VulkanRenderData> renderData;
  VkDescriptorSet sceneDescriptorSet = VK_NULL_HANDLE;
  VulkanDescriptorManager *descriptorManager;
};

} // namespace liquid
