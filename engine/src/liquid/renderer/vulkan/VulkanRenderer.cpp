#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/window/glfw/GLFWWindow.h"

#include "VulkanRenderer.h"
#include "VulkanStandardPushConstants.h"

#include "liquid/renderer/render-graph/RenderGraph.h"

#include "liquid/renderer/passes/ImguiPass.h"
#include "liquid/renderer/passes/ScenePass.h"
#include "liquid/renderer/passes/ShadowPass.h"
#include "liquid/renderer/passes/EnvironmentPass.h"
#include "liquid/renderer/passes/FullscreenQuadPass.h"

#include "liquid/renderer/SceneRenderer.h"

namespace liquid {

VulkanRenderer::VulkanRenderer(EntityContext &entityContext_,
                               GLFWWindow *window,
                               experimental::VulkanRenderDevice *device)
    : mEntityContext(entityContext_), mGraphEvaluator(mRegistry),
      mDevice(device), mImguiRenderer(window, mRegistry) {
  loadShaders();
}

VulkanRenderer::~VulkanRenderer() {
  mEntityContext.destroyComponents<MeshComponent>();
  mEntityContext.destroyComponents<SkinnedMeshComponent>();
  mEntityContext.destroyComponents<SkeletonComponent>();

  mShadowMaterials.clear();
}

void VulkanRenderer::render(RenderGraph &graph) {
  mDevice->execute(graph, mGraphEvaluator);
}

void VulkanRenderer::loadShaders() {
  mShaderLibrary.addShader(
      "__engine.geometry.default.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/geometry.vert.spv"));
  mShaderLibrary.addShader("__engine.geometry.skinned.vertex",
                           createShader(Engine::getAssetsPath() +
                                        "/shaders/skinnedGeometry.vert.spv"));
  mShaderLibrary.addShader(
      "__engine.pbr.default.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/pbr.frag.spv"));
  mShaderLibrary.addShader(
      "__engine.skybox.default.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/skybox.vert.spv"));
  mShaderLibrary.addShader(
      "__engine.skybox.default.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/skybox.frag.spv"));
  mShaderLibrary.addShader(
      "__engine.shadowmap.default.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/shadowmap.vert.spv"));
  mShaderLibrary.addShader("__engine.shadowmap.skinned.vertex",
                           createShader(Engine::getAssetsPath() +
                                        "/shaders/skinnedShadowmap.vert.spv"));

  mShaderLibrary.addShader(
      "__engine.shadowmap.default.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/shadowmap.frag.spv"));
  mShaderLibrary.addShader(
      "__engine.imgui.default.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/imgui.vert.spv"));
  mShaderLibrary.addShader(
      "__engine.imgui.default.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/imgui.frag.spv"));
  mShaderLibrary.addShader("__engine.fullscreenQuad.default.vertex",
                           createShader(Engine::getAssetsPath() +
                                        "/shaders/fullscreenQuad.vert.spv"));
  mShaderLibrary.addShader("__engine.fullscreenQuad.default.fragment",
                           createShader(Engine::getAssetsPath() +
                                        "/shaders/fullscreenQuad.frag.spv"));
}

RenderGraph VulkanRenderer::createRenderGraph(
    const SharedPtr<VulkanRenderData> &renderData, const String &imguiDep,
    const std::function<void(TextureHandle)> &imUpdate) {
  RenderGraph graph;
  constexpr uint32_t NUM_LIGHTS = 16;
  constexpr uint32_t SHADOWMAP_DIMENSIONS = 2048;
  constexpr glm::vec4 BLUEISH_CLEAR_VALUE{0.19f, 0.21f, 0.26f, 1.0f};

  graph.setSwapchainColor(BLUEISH_CLEAR_VALUE);

  graph.create("shadowmap",
               AttachmentData{AttachmentType::Depth,
                              AttachmentSizeMethod::Fixed, SHADOWMAP_DIMENSIONS,
                              SHADOWMAP_DIMENSIONS, NUM_LIGHTS,
                              VK_FORMAT_D16_UNORM, DepthStencilClear{1.0f, 0}});

  constexpr uint32_t SWAPCHAIN_SIZE_PERCENTAGE = 100;

  graph.create("mainColor",
               AttachmentData{AttachmentType::Color,
                              AttachmentSizeMethod::SwapchainRelative,
                              SWAPCHAIN_SIZE_PERCENTAGE,
                              SWAPCHAIN_SIZE_PERCENTAGE, 1,
                              VK_FORMAT_B8G8R8A8_SRGB, BLUEISH_CLEAR_VALUE});

  graph.create(
      "depthBuffer",
      AttachmentData{AttachmentType::Depth,
                     AttachmentSizeMethod::SwapchainRelative,
                     SWAPCHAIN_SIZE_PERCENTAGE, SWAPCHAIN_SIZE_PERCENTAGE, 1,
                     VK_FORMAT_D32_SFLOAT, DepthStencilClear{1.0f, 0}});

  graph.addPass<ShadowPass>("shadowPass", mEntityContext, mShaderLibrary,
                            mShadowMaterials);
  graph.addPass<ScenePass>("mainPass", mEntityContext, mShaderLibrary,
                           renderData, mDebugManager);
  graph.addPass<EnvironmentPass>("environmentPass", mEntityContext,
                                 mShaderLibrary, renderData);
  graph.addPass<ImguiPass>("imgui", mImguiRenderer, mShaderLibrary,
                           mDevice->getPhysicalDevice().getDeviceInfo(),
                           mStatsManager, mDebugManager, imguiDep, imUpdate);

  return graph;
}

SharedPtr<VulkanShader> VulkanRenderer::createShader(const String &shaderFile) {
  return std::make_shared<VulkanShader>(mDevice->getVulkanDevice(), shaderFile);
}

SharedPtr<Material> VulkanRenderer::createMaterial(
    const SharedPtr<Shader> &vertexShader,
    const SharedPtr<Shader> &fragmentShader,
    const std::vector<TextureHandle> &textures,
    const std::vector<std::pair<String, Property>> &properties,
    const CullMode &cullMode) {
  return std::make_shared<Material>(textures, properties, mRegistry);
}

SharedPtr<Material>
VulkanRenderer::createMaterialPBR(const MaterialPBR::Properties &properties,
                                  const CullMode &cullMode) {
  return std::make_shared<MaterialPBR>(properties, mRegistry);
}

SharedPtr<VulkanRenderData> VulkanRenderer::prepareScene(Scene *scene) {
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

  return std::make_shared<VulkanRenderData>(mEntityContext, scene,
                                            mShadowMaterials, mRegistry);
}

} // namespace liquid
