#include "core/Base.h"
#include "core/Engine.h"
#include "core/EngineGlobals.h"
#include "window/glfw/GLFWWindow.h"

#include "VulkanValidator.h"
#include "VulkanRenderer.h"
#include "VulkanHardwareBuffer.h"
#include "VulkanStandardPushConstants.h"
#include "VulkanPipeline.h"
#include "VulkanDeferredMaterialBinder.h"
#include "VulkanError.h"

#include "renderer/render-graph/RenderGraph.h"
#include "VulkanGraphEvaluator.h"

#include "renderer/passes/ImguiPass.h"
#include "renderer/passes/ScenePass.h"
#include "renderer/passes/ShadowPass.h"

#include "renderer/SceneRenderer.h"

namespace liquid {

constexpr uint32_t NUM_LIGHTS = 16;

VulkanRenderer::VulkanRenderer(EntityContext &entityContext_,
                               GLFWWindow *window, bool enableValidations)
    : entityContext(entityContext_), renderBackend(window, enableValidations),
      debugManager(new DebugManager), shaderLibrary(new ShaderLibrary) {

  descriptorManager = new VulkanDescriptorManager(
      renderBackend.getVulkanInstance().getDevice());

  deferredResourceManager =
      new VulkanDeferredResourceManager(descriptorManager);

  loadShaders();
}

VulkanRenderer::~VulkanRenderer() {
  entityContext.destroyComponents<MeshComponent>();

  shadowMaterials.clear();

  if (shaderLibrary) {
    delete shaderLibrary;
    LOG_DEBUG("[Vulkan] Shader library destroyed");
  }

  if (deferredResourceManager) {
    delete deferredResourceManager;
    LOG_DEBUG("[Vulkan] deferred resource manager destroyed");
  }

  if (descriptorManager) {
    delete descriptorManager;
    descriptorManager = nullptr;
    LOG_DEBUG("[Vulkan] Descriptor manager destroyed");
  }
}

void VulkanRenderer::loadShaders() {
  shaderLibrary->addShader(
      "__engine.default.pbr.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/pbr.vert.spv"));
  shaderLibrary->addShader(
      "__engine.default.pbr.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/pbr.frag.spv"));
  shaderLibrary->addShader(
      "__engine.default.skybox.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/skybox.vert.spv"));
  shaderLibrary->addShader(
      "__engine.default.skybox.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/skybox.frag.spv"));
  shaderLibrary->addShader(
      "__engine.default.shadowmap.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/shadowmap.vert.spv"));
  shaderLibrary->addShader(
      "__engine.default.shadowmap.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/shadowmap.frag.spv"));
  shaderLibrary->addShader(
      "__engine.imgui.default.vertex",
      createShader(Engine::getAssetsPath() + "/shaders/imgui.vert.spv"));
  shaderLibrary->addShader(
      "__engine.imgui.default.fragment",
      createShader(Engine::getAssetsPath() + "/shaders/imgui.frag.spv"));
}

RenderGraph VulkanRenderer::createRenderGraph(
    const SharedPtr<VulkanRenderData> &renderData) {

  RenderGraph graph;

  graph.addPass<ShadowPass>("shadowPass", entityContext, shaderLibrary,
                            shadowMaterials);
  graph.addPass<ScenePass>("mainPass", entityContext, shaderLibrary,
                           descriptorManager, renderData);
  graph.addPass<ImguiPass>("imgui", renderBackend, shaderLibrary, "mainColor");

  return graph;
}

SharedPtr<VulkanShader> VulkanRenderer::createShader(const String &shaderFile) {
  return std::make_shared<VulkanShader>(
      renderBackend.getVulkanInstance().getDevice(), shaderFile);
}

SharedPtr<Material> VulkanRenderer::createMaterial(
    const SharedPtr<Shader> &vertexShader,
    const SharedPtr<Shader> &fragmentShader,
    const std::vector<SharedPtr<Texture>> &textures,
    const std::vector<std::pair<String, Property>> &properties,
    const CullMode &cullMode) {
  return std::make_shared<Material>(textures, properties,
                                    renderBackend.getResourceAllocator(),
                                    deferredResourceManager);
}

SharedPtr<Material>
VulkanRenderer::createMaterialPBR(const MaterialPBR::Properties &properties,
                                  const CullMode &cullMode) {
  return std::make_shared<MaterialPBR>(properties,
                                       renderBackend.getResourceAllocator(),
                                       deferredResourceManager);
}

SharedPtr<VulkanRenderData> VulkanRenderer::prepareScene(Scene *scene) {
  shadowMaterials.reserve(
      entityContext.getEntityCountForComponent<LightComponent>());

  size_t i = 0;
  entityContext.iterateEntities<LightComponent>(
      [&i, this](Entity entity, const LightComponent &light) {
        shadowMaterials.push_back(SharedPtr<Material>(new Material(
            {},
            {{"lightMatrix", glm::mat4{1.0f}},
             {"lightIndex", static_cast<int>(i)}},
            renderBackend.getResourceAllocator(), deferredResourceManager)));

        i++;
      });

  return std::make_shared<VulkanRenderData>(
      entityContext, scene, descriptorManager,
      renderBackend.getResourceAllocator(), nullptr, shadowMaterials);
}

} // namespace liquid
