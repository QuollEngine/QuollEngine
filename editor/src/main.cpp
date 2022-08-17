#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "liquid/rhi-vulkan/VulkanRenderBackend.h"
#include "liquidator/screens/EditorScreen.h"
#include "liquidator/screens/ProjectSelectorScreen.h"

int main() {
  static constexpr uint32_t InitialWidth = 1024;
  static constexpr uint32_t InitialHeight = 768;

  liquid::Engine::setPath(std::filesystem::current_path() / "engine");

  liquid::EventSystem eventSystem;
  liquid::Window window("Liquidator", InitialWidth, InitialHeight, eventSystem);

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
