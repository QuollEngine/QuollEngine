#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/rhi-vulkan/VulkanRenderBackend.h"
#include "screens/EditorScreen.h"
#include "screens/ProjectSelectorScreen.h"

int main() {
  static constexpr uint32_t INITIAL_WIDTH = 1024;
  static constexpr uint32_t INITIAL_HEIGHT = 768;

  liquid::Engine::setAssetsPath(
      std::filesystem::path("./engine/assets").string());

  liquid::EventSystem eventSystem;
  liquid::Window window("Liquidator", INITIAL_WIDTH, INITIAL_HEIGHT,
                        eventSystem);

  liquid::rhi::VulkanRenderBackend backend(window);
  auto *device = backend.createDefaultDevice();

  liquidator::ProjectSelectorScreen projectSelector(window, eventSystem,
                                                    device);

  auto project = projectSelector.start();

  device->waitForIdle();
  device->destroyResources();
  if (project.has_value()) {
    liquidator::EditorScreen editor(window, eventSystem, device);
    editor.start(project.value());
    device->waitForIdle();
  }

  return 0;
}
