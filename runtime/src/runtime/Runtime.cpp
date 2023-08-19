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

namespace liquid::runtime {

Runtime::Runtime(const LaunchConfig &config) : mConfig(config) {}

void Runtime::start() {
  static constexpr uint32_t Width = 800;
  static constexpr uint32_t Height = 600;

  liquid::Scene scene;
  liquid::EventSystem eventSystem;
  liquid::Window window(mConfig.name, Width, Height, eventSystem);
  liquid::AssetCache assetCache(mConfig.assetsPath, true);

  liquid::rhi::VulkanRenderBackend backend(window);
  auto *device = backend.createDefaultDevice();
  liquid::RenderStorage renderStorage(device);

  liquid::RendererOptions initialOptions{};
  initialOptions.size = {Width, Height};
  liquid::Renderer renderer(renderStorage, initialOptions);

  SceneRenderer sceneRenderer(assetCache.getRegistry(), renderStorage);

  auto res = assetCache.preloadAssets(renderStorage);

  liquid::FPSCounter fpsCounter;
  liquid::MainLoop mainLoop(window, fpsCounter);

  liquid::Presenter presenter(renderStorage);

  renderer.setGraphBuilder([&](auto &graph, const auto &options) {
    auto passData = sceneRenderer.attach(graph, options);
    sceneRenderer.attachText(graph, passData);

    return RendererTextures{passData.finalColor, passData.finalColor};
  });

  liquid::ScriptingSystem scriptingSystem(eventSystem,
                                          assetCache.getRegistry());
  liquid::SceneUpdater sceneUpdater;
  liquid::PhysicsSystem physicsSystem(eventSystem);
  liquid::CameraAspectRatioUpdater cameraAspectRatioUpdater(window);
  liquid::AnimationSystem animationSystem(assetCache.getRegistry());
  liquid::SkeletonUpdater skeletonUpdater;
  liquid::AudioSystem audioSystem(assetCache.getRegistry());
  liquid::EntityDeleter entityDeleter;

  audioSystem.observeChanges(scene.entityDatabase);
  scriptingSystem.observeChanges(scene.entityDatabase);
  physicsSystem.observeChanges(scene.entityDatabase);

  window.addResizeHandler([&](auto width, auto height) {
    renderer.setFramebufferSize({width, height});
    presenter.enqueueFramebufferUpdate();
  });

  liquid::SceneIO sceneIO(assetCache.getRegistry(), scene);
  sceneIO.loadScene(mConfig.scenesPath / "main.scene");

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

} // namespace liquid::runtime
