#include "core/Base.h"
#include "Texture.h"

namespace liquid {

Texture::Texture(const SharedPtr<TextureBinder> &binder_, size_t size_,
                 StatsManager &statsManager_)
    : binder(binder_), size(size_), statsManager(statsManager_) {
  statsManager.addTexture(size);
}

Texture::~Texture() { statsManager.removeTexture(size); }

} // namespace liquid
