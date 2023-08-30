#include "liquid/core/Base.h"
#include "Runtime.h"

// Core systems
#include "liquid/window/Window.h"
#include "liquid/events/EventSystem.h"
#include "liquid/profiler/FPSCounter.h"
#include "liquid/loop/MainLoop.h"
#include "liquid/renderer/Renderer.h"
#include "liquid/scripting/ScriptingSystem.h"
#include "liquid/scene/SceneUpdater.h"
#include "liquid/physics/PhysicsSystem.h"
#include "liquid/renderer/Presenter.h"
#include "liquid/scene/CameraAspectRatioUpdater.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/scene/SkeletonUpdater.h"
#include "liquid/audio/AudioSystem.h"
#include "liquid/core/EntityDeleter.h"
#include "liquid/scene/SceneIO.h"
#include "liquid/renderer/SceneRenderer.h"

// Render hardware interfaces
#include "liquid/rhi/RenderDevice.h"
#include "liquid/rhi-vulkan/VulkanRenderDevice.h"

// Asset
#include "liquid/asset/AssetCache.h"

namespace quoll::runtime {

Runtime::Runtime(const LaunchConfig &config) : mConfig(config) {}

void Runtime::start() {
  static constexpr uint32_t Width = 800;
  static constexpr uint32_t Height = 600;

  quoll::Scene scene;
  quoll::EventSystem eventSystem;
  quoll::Window window(mConfig.name, Width, Height, eventSystem);
  quoll::AssetCache assetCache(std::filesystem::current_path() / "assets",
                               true);

  quoll::rhi::VulkanRenderBackend backend(window);
  auto *device = backend.createDefaultDevice();
  quoll::RenderStorage renderStorage(device);

  quoll::RendererOptions initialOptions{};
  initialOptions.size = {Width, Height};
  quoll::Renderer renderer(renderStorage, initialOptions);

  SceneRenderer sceneRenderer(assetCache.getRegistry(), renderStorage);

  auto res = assetCache.preloadAssets(renderStorage);

  quoll::FPSCounter fpsCounter;
  quoll::MainLoop mainLoop(window, fpsCounter);

  quoll::Presenter presenter(renderStorage);

  renderer.setGraphBuilder([&](auto &graph, const auto &options) {
    auto passData = sceneRenderer.attach(graph, options);
    sceneRenderer.attachText(graph, passData);

    return RendererTextures{passData.finalColor, passData.finalColor};
  });

  quoll::ScriptingSystem scriptingSystem(eventSystem, assetCache.getRegistry());
  quoll::SceneUpdater sceneUpdater;
  quoll::PhysicsSystem physicsSystem(eventSystem);
  quoll::CameraAspectRatioUpdater cameraAspectRatioUpdater(window);
  quoll::AnimationSystem animationSystem(assetCache.getRegistry());
  quoll::SkeletonUpdater skeletonUpdater;
  quoll::AudioSystem audioSystem(assetCache.getRegistry());
  quoll::EntityDeleter entityDeleter;

  audioSystem.observeChanges(scene.entityDatabase);
  scriptingSystem.observeChanges(scene.entityDatabase);
  physicsSystem.observeChanges(scene.entityDatabase);

  window.addResizeHandler([&](auto width, auto height) {
    renderer.setFramebufferSize({width, height});
    presenter.enqueueFramebufferUpdate();
  });

  auto handle = assetCache.getRegistry().getScenes().findHandleByUuid(
      mConfig.startingScene);

  LIQUID_ASSERT(handle != SceneAssetHandle::Null, "Scene not found");
  if (handle == SceneAssetHandle::Null) {
    return;
  }

  quoll::SceneIO sceneIO(assetCache.getRegistry(), scene);
  sceneIO.loadScene(handle);

  presenter.updateFramebuffers(device->getSwapchain());

  mainLoop.setUpdateFn([&](float dt) mutable {
    eventSystem.poll();
    auto &entityDatabase = scene.entityDatabase;

    entityDeleter.update(scene);
    cameraAspectRatioUpdater.update(entityDatabase);
    scriptingSystem.start(entityDatabase);
    scriptingSystem.update(dt, entityDatabase);
    animationSystem.update(dt, entityDatabase);
    skeletonUpdater.update(entityDatabase);
    sceneUpdater.update(entityDatabase);
    physicsSystem.update(dt, entityDatabase);
    audioSystem.output(entityDatabase);

    return true;
  });

  mainLoop.setRenderFn([&]() {
    if (presenter.requiresFramebufferUpdate()) {
      device->recreateSwapchain();
      presenter.updateFramebuffers(device->getSwapchain());
      return;
    }

    renderer.rebuildIfSettingsChanged();

    const auto &renderFrame = device->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      sceneRenderer.updateFrameData(scene.entityDatabase, scene.activeCamera,
                                    renderFrame.frameIndex);

      renderer.execute(renderFrame.commandList, renderFrame.frameIndex);

      presenter.present(renderFrame.commandList, renderer.getFinalTexture(),
                        renderFrame.swapchainImageIndex);

      device->endFrame(renderFrame);
    } else {
      presenter.updateFramebuffers(device->getSwapchain());
    }
  });

  mainLoop.run();
  device->waitForIdle();
}

} // namespace quoll::runtime
