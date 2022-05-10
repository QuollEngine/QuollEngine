#pragma once

namespace liquidator {

enum class EditorIcon {
  Unknown,
  Directory,
  CreateDirectory,
  Material,
  Texture,
  Mesh,
  SkinnedMesh,
  Skeleton,
  Animation,
  Prefab,
  Script,
  Sun,
  Camera,
  Direction,
};

/**
 * @brief Icon registry
 *
 * Provides a way to select and render
 * icons
 */
class IconRegistry {
public:
  /**
   * @brief Load icons from path
   *
   * @param registry Resource registry
   * @param iconsPath Path to icons
   */
  void loadIcons(liquid::rhi::ResourceRegistry &registry,
                 const std::filesystem::path &iconsPath);

  /**
   * @brief Get icon
   *
   * @param icon Icon enum
   * @return Texture handle for the icon
   */
  inline liquid::rhi::TextureHandle getIcon(EditorIcon icon) {
    return mIconMap.at(icon);
  }

private:
  std::unordered_map<EditorIcon, liquid::rhi::TextureHandle> mIconMap;
};

} // namespace liquidator
