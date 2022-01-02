#include "core/Base.h"
#include "Texture.h"

namespace liquid {

Texture::Texture(const SharedPtr<TextureBinder> &binder_, size_t size_,
                 uint32_t width_, uint32_t height_, uint32_t layers_,
                 uint32_t format_, StatsManager &statsManager_)
    : binder(binder_), size(size_), width(width_), height(height_),
      layers(layers_), format(format_), statsManager(statsManager_) {
  statsManager.addTexture(size);
}

Texture::~Texture() { statsManager.removeTexture(size); }

} // namespace liquid
