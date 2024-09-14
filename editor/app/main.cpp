#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/rhi-vulkan/VulkanRenderBackend.h"
#include "quoll/editor/windows/EditorWindow.h"
#include "quoll/editor/windows/ProjectSelectorWindow.h"

int main() {
  static constexpr u32 InitialWidth = 1024;
  static constexpr u32 InitialHeight = 768;

  quoll::Engine::setEnginePath(std::filesystem::current_path() / "engine");

  quoll::InputDeviceManager deviceManager;
  quoll::Window window("Quoll Engine", InitialWidth, InitialHeight,
                       deviceManager);

  quoll::rhi::VulkanRenderBackend backend(window);
  auto *device = backend.createDefaultDevice();

  quoll::editor::ProjectSelectorWindow projectSelector(window, device);

  auto project = projectSelector.start();

  device->destroyResources();
  if (project.has_value()) {
    quoll::Engine::getLogger().info()
        << "Project selected: " << project.value().name;

    quoll::editor::EditorWindow editor(window, deviceManager, device);
    editor.start(project.value());
  }

  return 0;
}
