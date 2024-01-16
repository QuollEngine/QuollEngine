#include "quoll/core/Base.h"
#include "quoll/core/Engine.h"
#include "gltf/AnimationStep.h"
#include "gltf/LightStep.h"
#include "gltf/MaterialStep.h"
#include "gltf/MeshStep.h"
#include "gltf/PrefabStep.h"
#include "gltf/SkeletonStep.h"
#include "GLTFImporter.h"

namespace quoll::editor {

GLTFImporter::GLTFImporter(AssetCache &assetCache, ImageLoader &imageLoader,
                           bool optimize)
    : mAssetCache(assetCache), mImageLoader(imageLoader), mOptimize(optimize) {}

Result<UUIDMap> GLTFImporter::loadFromPath(const Path &sourceAssetPath,
                                           const UUIDMap &uuids) {
  tinygltf::TinyGLTF loader;
  tinygltf::Model model;
  String error, warning;

  bool ret = loader.LoadBinaryFromFile(&model, &error, &warning,
                                       sourceAssetPath.string());

  if (!warning.empty()) {
    return Result<UUIDMap>::Error(warning);
    // TODO: Show warning (in a dialog)
  }

  if (!error.empty()) {
    return Result<UUIDMap>::Error(error);
    // TODO: Show error (in a dialog)
  }

  if (!ret) {
    return Result<UUIDMap>::Error("Cannot load GLB file");
  }

  GLTFImportData importData{mAssetCache, mImageLoader, sourceAssetPath,
                            uuids,       model,        mOptimize};

  loadMaterials(importData);
  loadSkeletons(importData);
  loadAnimations(importData);
  loadMeshes(importData);
  loadLights(importData);
  loadPrefabs(importData);

  return Result<UUIDMap>::Ok(importData.outputUuids, importData.warnings);
}

Result<Path> GLTFImporter::createEmbeddedGlb(const Path &source,
                                             const Path &destination) {
  tinygltf::TinyGLTF gltf;
  tinygltf::Model model;
  String error, warning;

  bool ret = false;
  if (source.extension() == ".gltf") {
    ret = gltf.LoadASCIIFromFile(&model, &error, &warning, source.string());
  } else if (source.extension() == ".glb") {
    ret = gltf.LoadBinaryFromFile(&model, &error, &warning, source.string());
  }

  if (!warning.empty()) {
    return Result<Path>::Error(warning);
  }

  if (!error.empty()) {
    return Result<Path>::Error(error);
  }

  if (!ret) {
    return Result<Path>::Error("Cannot load ASCII GLTF file");
  }

  auto destinationGlb = destination;
  destinationGlb.replace_extension("glb");

  ret = gltf.WriteGltfSceneToFile(&model, destinationGlb.string(), true, true,
                                  false, true);

  if (!ret) {
    return Result<Path>::Error("Cannot create binary GLB file from GLTF file");
  }

  return Result<Path>::Ok(destinationGlb);
}

} // namespace quoll::editor
