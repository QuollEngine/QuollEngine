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
  auto res = assetCache.preloadAssets(device);

  liquid::FPSCounter fpsCounter;
  liquid::MainLoop mainLoop(window, fpsCounter);

  liquid::RenderGraph graph("Main");
  liquid::Renderer renderer(assetCache.getRegistry(), window, device);

  static constexpr glm::vec4 BlueishClearValue{0.52f, 0.54f, 0.89f, 1.0f};

  renderer.getSceneRenderer().setClearColor(BlueishClearValue);

  auto passData = renderer.getSceneRenderer().attach(graph);
  renderer.getSceneRenderer().attachText(graph, passData);

  liquid::ScriptingSystem scriptingSystem(eventSystem,
                                          assetCache.getRegistry());
  liquid::SceneUpdater sceneUpdater;
  liquid::PhysicsSystem physicsSystem(eventSystem);
  liquid::Presenter presenter(renderer.getShaderLibrary(), device);
  liquid::CameraAspectRatioUpdater cameraAspectRatioUpdater(window);
  liquid::AnimationSystem animationSystem(assetCache.getRegistry());
  liquid::SkeletonUpdater skeletonUpdater;
  liquid::AudioSystem audioSystem(assetCache.getRegistry());
  liquid::EntityDeleter entityDeleter;

  graph.setFramebufferExtent(window.getFramebufferSize());
  window.addResizeHandler([&graph](auto width, auto height) {
    graph.setFramebufferExtent({width, height});
  });

  liquid::SceneIO sceneIO(assetCache.getRegistry(), scene);
  sceneIO.loadScene(mConfig.scenesPath / "main.lqscene");

  presenter.updateFramebuffers(device->getSwapchain());

  mainLoop.setUpdateFn([&](float dt) mutable {
    eventSystem.poll();
    auto &entityDatabase = scene.entityDatabase;

    cameraAspectRatioUpdater.update(entityDatabase);
    scriptingSystem.start(entityDatabase);
    scriptingSystem.update(dt, entityDatabase);
    animationSystem.update(dt, entityDatabase);
    skeletonUpdater.update(entityDatabase);
    sceneUpdater.update(entityDatabase);
    physicsSystem.update(dt, entityDatabase);
    audioSystem.output(entityDatabase);
    entityDeleter.update(scene);

    return true;
  });

  mainLoop.setRenderFn([&]() {
    const auto &renderFrame = device->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      renderer.getSceneRenderer().updateFrameData(
          scene.entityDatabase, scene.activeCamera, renderFrame.frameIndex);

      renderer.render(graph, renderFrame.commandList, renderFrame.frameIndex);

      presenter.present(renderFrame.commandList, passData.sceneColor,
                        renderFrame.swapchainImageIndex);

      renderer.getRenderDevice()->endFrame(renderFrame);
    } else {
      presenter.updateFramebuffers(renderer.getRenderDevice()->getSwapchain());
    }
  });

  mainLoop.run();
  device->waitForIdle();
}

} // namespace liquid::runtime
