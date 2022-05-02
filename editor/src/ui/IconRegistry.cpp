#include "liquid/core/Base.h"
#include "liquid/rhi/ResourceRegistry.h"
#include "liquid/loaders/ImageTextureLoader.h"

#include "IconRegistry.h"

namespace liquidator {

void IconRegistry::loadIcons(liquid::rhi::ResourceRegistry &registry,
                             const std::filesystem::path &iconsPath) {

  liquid::ImageTextureLoader loader(registry);

  mIconMap.insert_or_assign(
      EditorIcon::Unknown,
      loader.loadFromFile((iconsPath / "unknown.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Directory,
      loader.loadFromFile((iconsPath / "directory.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::CreateDirectory,
      loader.loadFromFile((iconsPath / "create-directory.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Material,
      loader.loadFromFile((iconsPath / "material.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Texture,
      loader.loadFromFile((iconsPath / "texture.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Mesh, loader.loadFromFile((iconsPath / "mesh.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Skeleton,
      loader.loadFromFile((iconsPath / "skeleton.png").string()));
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
      EditorIcon::Sun, loader.loadFromFile((iconsPath / "sun.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Camera,
      loader.loadFromFile((iconsPath / "camera.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Direction,
      loader.loadFromFile((iconsPath / "direction.png").string()));
}

} // namespace liquidator
