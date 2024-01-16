#pragma once

#include "quoll/rhi/TextureDescription.h"
#include "quoll/rhi/TextureViewDescription.h"

namespace quoll::rhi {

class MockTexture {
public:
  MockTexture(const TextureDescription &description);

  MockTexture(const TextureViewDescription &description);

  inline const TextureDescription &getDescription() const {
    return mDescription;
  }

  inline const TextureViewDescription &getViewDescription() const {
    return mViewDescription;
  }

private:
  std::vector<u8> mData;
  TextureDescription mDescription;
  TextureViewDescription mViewDescription;
};

} // namespace quoll::rhi
