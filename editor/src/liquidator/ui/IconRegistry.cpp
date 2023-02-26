#include "liquid/core/Base.h"
#include "liquid/rhi/RenderDevice.h"
#include "liquid/loaders/ImageTextureLoader.h"

#include "IconRegistry.h"

namespace liquid::editor {

void IconRegistry::loadIcons(RenderStorage &renderStorage,
                             const std::filesystem::path &iconsPath) {
  ImageTextureLoader loader(renderStorage);

  mIconMap.insert_or_assign(
      EditorIcon::Unknown,
      loader.loadFromFile((iconsPath / "unknown.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Directory,
      loader.loadFromFile((iconsPath / "directory.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Material,
      loader.loadFromFile((iconsPath / "material.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Texture,
      loader.loadFromFile((iconsPath / "texture.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Font, loader.loadFromFile((iconsPath / "font.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Mesh, loader.loadFromFile((iconsPath / "mesh.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Skeleton,
      loader.loadFromFile((iconsPath / "skeleton.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Audio,
      loader.loadFromFile((iconsPath / "audio.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Animation,
      loader.loadFromFile((iconsPath / "animation.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Prefab,
      loader.loadFromFile((iconsPath / "prefab.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Script,
      loader.loadFromFile((iconsPath / "script.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Environment,
      loader.loadFromFile((iconsPath / "environment.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Sun, loader.loadFromFile((iconsPath / "sun.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Camera,
      loader.loadFromFile((iconsPath / "camera.png").string()));
}

} // namespace liquid::editor
