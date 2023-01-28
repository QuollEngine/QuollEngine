#include "liquid/core/Base.h"

#include "MaterialStep.h"

namespace liquid::editor {

/**
 * @brief Load materials into registry
 *
 * @param importData GLTF import data
 */
void loadMaterials(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &targetPath = importData.targetPath;
  const auto &model = importData.model;
  const auto &textures = importData.textures;

  for (size_t i = 0; i < model.materials.size(); ++i) {
    auto &gltfMaterial = model.materials.at(i);

    AssetData<MaterialAsset> material;
    material.name = targetPath.string() + "/material" + std::to_string(i);
    material.type = AssetType::Material;

    if (gltfMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0) {
      material.data.baseColorTexture = textures.map.at(
          gltfMaterial.pbrMetallicRoughness.baseColorTexture.index);
    }
    material.data.baseColorTextureCoord = static_cast<int8_t>(
        gltfMaterial.pbrMetallicRoughness.baseColorTexture.texCoord);
    auto &colorFactor = gltfMaterial.pbrMetallicRoughness.baseColorFactor;
    material.data.baseColorFactor = glm::vec4{colorFactor[0], colorFactor[1],
                                              colorFactor[2], colorFactor[3]};

    if (gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
      material.data.metallicRoughnessTexture = textures.map.at(
          gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index);
    }
    material.data.metallicRoughnessTextureCoord = static_cast<int8_t>(
        gltfMaterial.pbrMetallicRoughness.baseColorTexture.texCoord);
    material.data.metallicFactor =
        static_cast<float>(gltfMaterial.pbrMetallicRoughness.metallicFactor);
    material.data.roughnessFactor =
        static_cast<float>(gltfMaterial.pbrMetallicRoughness.roughnessFactor);

    if (gltfMaterial.normalTexture.index >= 0) {
      material.data.normalTexture =
          textures.map.at(gltfMaterial.normalTexture.index);
    }
    material.data.normalTextureCoord =
        static_cast<int8_t>(gltfMaterial.normalTexture.texCoord);
    material.data.normalScale =
        static_cast<float>(gltfMaterial.normalTexture.scale);

    if (gltfMaterial.occlusionTexture.index >= 0) {
      material.data.occlusionTexture =
          textures.map.at(gltfMaterial.occlusionTexture.index);
    }
    material.data.occlusionTextureCoord =
        static_cast<int8_t>(gltfMaterial.occlusionTexture.texCoord);
    material.data.occlusionStrength =
        static_cast<float>(gltfMaterial.occlusionTexture.strength);

    if (gltfMaterial.emissiveTexture.index >= 0) {
      material.data.emissiveTexture =
          textures.map.at(gltfMaterial.emissiveTexture.index);
    }
    material.data.emissiveTextureCoord =
        static_cast<int8_t>(gltfMaterial.emissiveTexture.texCoord);
    auto &emissiveFactor = gltfMaterial.emissiveFactor;
    material.data.emissiveFactor =
        glm::vec3{emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]};

    auto path = assetCache.createMaterialFromAsset(material);
    auto handle = assetCache.loadMaterialFromFile(path.getData());
    importData.materials.map.insert_or_assign(i, handle.getData());
  }
}

} // namespace liquid::editor
