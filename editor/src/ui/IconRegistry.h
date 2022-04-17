#pragma once

namespace liquidator {

enum class EditorIcon {
  Unknown,
  Directory,
  Material,
  Texture,
  Mesh,
  SkinnedMesh,
  Skeleton,
  Animation,
  Prefab
};

class IconRegistry {
public:
  void loadIcons(liquid::rhi::ResourceRegistry &registry,
                 const std::filesystem::path &iconsPath);

  inline liquid::rhi::TextureHandle getIcon(EditorIcon icon) {
    return mIconMap.at(icon);
  }

private:
  std::unordered_map<EditorIcon, liquid::rhi::TextureHandle> mIconMap;
};

} // namespace liquidator
