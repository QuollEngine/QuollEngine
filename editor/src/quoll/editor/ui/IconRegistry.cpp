#include "quoll/core/Base.h"
#include "quoll/rhi/RenderDevice.h"
#include "quoll/loaders/ImageTextureLoader.h"

#include "IconRegistry.h"

namespace quoll::editor {

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,-warnings-as-errors)
std::unordered_map<EditorIcon, rhi::TextureHandle> IconRegistry::mIconMap{};

void IconRegistry::loadIcons(RenderStorage &renderStorage,
                             const std::filesystem::path &iconsPath) {
  QuollAssert(mIconMap.empty(), "Icons are already loaded");

  ImageTextureLoader loader(renderStorage);

  mIconMap.insert_or_assign(EditorIcon::Unknown,
                            loader.loadFromFile(iconsPath / "unknown.png"));
  mIconMap.insert_or_assign(EditorIcon::Directory,
                            loader.loadFromFile(iconsPath / "directory.png"));
  mIconMap.insert_or_assign(EditorIcon::Material,
                            loader.loadFromFile(iconsPath / "material.png"));
  mIconMap.insert_or_assign(EditorIcon::Texture,
                            loader.loadFromFile(iconsPath / "texture.png"));
  mIconMap.insert_or_assign(EditorIcon::Font,
                            loader.loadFromFile(iconsPath / "font.png"));
  mIconMap.insert_or_assign(EditorIcon::Mesh,
                            loader.loadFromFile(iconsPath / "mesh.png"));
  mIconMap.insert_or_assign(EditorIcon::Skeleton,
                            loader.loadFromFile(iconsPath / "skeleton.png"));
  mIconMap.insert_or_assign(EditorIcon::Audio,
                            loader.loadFromFile(iconsPath / "audio.png"));
  mIconMap.insert_or_assign(EditorIcon::Animation,
                            loader.loadFromFile(iconsPath / "animation.png"));
  mIconMap.insert_or_assign(EditorIcon::Animator,
                            loader.loadFromFile(iconsPath / "animator.png"));
  mIconMap.insert_or_assign(EditorIcon::InputMap,
                            loader.loadFromFile(iconsPath / "input-map.png"));
  mIconMap.insert_or_assign(EditorIcon::Prefab,
                            loader.loadFromFile(iconsPath / "prefab.png"));
  mIconMap.insert_or_assign(EditorIcon::Script,
                            loader.loadFromFile(iconsPath / "script.png"));
  mIconMap.insert_or_assign(EditorIcon::Environment,
                            loader.loadFromFile(iconsPath / "environment.png"));
  mIconMap.insert_or_assign(EditorIcon::Scene,
                            loader.loadFromFile(iconsPath / "scene.png"));
  mIconMap.insert_or_assign(EditorIcon::Sun,
                            loader.loadFromFile(iconsPath / "sun.png"));
  mIconMap.insert_or_assign(EditorIcon::Light,
                            loader.loadFromFile(iconsPath / "light.png"));
  mIconMap.insert_or_assign(EditorIcon::Camera,
                            loader.loadFromFile(iconsPath / "camera.png"));
}

} // namespace quoll::editor
