#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"

#include "quoll/rhi-vulkan/VulkanRenderBackend.h"
#include "quoll/editor/screens/EditorScreen.h"
#include "quoll/editor/screens/ProjectSelectorScreen.h"

int main() {
  static constexpr uint32_t InitialWidth = 1024;
  static constexpr uint32_t InitialHeight = 768;

  quoll::Engine::setPath(std::filesystem::current_path() / "engine");

  quoll::EventSystem eventSystem;
  quoll::Window window("Liquidator", InitialWidth, InitialHeight, eventSystem);

  quoll::rhi::VulkanRenderBackend backend(window);
  auto *device = backend.createDefaultDevice();

  quoll::editor::ProjectSelectorScreen projectSelector(window, eventSystem,
                                                       device);

  auto project = projectSelector.start();

  device->destroyResources();
  if (project.has_value()) {
    quoll::Engine::getLogger().info()
        << "Project selected: " << project.value().name;

    quoll::editor::EditorScreen editor(window, eventSystem, device);
    editor.start(project.value());
  }

  return 0;
}
