#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/RenderDevice.h"

namespace liquidator {

enum class EditorIcon {
  Unknown,
  Directory,
  CreateDirectory,
  Material,
  Texture,
  Font,
  Mesh,
  SkinnedMesh,
  Skeleton,
  Animation,
  Audio,
  Prefab,
  Script,
  Sun,
  Camera,
  Direction,
  Play,
  Stop,
  Move,
  Rotate,
  Scale
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
   * @param device Render device
   * @param iconsPath Path to icons
   */
  void loadIcons(liquid::rhi::RenderDevice *device,
                 const std::filesystem::path &iconsPath);

  /**
   * @brief Get icon
   *
   * @param icon Icon enum
   * @return Texture handle for the icon
   */
  inline liquid::rhi::TextureHandle getIcon(EditorIcon icon) const {
    return mIconMap.at(icon);
  }

private:
  std::unordered_map<EditorIcon, liquid::rhi::TextureHandle> mIconMap;
};

} // namespace liquidator
