#include "Texture.h"

namespace liquid {

Texture::Texture(const SharedPtr<TextureBinder> &binder_, size_t size_,
                 const SharedPtr<StatsManager> &statsManager_)
    : binder(binder_), size(size_), statsManager(statsManager_) {
  if (statsManager) {
    statsManager->addTexture(size);
  }
}

Texture::~Texture() {
  if (statsManager) {
    statsManager->removeTexture(size);
  }
}

} // namespace liquid
