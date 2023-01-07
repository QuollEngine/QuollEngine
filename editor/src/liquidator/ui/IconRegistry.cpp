#include "liquid/core/Base.h"
#include "liquid/rhi/RenderDevice.h"
#include "liquid/loaders/ImageTextureLoader.h"

#include "IconRegistry.h"

namespace liquidator {

void IconRegistry::loadIcons(liquid::RenderStorage &renderStorage,
                             const std::filesystem::path &iconsPath) {
  liquid::ImageTextureLoader loader(renderStorage);

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
      EditorIcon::Sun, loader.loadFromFile((iconsPath / "sun.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Camera,
      loader.loadFromFile((iconsPath / "camera.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Direction,
      loader.loadFromFile((iconsPath / "direction.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Play, loader.loadFromFile((iconsPath / "play.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Stop, loader.loadFromFile((iconsPath / "stop.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Move, loader.loadFromFile((iconsPath / "move.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Rotate,
      loader.loadFromFile((iconsPath / "rotate.png").string()));
  mIconMap.insert_or_assign(
      EditorIcon::Scale,
      loader.loadFromFile((iconsPath / "scale.png").string()));
}

} // namespace liquidator
