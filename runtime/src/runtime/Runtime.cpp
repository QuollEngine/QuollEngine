#include "quoll/core/Base.h"
#include "Runtime.h"

// Core systems
#include "quoll/imgui/ImguiRenderer.h"
#include "quoll/imgui/ImguiUtils.h"
#include "quoll/io/SceneIO.h"
#include "quoll/loop/MainEngineModules.h"
#include "quoll/loop/MainLoop.h"
#include "quoll/profiler/FPSCounter.h"
#include "quoll/profiler/MetricsCollector.h"
#include "quoll/renderer/Presenter.h"
#include "quoll/renderer/RenderStorage.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/renderer/SceneRenderer.h"
#include "quoll/scene/Scene.h"
#include "quoll/window/Window.h"

// Render hardware interfaces
#include "quoll/rhi-vulkan/VulkanRenderDevice.h"
#include "quoll/rhi/RenderDevice.h"

// Asset
#include "quoll/asset/AssetCache.h"

namespace quoll::runtime {

Runtime::Runtime(const LaunchConfig &config) : mConfig(config) {}

void Runtime::start() {
  static constexpr u32 Width = 800;
  static constexpr u32 Height = 600;

  Scene scene;
  InputDeviceManager deviceManager;
  Window window(mConfig.name, Width, Height, deviceManager);
  AssetCache assetCache(std::filesystem::current_path() / "assets", true);

  MainEngineModules engineModules(deviceManager, window,
                                  assetCache.getRegistry());

  rhi::VulkanRenderBackend backend(window);
  auto *device = backend.createDefaultDevice();
  MetricsCollector metricsCollector;

  RenderStorage renderStorage(device, metricsCollector);

  RendererOptions initialOptions{};
  initialOptions.framebufferSize = {Width, Height};
  Renderer renderer(renderStorage, initialOptions);
  ImguiRenderer imguiRenderer(window, renderStorage);

  {
    static constexpr f32 FontSize = 18.0f;
    auto &io = ImGui::GetIO();

    Path defaultFontPath = Engine::getFontsPath() / "Roboto-Regular.ttf";
    io.Fonts->AddFontFromFileTTF(defaultFontPath.string().c_str(), FontSize);
    imguiRenderer.buildFonts();
  }

  SceneRenderer sceneRenderer(assetCache.getRegistry(), renderStorage);

  auto res = assetCache.preloadAssets(renderStorage);

  FPSCounter fpsCounter;
  MainLoop mainLoop(window, fpsCounter);

  Presenter presenter(renderStorage);

  renderer.setGraphBuilder([&](auto &graph, const auto &options) {
    auto passData = sceneRenderer.attach(graph, options);
    sceneRenderer.attachText(graph, passData);
    auto imguiData = imguiRenderer.attach(graph, options);
    imguiData.pass.read(passData.finalColor);

    return RendererTextures{imguiData.imguiColor, passData.finalColor};
  });

  engineModules.getCameraAspectRatioUpdater().setViewportSize(
      window.getFramebufferSize());
  engineModules.getUICanvasUpdater().setViewport(
      0.0f, 0.0f, static_cast<f32>(window.getFramebufferSize().x),
      static_cast<f32>(window.getFramebufferSize().y));

  window.getSignals().onFramebufferResize().connect(
      [&](auto width, auto height) {
        renderer.setFramebufferSize({width, height});
        presenter.enqueueFramebufferUpdate();
        engineModules.getCameraAspectRatioUpdater().setViewportSize(
            {width, height});
        engineModules.getUICanvasUpdater().setViewport(
            0.0f, 0.0f, static_cast<f32>(width), static_cast<f32>(height));
      });

  auto handle = assetCache.getRegistry().getScenes().findHandleByUuid(
      mConfig.startingScene);

  QuollAssert(handle != SceneAssetHandle::Null, "Scene not found");
  if (handle == SceneAssetHandle::Null) {
    return;
  }

  SceneIO sceneIO(assetCache.getRegistry(), scene);
  sceneIO.loadScene(handle);

  auto systemView = engineModules.createSystemView(scene);

  presenter.updateFramebuffers(device->getSwapchain());

  mainLoop.setPrepareFn([&]() { engineModules.prepare(systemView); });

  mainLoop.setFixedUpdateFn(
      [&](f32 dt) { engineModules.fixedUpdate(dt, systemView); });

  mainLoop.setUpdateFn([&](f32 dt) { engineModules.update(dt, systemView); });

  mainLoop.setRenderFn([&]() {
    if (presenter.requiresFramebufferUpdate()) {
      device->recreateSwapchain();
      presenter.updateFramebuffers(device->getSwapchain());
      return;
    }

    renderer.rebuildIfSettingsChanged();

    imguiRenderer.beginRendering();

    ImGuiWindowFlags WindowFlags =
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    auto size = window.getFramebufferSize();
    ImGui::SetNextWindowPos({0.0f, 0.0f});
    ImGui::SetNextWindowSize(
        {static_cast<f32>(size.x), static_cast<f32>(size.y)});

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("SceneView", nullptr, WindowFlags)) {
      imgui::image(renderer.getSceneTexture(), ImGui::GetContentRegionAvail());
      ImGui::End();
    }

    ImGui::PopStyleVar();

    engineModules.render(systemView);

    imguiRenderer.endRendering();

    const auto &renderFrame = device->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<u32>::max()) {
      sceneRenderer.updateFrameData(scene.entityDatabase, scene.activeCamera,
                                    renderFrame.frameIndex);
      imguiRenderer.updateFrameData(renderFrame.frameIndex);

      renderer.execute(renderFrame.commandList, renderFrame.frameIndex);

      presenter.present(renderFrame.commandList, renderer.getFinalTexture(),
                        renderFrame.swapchainImageIndex);

      device->endFrame(renderFrame);

      metricsCollector.getResults(device);
    } else {
      presenter.updateFramebuffers(device->getSwapchain());
    }
  });

  mainLoop.setStatsFn([this, &metricsCollector](u32 frames) {
    metricsCollector.markForCollection();
  });

  mainLoop.run();
  device->waitForIdle();
}

} // namespace quoll::runtime
