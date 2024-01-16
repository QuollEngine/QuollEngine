#pragma once

#include "quoll/asset/Result.h"
#include "gltf/GLTFImportData.h"
#include "ImageLoader.h"

namespace quoll::editor {

class GLTFImporter {
public:
  GLTFImporter(AssetCache &assetCache, ImageLoader &imageLoader, bool optimize);

  Result<UUIDMap> loadFromPath(const Path &sourceAssetPath,
                               const UUIDMap &uuids);

  static Result<Path> createEmbeddedGlb(const Path &source,
                                        const Path &destination);

private:
  AssetCache &mAssetCache;
  ImageLoader &mImageLoader;
  bool mOptimize = false;
};

} // namespace quoll::editor
