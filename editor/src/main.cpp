#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "quoll/rhi-vulkan/VulkanRenderBackend.h"
#include "quoll/editor/screens/EditorScreen.h"
#include "quoll/editor/screens/ProjectSelectorScreen.h"

int main() {
  static constexpr u32 InitialWidth = 1024;
  static constexpr u32 InitialHeight = 768;

  quoll::Engine::create({.path = std::filesystem::current_path() / "engine",
                         .numThreads = std::thread::hardware_concurrency()});

  quoll::InputDeviceManager deviceManager;
  quoll::Window window("Quoll Engine", InitialWidth, InitialHeight,
                       deviceManager);

  quoll::rhi::VulkanRenderBackend backend(window);
  auto *device = backend.createDefaultDevice();

  quoll::editor::ProjectSelectorScreen projectSelector(window, device);

  auto project = projectSelector.start();

  device->destroyResources();
  if (project.has_value()) {
    quoll::Engine::getLogger().info()
        << "Project selected: " << project.value().name;

    quoll::editor::EditorScreen editor(window, deviceManager, device);
    editor.start(project.value());
  }

  quoll::Engine::destroy();

  return 0;
}
