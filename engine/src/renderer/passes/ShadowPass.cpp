#include "core/Base.h"
#include "ShadowPass.h"

namespace liquid {

constexpr uint32_t NUM_LIGHTS = 16;
constexpr uint32_t SHADOWMAP_DIMENSIONS = 2048;

ShadowPass::ShadowPass(const String &name, GraphResourceId renderPass,
                       EntityContext &entityContext,
                       ShaderLibrary *shaderLibrary_,
                       const std::vector<SharedPtr<Material>> &shadowMaterials_)
    : RenderGraphPassBase(name, renderPass), shaderLibrary(shaderLibrary_),
      sceneRenderer(entityContext, false), shadowMaterials(shadowMaterials_) {}

void ShadowPass::buildInternal(RenderGraphBuilder &builder) {
  shadowMapId = builder.write("shadowmap",
                              RenderPassAttachment{AttachmentType::Depth,
                                                   TextureFramebufferData{
                                                       SHADOWMAP_DIMENSIONS,
                                                       SHADOWMAP_DIMENSIONS,
                                                       VK_FORMAT_D16_UNORM,
                                                       NUM_LIGHTS,
                                                   },
                                                   AttachmentLoadOp::Clear,
                                                   AttachmentStoreOp::Store,
                                                   DepthStencilClear{1.0f, 0}});

  pipelineId = builder.create(PipelineDescriptor{
      shaderLibrary->getShader("__engine.default.shadowmap.vertex"),
      shaderLibrary->getShader("__engine.default.shadowmap.fragment"),
      PipelineVertexInputLayout::create<Vertex>(),
      PipelineInputAssembly{PrimitiveTopology::TriangleList},
      PipelineRasterizer{PolygonMode::Fill, CullMode::Front,
                         FrontFace::Clockwise}});
}

void ShadowPass::execute(RenderCommandList &commandList,
                         RenderGraphRegistry &registry) {
  const auto &pipeline = registry.getPipeline(pipelineId);

  commandList.bindPipeline(pipeline);

  for (auto &shadowMaterial : shadowMaterials) {
    commandList.bindDescriptor(pipeline, 0, shadowMaterial->getDescriptor());

    sceneRenderer.render(commandList, pipeline);
  }
}

} // namespace liquid
