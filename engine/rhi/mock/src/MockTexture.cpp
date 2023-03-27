#include "liquid/core/Base.h"
#include "MockTexture.h"

namespace liquid::rhi {

MockTexture::MockTexture(const TextureDescription &description)
    : mDescription(description) {
  // TODO: Calculate size
}

} // namespace liquid::rhi
