#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/window/glfw/GLFWWindow.h"

#include "VulkanRenderer.h"
#include "VulkanStandardPushConstants.h"
#include "VulkanError.h"

#include "liquid/renderer/render-graph/RenderGraph.h"
#include "VulkanGraphEvaluator.h"

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
    : entityContext(entityContext_), abstraction(window, device),
      debugManager(new DebugManager) {
  loadShaders();
}

VulkanRenderer::~VulkanRenderer() {
  entityContext.destroyComponents<MeshComponent>();
  entityContext.destroyComponents<SkinnedMeshComponent>();
  entityContext.destroyComponents<SkeletonComponent>();

  shadowMaterials.clear();

  if (shaderLibrary) {
    delete shaderLibrary;
    LOG_DEBUG("[Vulkan] Shader library destroyed");
  }
}

void VulkanRenderer::loadShaders() {
  shaderLibrary->addShader(
      "__engine.geometry.default.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/geometry.vert.spv"));
  shaderLibrary->addShader("__engine.geometry.skinned.vertex",
                           createShader(Engine::getAssetsPath() +
                                        "/shaders/skinnedGeometry.vert.spv"));
  shaderLibrary->addShader(
      "__engine.pbr.default.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/pbr.frag.spv"));
  shaderLibrary->addShader(
      "__engine.skybox.default.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/skybox.vert.spv"));
  shaderLibrary->addShader(
      "__engine.skybox.default.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/skybox.frag.spv"));
  shaderLibrary->addShader(
      "__engine.shadowmap.default.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/shadowmap.vert.spv"));
  shaderLibrary->addShader("__engine.shadowmap.skinned.vertex",
                           createShader(Engine::getAssetsPath() +
                                        "/shaders/skinnedShadowmap.vert.spv"));

  shaderLibrary->addShader(
      "__engine.shadowmap.default.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/shadowmap.frag.spv"));
  shaderLibrary->addShader(
      "__engine.imgui.default.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/imgui.vert.spv"));
  shaderLibrary->addShader(
      "__engine.imgui.default.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/imgui.frag.spv"));
  shaderLibrary->addShader("__engine.fullscreenQuad.default.vertex",
                           createShader(Engine::getAssetsPath() +
                                        "/shaders/fullscreenQuad.vert.spv"));
  shaderLibrary->addShader("__engine.fullscreenQuad.default.fragment",
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

  graph.addPass<ShadowPass>("shadowPass", entityContext, shaderLibrary,
                            shadowMaterials);
  graph.addPass<ScenePass>("mainPass", entityContext, shaderLibrary, renderData,
                           debugManager);
  graph.addPass<EnvironmentPass>("environmentPass", entityContext,
                                 shaderLibrary, renderData);
  graph.addPass<ImguiPass>("imgui", abstraction, shaderLibrary, debugManager,
                           imguiDep, imUpdate);

  return graph;
}

SharedPtr<VulkanShader> VulkanRenderer::createShader(const String &shaderFile) {
  return std::make_shared<VulkanShader>(
      abstraction.getDevice()->getVulkanDevice(), shaderFile);
}

SharedPtr<Material> VulkanRenderer::createMaterial(
    const SharedPtr<Shader> &vertexShader,
    const SharedPtr<Shader> &fragmentShader,
    const std::vector<TextureHandle> &textures,
    const std::vector<std::pair<String, Property>> &properties,
    const CullMode &cullMode) {
  return std::make_shared<Material>(textures, properties,
                                    abstraction.getRegistry());
}

SharedPtr<Material>
VulkanRenderer::createMaterialPBR(const MaterialPBR::Properties &properties,
                                  const CullMode &cullMode) {
  return std::make_shared<MaterialPBR>(properties, abstraction.getRegistry());
}

SharedPtr<VulkanRenderData> VulkanRenderer::prepareScene(Scene *scene) {
  shadowMaterials.reserve(
      entityContext.getEntityCountForComponent<LightComponent>());

  size_t i = 0;
  entityContext.iterateEntities<LightComponent>(
      [&i, this](Entity entity, const LightComponent &light) {
        shadowMaterials.push_back(SharedPtr<Material>(
            new Material({},
                         {{"lightMatrix", glm::mat4{1.0f}},
                          {"lightIndex", static_cast<int>(i)}},
                         abstraction.getRegistry())));

        i++;
      });

  return std::make_shared<VulkanRenderData>(
      entityContext, scene, shadowMaterials, abstraction.getRegistry());
}

} // namespace liquid
