#include "quoll/core/Base.h"
#include "MockTexture.h"

namespace quoll::rhi {

MockTexture::MockTexture(const TextureDescription &description)
    : mDescription(description) {
  // TODO: Calculate size
}

MockTexture::MockTexture(const TextureViewDescription &description)
    : mViewDescription(description) {}

} // namespace quoll::rhi
