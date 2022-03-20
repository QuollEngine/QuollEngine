#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/window/Window.h"

#include "Renderer.h"
#include "StandardPushConstants.h"

#include "liquid/renderer/render-graph/RenderGraph.h"

#include "liquid/renderer/passes/ImguiPass.h"
#include "liquid/renderer/passes/ScenePass.h"
#include "liquid/renderer/passes/ShadowPass.h"
#include "liquid/renderer/passes/EnvironmentPass.h"
#include "liquid/renderer/passes/FullscreenQuadPass.h"

#include "liquid/renderer/SceneRenderer.h"

namespace liquid {

Renderer::Renderer(EntityContext &entityContext, Window &window,
                   rhi::RenderDevice *device)
    : mEntityContext(entityContext), mGraphEvaluator(mRegistry),
      mDevice(device), mImguiRenderer(window, mRegistry) {
  loadShaders();
}

Renderer::~Renderer() {
  mEntityContext.destroyComponents<MeshComponent>();
  mEntityContext.destroyComponents<SkinnedMeshComponent>();
  mEntityContext.destroyComponents<SkeletonComponent>();

  mShadowMaterials.clear();
}

void Renderer::render(RenderGraph &graph) {
  mDevice->execute(graph, mGraphEvaluator);
}

void Renderer::loadShaders() {
  mShaderLibrary.addShader("__engine.geometry.default.vertex",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/geometry.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.geometry.skinned.vertex",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/skinnedGeometry.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.pbr.default.fragment",
      mRegistry.setShader({Engine::getAssetsPath() + "/shaders/pbr.frag.spv"}));
  mShaderLibrary.addShader("__engine.skybox.default.vertex",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/skybox.vert.spv"}));
  mShaderLibrary.addShader("__engine.skybox.default.fragment",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/skybox.frag.spv"}));
  mShaderLibrary.addShader(
      "__engine.shadowmap.default.vertex",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/shadowmap.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.shadowmap.skinned.vertex",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/skinnedShadowmap.vert.spv"}));

  mShaderLibrary.addShader(
      "__engine.shadowmap.default.fragment",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/shadowmap.frag.spv"}));
  mShaderLibrary.addShader("__engine.imgui.default.vertex",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/imgui.vert.spv"}));
  mShaderLibrary.addShader("__engine.imgui.default.fragment",
                           mRegistry.setShader({Engine::getAssetsPath() +
                                                "/shaders/imgui.frag.spv"}));
  mShaderLibrary.addShader(
      "__engine.fullscreenQuad.default.vertex",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/fullscreenQuad.vert.spv"}));
  mShaderLibrary.addShader(
      "__engine.fullscreenQuad.default.fragment",
      mRegistry.setShader(
          {Engine::getAssetsPath() + "/shaders/fullscreenQuad.frag.spv"}));
}

RenderGraph Renderer::createRenderGraph(const SharedPtr<RenderData> &renderData,
                                        const String &imguiDep) {
  RenderGraph graph;
  constexpr uint32_t NUM_LIGHTS = 16;
  constexpr uint32_t SHADOWMAP_DIMENSIONS = 2048;
  constexpr glm::vec4 BLUEISH_CLEAR_VALUE{0.19f, 0.21f, 0.26f, 1.0f};
  constexpr uint32_t SWAPCHAIN_SIZE_PERCENTAGE = 100;

  graph.setSwapchainColor(BLUEISH_CLEAR_VALUE);

  rhi::TextureDescription shadowMapDesc{};
  shadowMapDesc.sizeMethod = rhi::TextureSizeMethod::Fixed;
  shadowMapDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  shadowMapDesc.width = SHADOWMAP_DIMENSIONS;
  shadowMapDesc.height = SHADOWMAP_DIMENSIONS;
  shadowMapDesc.layers = NUM_LIGHTS;
  shadowMapDesc.format = VK_FORMAT_D16_UNORM;
  auto shadowmap = mRegistry.setTexture(shadowMapDesc);

  rhi::TextureDescription mainColorDesc{};
  mainColorDesc.sizeMethod = rhi::TextureSizeMethod::SwapchainRatio;
  mainColorDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Sampled;
  mainColorDesc.width = SWAPCHAIN_SIZE_PERCENTAGE;
  mainColorDesc.height = SWAPCHAIN_SIZE_PERCENTAGE;
  mainColorDesc.layers = 1;
  mainColorDesc.format = VK_FORMAT_B8G8R8A8_SRGB;
  auto mainColor = mRegistry.setTexture(mainColorDesc);

  rhi::TextureDescription depthBufferDesc{};
  depthBufferDesc.sizeMethod = rhi::TextureSizeMethod::SwapchainRatio;
  depthBufferDesc.usage = rhi::TextureUsage::Depth | rhi::TextureUsage::Sampled;
  depthBufferDesc.width = SWAPCHAIN_SIZE_PERCENTAGE;
  depthBufferDesc.height = SWAPCHAIN_SIZE_PERCENTAGE;
  depthBufferDesc.layers = 1;
  depthBufferDesc.format = VK_FORMAT_D32_SFLOAT;
  auto depthBuffer = mRegistry.setTexture(depthBufferDesc);

  graph.import("shadowmap", shadowmap, DepthStencilClear{1.0f, 0});
  graph.import("mainColor", mainColor, BLUEISH_CLEAR_VALUE);
  graph.import("depthBuffer", depthBuffer, DepthStencilClear{1.0f, 0});

  graph.addPass<ShadowPass>("shadowPass", mEntityContext, mShaderLibrary,
                            mShadowMaterials);
  graph.addPass<ScenePass>("mainPass", mEntityContext, mShaderLibrary,
                           renderData, mDebugManager);
  graph.addPass<EnvironmentPass>("environmentPass", mEntityContext,
                                 mShaderLibrary, renderData);
  graph.addPass<ImguiPass>("imgui", mImguiRenderer, mShaderLibrary, imguiDep);

  return graph;
}

SharedPtr<Material> Renderer::createMaterial(
    const std::vector<rhi::TextureHandle> &textures,
    const std::vector<std::pair<String, Property>> &properties,
    const CullMode &cullMode) {
  return std::make_shared<Material>(textures, properties, mRegistry);
}

SharedPtr<Material>
Renderer::createMaterialPBR(const MaterialPBR::Properties &properties,
                            const CullMode &cullMode) {
  return std::make_shared<MaterialPBR>(properties, mRegistry);
}

SharedPtr<RenderData> Renderer::prepareScene(Scene *scene) {
  mShadowMaterials.reserve(
      mEntityContext.getEntityCountForComponent<LightComponent>());

  size_t i = 0;
  mEntityContext.iterateEntities<LightComponent>(
      [&i, this](Entity entity, const LightComponent &light) {
        mShadowMaterials.push_back(SharedPtr<Material>(
            new Material({},
                         {{"lightMatrix", glm::mat4{1.0f}},
                          {"lightIndex", static_cast<int>(i)}},
                         mRegistry)));

        i++;
      });

  return std::make_shared<RenderData>(mEntityContext, scene, mShadowMaterials,
                                      mRegistry);
}

} // namespace liquid
