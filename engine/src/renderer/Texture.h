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
  uint32_t format = 0;
  uint32_t layers = 0;
};

class Texture {
public:
  /**
   * @brief Create resource
   *
   * @param binder Texture binder
   * @param size Texture size
   * @param statsManager Stats manager
   */
  Texture(const SharedPtr<TextureBinder> &binder, size_t size,
          const SharedPtr<StatsManager> &statsManager);

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

private:
  SharedPtr<TextureBinder> binder = nullptr;
  SharedPtr<StatsManager> statsManager = nullptr;
  size_t size = 0;
};

} // namespace liquid
