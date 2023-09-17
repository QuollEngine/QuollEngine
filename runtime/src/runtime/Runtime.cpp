#include "quoll/core/Base.h"
#include "Runtime.h"

// Core systems
#include "quoll/window/Window.h"
#include "quoll/events/EventSystem.h"
#include "quoll/profiler/FPSCounter.h"
#include "quoll/loop/MainLoop.h"
#include "quoll/renderer/Renderer.h"
#include "quoll/scripting/ScriptingSystem.h"
#include "quoll/scene/SceneUpdater.h"
#include "quoll/physics/PhysicsSystem.h"
#include "quoll/renderer/Presenter.h"
#include "quoll/scene/CameraAspectRatioUpdater.h"
#include "quoll/animation/AnimationSystem.h"
#include "quoll/scene/SkeletonUpdater.h"
#include "quoll/audio/AudioSystem.h"
#include "quoll/core/EntityDeleter.h"
#include "quoll/scene/SceneIO.h"
#include "quoll/renderer/SceneRenderer.h"

// Render hardware interfaces
#include "quoll/rhi/RenderDevice.h"
#include "quoll/rhi-vulkan/VulkanRenderDevice.h"

// Asset
#include "quoll/asset/AssetCache.h"

namespace quoll::runtime {

Runtime::Runtime(const LaunchConfig &config) : mConfig(config) {}

void Runtime::start() {
  static constexpr uint32_t Width = 800;
  static constexpr uint32_t Height = 600;

  quoll::Scene scene;
  quoll::EventSystem eventSystem;
  quoll::InputDeviceManager deviceManager;
  quoll::Window window(mConfig.name, Width, Height, deviceManager, eventSystem);
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

  QuollAssert(handle != SceneAssetHandle::Null, "Scene not found");
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
