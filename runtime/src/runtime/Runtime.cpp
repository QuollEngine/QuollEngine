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
#include "liquid/asset/AssetManager.h"

namespace liquid::runtime {

Runtime::Runtime(const LaunchConfig &config) : mConfig(config) {}

void Runtime::start() {
  static constexpr uint32_t Width = 800;
  static constexpr uint32_t Height = 600;

  liquid::EntityDatabase entityDatabase;
  liquid::EventSystem eventSystem;
  liquid::Window window(mConfig.name, Width, Height, eventSystem);
  liquid::AssetManager assetManager(mConfig.assetsPath, true);

  liquid::rhi::VulkanRenderBackend backend(window);
  auto *device = backend.createDefaultDevice();
  auto res = assetManager.preloadAssets(device);

  liquid::FPSCounter fpsCounter;
  liquid::MainLoop mainLoop(window, fpsCounter);

  liquid::rhi::RenderGraph graph;
  liquid::Renderer renderer(assetManager.getRegistry(), window, device);

  static constexpr glm::vec4 BlueishClearValue{0.52f, 0.54f, 0.89f, 1.0f};

  renderer.getSceneRenderer().setClearColor(BlueishClearValue);

  auto passData = renderer.getSceneRenderer().attach(graph);
  renderer.getSceneRenderer().attachText(graph, passData);

  liquid::ScriptingSystem scriptingSystem(eventSystem,
                                          assetManager.getRegistry());
  liquid::SceneUpdater sceneUpdater;
  liquid::PhysicsSystem physicsSystem(eventSystem);
  liquid::Presenter presenter(renderer.getShaderLibrary(), device);
  liquid::CameraAspectRatioUpdater cameraAspectRatioUpdater(window);
  liquid::AnimationSystem animationSystem(assetManager.getRegistry());
  liquid::SkeletonUpdater skeletonUpdater;
  liquid::AudioSystem audioSystem(assetManager.getRegistry());
  liquid::EntityDeleter entityDeleter;

  graph.setFramebufferExtent(window.getFramebufferSize());
  window.addResizeHandler([&graph](auto width, auto height) {
    graph.setFramebufferExtent({width, height});
  });

  liquid::SceneIO sceneIO(assetManager.getRegistry(), entityDatabase);
  sceneIO.loadScene(mConfig.scenesPath);

  auto cameraEntity = liquid::EntityNull;
  entityDatabase.iterateEntities<liquid::PerspectiveLensComponent>(
      [&cameraEntity](auto entity, auto &) mutable { cameraEntity = entity; });

  presenter.updateFramebuffers(device->getSwapchain());

  mainLoop.setUpdateFn([&](float dt) mutable {
    eventSystem.poll();

    cameraAspectRatioUpdater.update(entityDatabase);
    scriptingSystem.start(entityDatabase);
    scriptingSystem.update(dt, entityDatabase);
    animationSystem.update(dt, entityDatabase);
    skeletonUpdater.update(entityDatabase);
    sceneUpdater.update(entityDatabase);
    physicsSystem.update(dt, entityDatabase);
    audioSystem.output(entityDatabase);
    entityDeleter.update(entityDatabase);

    return true;
  });

  mainLoop.setRenderFn([&]() {
    const auto &renderFrame = device->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      renderer.getSceneRenderer().updateFrameData(entityDatabase, cameraEntity);

      renderer.render(graph, renderFrame.commandList);

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
