#pragma once

#include "liquid/asset/AssetRegistry.h"

namespace liquidator {

class GLTFImporter {
public:
  GLTFImporter(liquid::AssetRegistry &regisyry);

  void loadFromFile(const liquid::String &filename);

private:
  liquid::AssetRegistry &mRegistry;
};

} // namespace liquidator
