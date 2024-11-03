#include "quoll/core/Base.h"
#include "quoll/renderer/RenderStorage.h"
#include "quoll/renderer/TextureUtils.h"
#include "quoll/editor/asset/Stb.h"
#include "IconRegistry.h"

namespace quoll::editor {

namespace {

constexpr auto createLoader(RenderStorage &renderStorage) {
  return [&renderStorage](const Path &path) {
    quoll::rhi::TextureDescription description;
    i32 width = 0, height = 0, channels = 0;

    void *data = stbi_load(path.string().c_str(), &width, &height, &channels,
                           STBI_rgb_alpha);
    QuollAssert(data, "Failed to load image: " + path.string());

    description.format = rhi::Format::Rgba8Srgb;
    description.width = width;
    description.height = height;
    description.usage = rhi::TextureUsage::Color |
                        rhi::TextureUsage::TransferDestination |
                        rhi::TextureUsage::Sampled;
    description.type = rhi::TextureType::Standard;
    description.debugName = path.filename().string();

    auto texture = renderStorage.createTexture(description);

    TextureUtils::copyDataToTexture(
        renderStorage.getDevice(), data, texture,
        rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
        {TextureAssetMipLevel{0, static_cast<usize>(width) * height * channels,
                              static_cast<u32>(width),
                              static_cast<u32>(height)}});

    return texture;
  };
}

} // namespace

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables,-warnings-as-errors)
std::unordered_map<EditorIcon, rhi::TextureHandle> IconRegistry::mIconMap{};

void IconRegistry::loadIcons(RenderStorage &renderStorage,
                             const std::filesystem::path &iconsPath) {
  QuollAssert(mIconMap.empty(), "Icons are already loaded");

  auto loadIcon = createLoader(renderStorage);

  mIconMap.insert_or_assign(EditorIcon::Unknown,
                            loadIcon(iconsPath / "unknown.png"));
  mIconMap.insert_or_assign(EditorIcon::Directory,
                            loadIcon(iconsPath / "directory.png"));
  mIconMap.insert_or_assign(EditorIcon::Material,
                            loadIcon(iconsPath / "material.png"));
  mIconMap.insert_or_assign(EditorIcon::Texture,
                            loadIcon(iconsPath / "texture.png"));
  mIconMap.insert_or_assign(EditorIcon::Font, loadIcon(iconsPath / "font.png"));
  mIconMap.insert_or_assign(EditorIcon::Mesh, loadIcon(iconsPath / "mesh.png"));
  mIconMap.insert_or_assign(EditorIcon::Skeleton,
                            loadIcon(iconsPath / "skeleton.png"));
  mIconMap.insert_or_assign(EditorIcon::Audio,
                            loadIcon(iconsPath / "audio.png"));
  mIconMap.insert_or_assign(EditorIcon::Animation,
                            loadIcon(iconsPath / "animation.png"));
  mIconMap.insert_or_assign(EditorIcon::Animator,
                            loadIcon(iconsPath / "animator.png"));
  mIconMap.insert_or_assign(EditorIcon::InputMap,
                            loadIcon(iconsPath / "input-map.png"));
  mIconMap.insert_or_assign(EditorIcon::Prefab,
                            loadIcon(iconsPath / "prefab.png"));
  mIconMap.insert_or_assign(EditorIcon::LuaScript,
                            loadIcon(iconsPath / "script.png"));
  mIconMap.insert_or_assign(EditorIcon::Environment,
                            loadIcon(iconsPath / "environment.png"));
  mIconMap.insert_or_assign(EditorIcon::Scene,
                            loadIcon(iconsPath / "scene.png"));
  mIconMap.insert_or_assign(EditorIcon::Sun, loadIcon(iconsPath / "sun.png"));
  mIconMap.insert_or_assign(EditorIcon::Light,
                            loadIcon(iconsPath / "light.png"));
  mIconMap.insert_or_assign(EditorIcon::Camera,
                            loadIcon(iconsPath / "camera.png"));
}

} // namespace quoll::editor
