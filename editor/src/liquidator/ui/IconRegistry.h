#pragma once

#include "liquid/rhi/RenderHandle.h"
#include "liquid/rhi/RenderDevice.h"
#include "liquid/renderer/RenderStorage.h"

namespace liquid::editor {

enum class EditorIcon {
  Unknown,
  Directory,
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
  Light,
  Camera,
  Environment
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
   * @param renderStorage Render storage
   * @param iconsPath Path to icons
   */
  void loadIcons(RenderStorage &renderStorage,
                 const std::filesystem::path &iconsPath);

  /**
   * @brief Get icon
   *
   * @param icon Icon enum
   * @return Texture handle for the icon
   */
  inline rhi::TextureHandle getIcon(EditorIcon icon) const {
    return mIconMap.at(icon);
  }

private:
  std::unordered_map<EditorIcon, rhi::TextureHandle> mIconMap;
};

} // namespace liquid::editor
