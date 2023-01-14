#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "GLTFImporter.h"
#include "gltf/TextureStep.h"
#include "gltf/MaterialStep.h"
#include "gltf/MeshStep.h"
#include "gltf/SkeletonStep.h"
#include "gltf/AnimationStep.h"
#include "gltf/PrefabStep.h"

namespace liquidator {

using Logger = liquid::Logger;

GLTFImporter::GLTFImporter(liquid::AssetCache &assetCache)
    : mAssetCache(assetCache) {}

liquid::Result<liquid::Path>
GLTFImporter::loadFromPath(const liquid::Path &originalAssetPath,
                           const liquid::Path &engineAssetPath) {
  tinygltf::TinyGLTF loader;
  tinygltf::Model model;
  liquid::String error, warning;

  bool ret = loader.LoadBinaryFromFile(&model, &error, &warning,
                                       originalAssetPath.string());

  if (!warning.empty()) {
    return liquid::Result<liquid::Path>::Error(warning);
    // TODO: Show warning (in a dialog)
  }

  if (!error.empty()) {
    return liquid::Result<liquid::Path>::Error(error);
    // TODO: Show error (in a dialog)
  }

  if (!ret) {
    return liquid::Result<liquid::Path>::Error("Cannot load GLB file");
  }

  std::vector<liquid::String> warnings;

  if (std::filesystem::exists(engineAssetPath)) {
    std::filesystem::remove_all(engineAssetPath);
  }

  std::filesystem::create_directory(engineAssetPath);

  GLTFImportData importData{mAssetCache, engineAssetPath, model};

  loadTextures(importData);
  loadMaterials(importData);
  loadSkeletons(importData);
  loadAnimations(importData);
  loadMeshes(importData);
  loadPrefabs(importData);

  if (!importData.outputPath.hasData()) {
    return importData.outputPath;
  }

  return liquid::Result<liquid::Path>::Ok(importData.outputPath.getData(),
                                          warnings);
}

liquid::Result<liquid::Path>
GLTFImporter::saveBinary(const liquid::Path &source,
                         const liquid::Path &destination) {
  tinygltf::TinyGLTF gltf;
  tinygltf::Model model;
  liquid::String error, warning;

  bool ret = gltf.LoadASCIIFromFile(&model, &error, &warning, source.string());

  if (!warning.empty()) {
    return liquid::Result<liquid::Path>::Error(warning);
  }

  if (!error.empty()) {
    return liquid::Result<liquid::Path>::Error(error);
  }

  if (!ret) {
    return liquid::Result<liquid::Path>::Error("Cannot load ASCII GLTF file");
  }

  auto destinationGlb = destination;
  destinationGlb.replace_extension("glb");

  ret = gltf.WriteGltfSceneToFile(&model, destinationGlb.string(), true, true,
                                  false, true);

  if (!ret) {
    return liquid::Result<liquid::Path>::Error(
        "Cannot create binary GLB file from GLTF file");
  }

  return liquid::Result<liquid::Path>::Ok(destinationGlb);
}

} // namespace liquidator
