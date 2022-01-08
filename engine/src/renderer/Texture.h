#pragma once

#include "core/Base.h"
#include "TextureBinder.h"
#include "profiler/StatsManager.h"

namespace liquid {

constexpr size_t CUBE_FACES = 6;

struct TextureData {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t channels = 0;
  uint32_t format = 0;
  unsigned char *data = nullptr;
};

struct TextureCubemapDataLayerParams {
  size_t offset;
  size_t size;
};

struct TextureCubemapData {
  uint32_t width = 0;
  uint32_t height = 0;
  size_t size = 0;
  uint32_t format = 0;

  unsigned char *data = nullptr;
  std::array<TextureCubemapDataLayerParams, CUBE_FACES> faceData{};
};

struct TextureFramebufferData {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;
  uint32_t format = 0;
  uint32_t usageFlags = 0;
  uint32_t aspectMask = 0;
};

class Texture : public std::enable_shared_from_this<Texture> {
public:
  /**
   * @brief Create resource
   *
   * @param binder Texture binder
   * @param size Texture size
   * @param width Texture width
   * @param height Texture height
   * @param layers Texture layers
   * @param format Texture format
   * @param statsManager Stats manager
   */
  Texture(const SharedPtr<TextureBinder> &binder, size_t size, uint32_t width,
          uint32_t height, uint32_t layers, uint32_t format,
          StatsManager &statsManager);

  /**
   * @brief Destroy texture
   *
   * If stats manager exists, statistics will be collected
   */
  ~Texture();

  Texture(const Texture &rhs) = delete;
  Texture(Texture &&rhs) = delete;
  Texture &operator=(const Texture &rhs) = delete;
  Texture &operator=(Texture &&rhs) = delete;

  /**
   * @brief Get texture resource binder
   *
   * @return Texture resource binder
   */
  inline const SharedPtr<TextureBinder> &getResourceBinder() { return binder; }

  /**
   * @brief Get texture size
   *
   * @return Texture size
   */
  inline size_t getSize() const { return size; }

  /**
   * @brief Get width
   *
   * @return Width
   */
  inline uint32_t getWidth() const { return width; }

  /**
   * @brief Get height
   *
   * @return Height
   */
  inline uint32_t getHeight() const { return height; }

  /**
   * @brief Get layers
   *
   * @return Layers
   */
  inline uint32_t getLayers() const { return layers; }

  /**
   * @brief Get format
   *
   * @return Format
   */
  inline uint32_t getFormat() const { return format; }

private:
  SharedPtr<TextureBinder> binder = nullptr;
  StatsManager &statsManager;
  size_t size = 0;
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t layers = 0;
  uint32_t format = 0;
};

} // namespace liquid
