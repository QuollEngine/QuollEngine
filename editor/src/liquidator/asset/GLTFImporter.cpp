#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"

#include "GLTFImporter.h"
#include "gltf/TextureStep.h"
#include "gltf/MaterialStep.h"
#include "gltf/MeshStep.h"
#include "gltf/SkeletonStep.h"
#include "gltf/AnimationStep.h"
#include "gltf/PrefabStep.h"

namespace liquid::editor {

GLTFImporter::GLTFImporter(AssetCache &assetCache, ImageLoader &imageLoader,
                           bool optimize)
    : mAssetCache(assetCache), mImageLoader(imageLoader), mOptimize(optimize) {}

Result<Path> GLTFImporter::loadFromPath(const Path &originalAssetPath,
                                        const Path &engineAssetPath) {
  tinygltf::TinyGLTF loader;
  tinygltf::Model model;
  String error, warning;

  bool ret = loader.LoadBinaryFromFile(&model, &error, &warning,
                                       originalAssetPath.string());

  if (!warning.empty()) {
    return Result<Path>::Error(warning);
    // TODO: Show warning (in a dialog)
  }

  if (!error.empty()) {
    return Result<Path>::Error(error);
    // TODO: Show error (in a dialog)
  }

  if (!ret) {
    return Result<Path>::Error("Cannot load GLB file");
  }

  if (std::filesystem::exists(engineAssetPath)) {
    std::filesystem::remove_all(engineAssetPath);
  }

  std::filesystem::create_directory(engineAssetPath);

  GLTFImportData importData{mAssetCache, mImageLoader, engineAssetPath, model,
                            mOptimize};

  loadTextures(importData);
  loadMaterials(importData);
  loadSkeletons(importData);
  loadAnimations(importData);
  loadMeshes(importData);
  loadPrefabs(importData);

  if (!importData.outputPath.hasData()) {
    return importData.outputPath;
  }

  return Result<Path>::Ok(importData.outputPath.getData(), importData.warnings);
}

Result<Path> GLTFImporter::saveBinary(const Path &source,
                                      const Path &destination) {
  tinygltf::TinyGLTF gltf;
  tinygltf::Model model;
  String error, warning;

  bool ret = gltf.LoadASCIIFromFile(&model, &error, &warning, source.string());

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

} // namespace liquid::editor
