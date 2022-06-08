#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "GLTFImporter.h"

#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_USE_CPP14
#define TINYGLTF_IMPLEMENTATION

#include <json/json.hpp>
#include <stb/stb_image.h>
#include <tinygltf/tiny_gltf.h>

namespace liquidator {

using Logger = liquid::Logger;

/**
 * @brief Mapping for GLTF index to
 * engine specific asset
 *
 * @tparam THandle Asset handle
 */
template <class THandle> struct GLTFToAsset {
  /**
   * GLTF index to asset handle map
   */
  std::map<size_t, THandle> map;
};

/**
 * @brief Transient buffer meta data
 *
 * Stores accessor, bufferview, and raw data
 * for easy operations
 */
struct BufferMeta {
  /**
   * TinyGLTF accessor
   */
  tinygltf::Accessor accessor;

  /**
   * TinyGLTF buffer view
   */
  tinygltf::BufferView bufferView;

  /**
   * Raw data
   */
  const unsigned char *rawData;
};

/**
 * @brief Transient transform data
 *
 * Stores transform information
 * for all nodes in a GLTF file
 */
struct TransformData {
  /**
   * Local position
   */
  glm::vec3 localPosition{0.0f};

  /**
   * Local rotation
   */
  glm::quat localRotation{1.0f, 0.0f, 0.0f, 0.0f};

  /**
   * Local scale
   */
  glm::vec3 localScale{1.0f};

  /**
   * Local transform matrix
   */
  glm::mat4 localTransform{1.0f};
};

/**
 * @brief Transient skeleton data
 *
 * Used to store GLTF skins an joints with engine
 * specific skeleton handles
 */
struct SkeletonData {
  /**
   * GLTF joint to engine specific joint I
   */
  std::unordered_map<uint32_t, uint32_t> gltfToNormalizedJointMap;

  /**
   * Joints that are associated with skins
   */
  std::unordered_map<uint32_t, uint32_t> jointSkinMap;

  /**
   * Skin map
   */
  GLTFToAsset<liquid::SkeletonAssetHandle> skeletonMap;
};

/**
 * @brief Transient animation data
 *
 * Used to store GLTF animations with engine
 * specific animation handles
 */
struct AnimationData {
  /**
   * Node to animation map
   */
  std::map<uint32_t, std::vector<liquid::AnimationAssetHandle>>
      nodeAnimationMap;

  /**
   * Skin to animation map
   */
  std::map<uint32_t, std::vector<liquid::AnimationAssetHandle>>
      skinAnimationMap;
};

/**
 * @brief Decomposes matrix into TRS values
 *
 * @param matrix Input matrix
 * @param position Output position
 * @param rotation Output rotation
 * @param scale Output scale
 */
static void decomposeMatrix(const glm::mat4 &matrix, glm::vec3 &position,
                            glm::quat &rotation, glm::vec3 &scale) {
  glm::mat4 temp = matrix;
  position = temp[3];

  for (glm::mat4::length_type i = 0; i < 3; ++i) {
    scale[i] = glm::length(temp[i]);
    temp[i] /= scale[i];
  }

  rotation = glm::toQuat(matrix);
}

/**
 * @brief Geta buffer metadata for accessor
 *
 * @param model TinyGLTF model
 * @param accessorIndex Index of buffer accessor
 * @return GLTF buffer metadata
 */
static BufferMeta getBufferMetaForAccessor(const tinygltf::Model &model,
                                           int accessorIndex) {
  auto accessor = model.accessors.at(accessorIndex);
  auto bufferView = model.bufferViews.at(accessor.bufferView);
  const unsigned char *bufferStart =
      &model.buffers.at(bufferView.buffer).data.at(0);
  const unsigned char *bufferOffset =
      bufferStart + accessor.byteOffset + bufferView.byteOffset;

  return {accessor, bufferView, bufferOffset};
}

/**
 * @brief Load transform data
 *
 * Load matrix or each transform
 * attribute based on the given
 * ones
 *
 * @param node
 * @return
 */
TransformData loadTransformData(const tinygltf::Node &node) {
  TransformData data{};

  constexpr size_t TRANSFORM_MATRIX_SIZE = 6;
  constexpr size_t TRANSLATION_MATRIX_SIZE = 3;
  constexpr size_t SCALE_MATRIX_SIZE = 3;
  constexpr size_t ROTATION_MATRIX_SIZE = 4;

  glm::mat4 finalTransform = glm::mat4{1.0f};
  if (node.matrix.size() == TRANSFORM_MATRIX_SIZE) {
    finalTransform = glm::make_mat4(node.matrix.data());
    decomposeMatrix(finalTransform, data.localPosition, data.localRotation,
                    data.localScale);

  } else if (node.matrix.size() > 0) {
    liquid::engineLogger.log(Logger::Warning)
        << "Node matrix data must have 16 values. Skipping...";
  } else {
    if (node.translation.size() == TRANSLATION_MATRIX_SIZE) {
      data.localPosition = glm::make_vec3(node.translation.data());
      finalTransform *= glm::translate(glm::mat4{1.0f}, data.localPosition);
    } else if (node.translation.size() > 0) {
      liquid::engineLogger.log(Logger::Warning)
          << "Node translation data must have 3 values. Skipping...";
    }

    if (node.rotation.size() == ROTATION_MATRIX_SIZE) {
      data.localRotation = glm::make_quat(node.rotation.data());
      finalTransform *= glm::toMat4(data.localRotation);
    } else if (node.rotation.size() > 0) {
      liquid::engineLogger.log(Logger::Warning)
          << "Node rotation data must have 4 values. Skipping...";
    }

    if (node.scale.size() == SCALE_MATRIX_SIZE) {
      data.localScale = glm::make_vec3(node.scale.data());
      finalTransform *= glm::scale(glm::mat4{1.0f}, data.localScale);
    } else if (node.scale.size() > 0) {
      liquid::engineLogger.log(Logger::Warning)
          << "Node scale data must have 3 values. Skipping...";
    }
  }

  data.localTransform = finalTransform;

  return data;
}

/**
 * @brief Load textures into registry
 *
 * @param model GLTF model
 * @param path Path to directory
 * @param registry Asset registry
 * @return Asset map
 */
static GLTFToAsset<liquid::TextureAssetHandle>
loadTextures(const tinygltf::Model &model, const std::filesystem::path &path,
             liquid::AssetManager &assetManager) {
  std::map<size_t, liquid::TextureAssetHandle> map;

  for (size_t i = 0; i < model.textures.size(); ++i) {
    // TODO: Support creating different samplers
    auto &image = model.images.at(model.textures.at(i).source);
    liquid::AssetData<liquid::TextureAsset> texture{};
    texture.name = path.string() + "/" + image.uri;
    texture.type = liquid::AssetType::Texture;
    texture.size = image.width * image.height * 4;
    texture.data.data =
        const_cast<void *>(static_cast<const void *>(image.image.data()));
    texture.data.width = image.width;
    texture.data.height = image.height;

    auto &&texturePath = assetManager.createTextureFromAsset(texture);
    auto handle = assetManager.loadTextureFromFile(texturePath.getData());
    map.insert_or_assign(i, handle.getData());
  }

  return {map};
}

/**
 * @brief Load materials into registry
 *
 * @param model GLTF model
 * @param pathDirectory Path to asset
 * @param registry Asset registry
 * @param textures Texture map
 * @return Asset map
 */
static GLTFToAsset<liquid::MaterialAssetHandle>
loadMaterials(const tinygltf::Model &model,
              const std::filesystem::path &pathDirectory,
              liquid::AssetManager &assetManager,
              const GLTFToAsset<liquid::TextureAssetHandle> &textures) {
  std::map<size_t, liquid::MaterialAssetHandle> map;

  for (size_t i = 0; i < model.materials.size(); ++i) {
    auto &gltfMaterial = model.materials.at(i);

    liquid::AssetData<liquid::MaterialAsset> material;
    material.name = pathDirectory.string() + "/material" + std::to_string(i);
    material.type = liquid::AssetType::Material;

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

    auto path = assetManager.createMaterialFromAsset(material);
    auto handle = assetManager.loadMaterialFromFile(path.getData());
    map.insert_or_assign(i, handle.getData());
  }

  return {map};
}

/**
 * @brief Load standard mesh attributes
 *
 * Loads position, normal, tangents,
 * and texture coordinates that exist
 * for both mesh and skinned mesh
 *
 * @tparam TMesh Mesh type
 * @param primitive GLTF mesh primitive
 * @param i Mesh index
 * @param p Primitive index
 * @param model GLTF model
 * @return Vertices and indices
 */
template <class TVertex>
liquid::Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>
loadStandardMeshAttributes(const tinygltf::Primitive &primitive, size_t i,
                           size_t p, const tinygltf::Model &model) {
  std::vector<uint32_t> indices;
  std::vector<TVertex> vertices;
  std::vector<liquid::String> warnings;

  if (primitive.attributes.find("POSITION") == primitive.attributes.end()) {
    liquid::engineLogger.log(Logger::Warning)
        << "Mesh #" << i << ", Primitive #" << p
        << " does not have a position attribute. Skipping...";
    return liquid::
        Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::Error(
            "Mesh primitive not have position attribute");
  }

  if (primitive.indices >= 0) {
    auto &&indexMeta = getBufferMetaForAccessor(model, primitive.indices);
    indices.resize(indexMeta.accessor.count);
    if (indexMeta.accessor.componentType ==
            TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT &&
        indexMeta.accessor.type == TINYGLTF_TYPE_SCALAR) {
      const auto *data = reinterpret_cast<const uint32_t *>(indexMeta.rawData);
      for (size_t i = 0; i < indexMeta.accessor.count; ++i) {
        indices[i] = data[i];
      }
    } else if (indexMeta.accessor.componentType ==
                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT &&
               indexMeta.accessor.type == TINYGLTF_TYPE_SCALAR) {
      const auto *data = reinterpret_cast<const uint16_t *>(indexMeta.rawData);
      for (size_t i = 0; i < indexMeta.accessor.count; ++i) {
        indices[i] = data[i];
      }
    } else if (indexMeta.accessor.componentType ==
                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE &&
               indexMeta.accessor.type == TINYGLTF_TYPE_SCALAR) {
      const auto *data = reinterpret_cast<const uint8_t *>(indexMeta.rawData);
      for (size_t i = 0; i < indexMeta.accessor.count; ++i) {
        indices[i] = data[i];
      }
    } else {
      liquid::engineLogger.log(Logger::Warning)
          << "Mesh #" << i << ", Primitive #" << p
          << " has invalid index format. Skipping...";
      return liquid::
          Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::Error(
              "Mesh primitive has invalid index format");
    }
  }

  auto &&positionMeta =
      getBufferMetaForAccessor(model, primitive.attributes.at("POSITION"));

  size_t vertexSize = positionMeta.accessor.count;
  vertices.resize(vertexSize);

  // According to spec, position attribute can only be vec3<float>
  if (positionMeta.accessor.type == TINYGLTF_TYPE_VEC3 &&
      positionMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
    auto *data = reinterpret_cast<const glm::vec3 *>(positionMeta.rawData);
    for (size_t i = 0; i < vertexSize; ++i) {
      vertices[i].x = data[i].x;
      vertices[i].y = data[i].y;
      vertices[i].z = data[i].z;
    }
  } else {
    liquid::engineLogger.log(Logger::Warning)
        << "Mesh #" << i << ", Primitive #0"
        << " has invalid position format. Skipping...";
    return liquid::
        Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::Error(
            "Mesh primitive has invalid position format");
  }

  if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
    auto &&normalMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("NORMAL"));
    // According to spec, normal attribute can only be vec3<float> and
    // all attributes of a primitive must have the same number of items
    if (normalMeta.accessor.type == TINYGLTF_TYPE_VEC3 &&
        normalMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT &&
        normalMeta.accessor.count == vertexSize) {
      auto *data = reinterpret_cast<const glm::vec3 *>(normalMeta.rawData);
      for (size_t i = 0; i < vertexSize; ++i) {
        vertices[i].nx = data[i].x;
        vertices[i].ny = data[i].y;
        vertices[i].nz = data[i].z;
      }
    }
  } else {
    warnings.push_back("Mesh does not have normals");
    liquid::engineLogger.log(Logger::Warning)
        << "Calculating flat normals is not supported";
  }

  if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
    auto &&tangentMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("TANGENT"));
    // According to spec, normal attribute can only be vec4<float> and
    // all attributes of a primitive must have the same number of items
    if (tangentMeta.accessor.type == TINYGLTF_TYPE_VEC4 &&
        tangentMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT &&
        tangentMeta.accessor.count == vertexSize) {
      auto *data = reinterpret_cast<const glm::vec4 *>(tangentMeta.rawData);
      for (size_t i = 0; i < vertexSize; ++i) {
        vertices[i].tx = data[i].x;
        vertices[i].ty = data[i].y;
        vertices[i].tz = data[i].z;
        vertices[i].tw = data[i].w;
      }
    }
  } else {
    // TODO: Calculate tangents using MikkTSpace algorithms
    warnings.push_back("Mesh does not have tangents");
    liquid::engineLogger.log(Logger::Warning)
        << "Tangents will be calculated using derivative functions in "
           "pixel shader. For more accurate results, you need to provide "
           "the tangent attribute when generating GLTF model.";
  }

  if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
    auto &&uvMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("TEXCOORD_0"));

    // According to spec, UV data is always in vec2 format
    // and all attributes of a primitive must have
    // the same number of items
    if (uvMeta.accessor.type == TINYGLTF_TYPE_VEC2 &&
        uvMeta.accessor.count == vertexSize) {
      // UV coordinates can be float, ubyte, and ushort
      if (uvMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
        auto *data = reinterpret_cast<const glm::vec2 *>(uvMeta.rawData);
        for (size_t i = 0; i < vertexSize; ++i) {
          vertices[i].u0 = data[i].x;
          vertices[i].v0 = data[i].y;
        }
      } else if (uvMeta.accessor.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ||
                 uvMeta.accessor.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        // TODO: Convert integer coordinates to float
        liquid::engineLogger.log(Logger::Warning)
            << "Integer based texture coordinates are not supported for "
               "TEXCOORD0";
        warnings.push_back("Integer based texture coordinates are not "
                           "supported for TEXCOORD0");
      }
    }
  }

  if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) {
    auto &&uvMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("TEXCOORD_1"));

    // According to spec, UV data is always in vec2 format
    // and all attributes of a primitive must have
    // the same number of items
    if (uvMeta.accessor.type == TINYGLTF_TYPE_VEC2 &&
        uvMeta.accessor.count == vertexSize) {
      // UV coordinates can be float, ubyte, and ushort
      if (uvMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
        auto *data = reinterpret_cast<const glm::vec2 *>(uvMeta.rawData);
        for (size_t i = 0; i < vertexSize; ++i) {
          vertices[i].u1 = data[i].x;
          vertices[i].v1 = data[i].y;
        }
      } else if (uvMeta.accessor.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ||
                 uvMeta.accessor.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        // TODO: Convert integer coordinates to float
        liquid::engineLogger.log(Logger::Warning)
            << "Integer based texture coordinates are not supported for "
               "TEXCOORD1";
        warnings.push_back("Integer based texture coordinates are not "
                           "supported for TEXCOORD1");
      }
    }
  }

  return liquid::Result<
      std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::Ok({vertices,
                                                                   indices},
                                                                  warnings);
}

/**
 * @brief Loads meshes into asset registry
 *
 * Conforms to on GLTF 2.0 spec
 * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * @param model TinyGLTF model
 * @param pathDirectory Path to asset
 * @param manager Asset manager
 * @param materials Material map
 * @param skeletons Skeleton map
 */
static liquid::Result<bool>
loadMeshes(const tinygltf::Model &model,
           const std::filesystem::path &pathDirectory,
           liquid::AssetManager &manager,
           const GLTFToAsset<liquid::MaterialAssetHandle> &materials,
           const GLTFToAsset<liquid::SkeletonAssetHandle> &skeletons,
           GLTFToAsset<liquid::MeshAssetHandle> &outMeshes,
           GLTFToAsset<liquid::SkinnedMeshAssetHandle> &outSkinnedMeshes) {
  std::vector<liquid::String> warnings;

  std::map<size_t, size_t> skeletonMeshMap;
  for (auto &node : model.nodes) {
    if (node.skin >= 0 && node.mesh >= 0) {
      skeletonMeshMap.insert_or_assign(static_cast<size_t>(node.mesh),
                                       static_cast<size_t>(node.skin));
    }
  }

  for (auto i = 0; i < model.meshes.size(); ++i) {
    const auto &gltfMesh = model.meshes.at(i);

    if (gltfMesh.primitives.empty()) {
      // TODO: Add warning
      continue;
    }

    bool isSkinnedMesh = false;
    for (auto &primitive : gltfMesh.primitives) {
      if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
        isSkinnedMesh = true;
      }
    }

    liquid::AssetData<liquid::MeshAsset> mesh;
    liquid::AssetData<liquid::SkinnedMeshAsset> skinnedMesh;

    for (size_t p = 0; p < gltfMesh.primitives.size(); ++p) {
      const auto &primitive = gltfMesh.primitives.at(p);

      auto material = primitive.material >= 0
                          ? materials.map.at(primitive.material)
                          : liquid::MaterialAssetHandle::Invalid;

      if (isSkinnedMesh) {
        auto &&result = loadStandardMeshAttributes<liquid::SkinnedVertex>(
            primitive, i, p, model);

        if (result.hasError()) {
          warnings.push_back(result.getError());
          continue;
        }

        warnings.insert(warnings.end(), result.getWarnings().begin(),
                        result.getWarnings().end());

        auto &vertices = result.getData().first;
        auto &indices = result.getData().second;

        if (primitive.attributes.find("JOINTS_0") !=
            primitive.attributes.end()) {

          auto &&jointMeta = getBufferMetaForAccessor(
              model, primitive.attributes.at("JOINTS_0"));

          if (jointMeta.accessor.type != TINYGLTF_TYPE_VEC4) {
            liquid::engineLogger.log(liquid::Logger::Warning)
                << "Mesh #" << i
                << " JOINTS_0 is not in VEC4 format. Skipping...";
          } else if (jointMeta.accessor.componentType ==
                         TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE &&
                     jointMeta.accessor.type == TINYGLTF_TYPE_VEC4) {
            const auto *data =
                reinterpret_cast<const glm::u8vec4 *>(jointMeta.rawData);

            for (size_t i = 0; i < jointMeta.accessor.count; ++i) {
              vertices.at(i).j0 = data[i].x;
              vertices.at(i).j1 = data[i].y;
              vertices.at(i).j2 = data[i].z;
              vertices.at(i).j3 = data[i].w;
            }
          } else if (jointMeta.accessor.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
            const auto *data =
                reinterpret_cast<const glm::u16vec4 *>(jointMeta.rawData);
            for (size_t i = 0; i < jointMeta.accessor.count; ++i) {
              vertices.at(i).j0 = data[i].x;
              vertices.at(i).j1 = data[i].y;
              vertices.at(i).j2 = data[i].z;
              vertices.at(i).j3 = data[i].w;
            }
          }
        }

        if (primitive.attributes.find("WEIGHTS_0") !=
            primitive.attributes.end()) {
          auto &&weightMeta = getBufferMetaForAccessor(
              model, primitive.attributes.at("WEIGHTS_0"));
          if (weightMeta.accessor.componentType ==
              TINYGLTF_COMPONENT_TYPE_FLOAT) {
            const auto *data =
                reinterpret_cast<const glm::vec4 *>(weightMeta.rawData);

            for (size_t i = 0; i < weightMeta.accessor.count; ++i) {
              vertices.at(i).w0 = data[i].x;
              vertices.at(i).w1 = data[i].y;
              vertices.at(i).w2 = data[i].z;
              vertices.at(i).w3 = data[i].w;
            }
          }
        }

        if (vertices.size() > 0) {
          skinnedMesh.data.geometries.push_back({vertices, indices, material});
          skinnedMesh.name =
              pathDirectory.string() + "/skinnedmesh" + std::to_string(i);
          skinnedMesh.type = liquid::AssetType::SkinnedMesh;

          auto it = skeletonMeshMap.find(i);
          if (it != skeletonMeshMap.end() &&
              skeletons.map.find(it->second) != skeletons.map.end()) {
            skinnedMesh.data.skeleton = skeletons.map.at(it->second);
          }

          auto path = manager.createSkinnedMeshFromAsset(skinnedMesh);
          auto handle = manager.loadSkinnedMeshFromFile(path.getData());
          outSkinnedMeshes.map.insert_or_assign(i, handle.getData());
        }
      } else {
        auto &&result =
            loadStandardMeshAttributes<liquid::Vertex>(primitive, i, p, model);

        if (result.hasError()) {
          warnings.push_back(result.getError());
          continue;
        }

        warnings.insert(warnings.end(), result.getWarnings().begin(),
                        result.getWarnings().end());

        auto &vertices = result.getData().first;
        auto &indices = result.getData().second;

        if (vertices.size() > 0) {
          mesh.name = pathDirectory.string() + "/mesh" + std::to_string(i);
          mesh.type = liquid::AssetType::Mesh;
          mesh.data.geometries.push_back({vertices, indices, material});

          auto path = manager.createMeshFromAsset(mesh);
          auto handle = manager.loadMeshFromFile(path.getData());
          outMeshes.map.insert_or_assign(i, handle.getData());
        }
      }
    }
  }

  return liquid::Result<bool>::Ok(true, warnings);
}

/**
 * @brief Load skeletons into asset registry
 *
 * Conforms to on GLTF 2.0 spec
 * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * @param model TinyGLTF model
 * @param path Path to asset
 * @param manager Asset manager
 */
SkeletonData loadSkeletons(const tinygltf::Model &model,
                           const std::filesystem::path &path,
                           liquid::AssetManager &manager) {
  SkeletonData skeletonData{};

  for (uint32_t si = 0; si < static_cast<uint32_t>(model.skins.size()); ++si) {
    const auto &skin = model.skins.at(si);

    std::unordered_map<uint32_t, int> jointParents;
    std::unordered_map<uint32_t, uint32_t> normalizedJointMap;

    auto &&ibMeta = getBufferMetaForAccessor(model, skin.inverseBindMatrices);

    if (ibMeta.accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
      liquid::engineLogger.log(Logger::Warning)
          << "Inverse bind matrices accessor must be of type FLOAT. Skipping "
             "skin #"
          << si;

      continue;
    }

    if (ibMeta.accessor.type != TINYGLTF_TYPE_MAT4) {
      liquid::engineLogger.log(Logger::Warning)
          << "Inverse bind matrices accessor must be MAT4. Skipping "
             "skin #"
          << si;
      continue;
    }

    if (ibMeta.accessor.count < skin.joints.size()) {
      liquid::engineLogger.log(Logger::Warning)
          << "Inverse bind matrices cannot be fewer than number of joints. "
             "Skipping "
             "skin #"
          << si;
      continue;
    }

    bool success = true;

    std::vector<glm::mat4> inverseBindMatrices(ibMeta.accessor.count);

    {
      const auto *data = reinterpret_cast<const glm::mat4 *>(ibMeta.rawData);
      for (size_t i = 0; i < ibMeta.accessor.count; ++i) {
        inverseBindMatrices.at(i) = data[i];
      }
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(skin.joints.size()); ++i) {
      const auto &joint = skin.joints.at(i);

      if (skeletonData.gltfToNormalizedJointMap.find(joint) !=
          skeletonData.gltfToNormalizedJointMap.end()) {
        liquid::engineLogger.log(Logger::Warning)
            << "Single joint cannot be a child of multiple skins. Skipping "
               "joint #"
            << joint
            << " for "
               "skin #"
            << si;
        continue;
      }
      LIQUID_ASSERT(skeletonData.gltfToNormalizedJointMap.find(joint) ==
                        skeletonData.gltfToNormalizedJointMap.end(),
                    "Single joint cannot be accessed by multiple skins");
      normalizedJointMap.insert({joint, i});
      skeletonData.gltfToNormalizedJointMap.insert({joint, i});
      skeletonData.jointSkinMap.insert({joint, si});

      jointParents.insert({i, -1});
    }

    for (uint32_t j = 0; j < static_cast<uint32_t>(model.nodes.size()); ++j) {
      if (normalizedJointMap.find(j) == normalizedJointMap.end()) {
        continue;
      }

      uint32_t nJ = normalizedJointMap.at(j);

      const auto &node = model.nodes.at(j);
      for (auto &child : node.children) {
        if (normalizedJointMap.find(child) == normalizedJointMap.end()) {
          continue;
        }

        uint32_t nChild = normalizedJointMap.at(child);
        jointParents.at(nChild) = static_cast<int>(nJ);
      }
    }

    uint32_t numJoints = static_cast<uint32_t>(skin.joints.size());

    liquid::AssetData<liquid::SkeletonAsset> asset;
    asset.name = path.string() + "/" + path.filename().string() + "-skeleton" +
                 std::to_string(si);
    asset.type = liquid::AssetType::Skeleton;

    for (auto &joint : skin.joints) {
      uint32_t nJoint = normalizedJointMap.at(joint);
      int parent = jointParents.at(nJoint);
      const auto &node = model.nodes.at(joint);
      const auto &data = loadTransformData(node);

      asset.data.jointLocalPositions.push_back(data.localPosition);
      asset.data.jointLocalRotations.push_back(data.localRotation);
      asset.data.jointLocalScales.push_back(data.localScale);
      asset.data.jointInverseBindMatrices.push_back(
          inverseBindMatrices.at(nJoint));
      asset.data.jointNames.push_back(node.name);
      asset.data.jointParents.push_back(parent >= 0 ? parent : 0);
    }

    auto path = manager.createSkeletonFromAsset(asset);
    auto handle = manager.loadSkeletonFromFile(path.getData());

    skeletonData.skeletonMap.map.insert_or_assign(static_cast<size_t>(si),
                                                  handle.getData());
  }

  return skeletonData;
}

/**
 * @brief Load animations into asset registry
 *
 * Conforms to on GLTF 2.0 spec
 * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * @param model TinyGLTF model
 * @param pathDirectory Path to asset
 * @param manager Asset manager
 * @param skeletonData Skeleton data
 */
AnimationData loadAnimations(const tinygltf::Model &model,
                             const std::filesystem::path &pathDirectory,
                             liquid::AssetManager &manager,
                             const SkeletonData &skeletonData) {
  AnimationData animationData;
  for (size_t i = 0; i < model.animations.size(); ++i) {
    const auto &gltfAnimation = model.animations.at(i);

    struct SamplerInfo {
      std::vector<float> times;
      std::vector<glm::vec4> values;
      liquid::KeyframeSequenceAssetInterpolation interpolation =
          liquid::KeyframeSequenceAssetInterpolation::Linear;
    };

    std::vector<SamplerInfo> samplers(gltfAnimation.samplers.size());

    float maxTime = 0.0f;

    for (size_t i = 0; i < gltfAnimation.samplers.size(); ++i) {
      const auto &sampler = gltfAnimation.samplers.at(i);
      const auto &input = getBufferMetaForAccessor(model, sampler.input);
      const auto &output = getBufferMetaForAccessor(model, sampler.output);

      if (input.accessor.type != TINYGLTF_TYPE_SCALAR) {
        liquid::engineLogger.log(Logger::Warning)
            << "Animation time accessor must be in SCALAR format. Skipping...";
        continue;
      }

      if (input.accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        liquid::engineLogger.log(Logger::Warning)
            << "Animation time accessor component type must be FLOAT";
        continue;
      }

      if (input.accessor.count != output.accessor.count) {
        liquid::engineLogger.log(Logger::Warning)
            << "Sampler input and output must have the same number of items";
        continue;
      }

      if (output.accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT) {
        liquid::engineLogger.log(Logger::Warning)
            << "Animation output accessor component type must be FLOAT";
        continue;
      }

      std::vector<float> &times = samplers.at(i).times;
      times.resize(input.accessor.count);

      std::vector<glm::vec4> &values = samplers.at(i).values;
      values.resize(output.accessor.count);

      if (sampler.interpolation == "LINEAR") {
        samplers.at(i).interpolation =
            liquid::KeyframeSequenceAssetInterpolation::Linear;
      } else if (sampler.interpolation == "STEP") {
        samplers.at(i).interpolation =
            liquid::KeyframeSequenceAssetInterpolation::Step;
      }

      float max = 0.0f;

      {
        const float *inputData = reinterpret_cast<const float *>(input.rawData);
        for (size_t i = 0; i < input.accessor.count; ++i) {
          times.at(i) = inputData[i];
          max = std::max(max, inputData[i]);
        }
      }

      maxTime = std::max(max, maxTime);

      // Normalize the time
      {
        for (size_t i = 0; i < times.size(); ++i) {
          times.at(i) = times.at(i) / max;
        }
      }

      if (output.accessor.type == TINYGLTF_TYPE_VEC3) {
        const glm::vec3 *outputData =
            reinterpret_cast<const glm::vec3 *>(output.rawData);
        for (size_t i = 0; i < output.accessor.count; ++i) {
          values.at(i) = glm::vec4(outputData[i], 0.0f);
        }
      } else if (output.accessor.type == TINYGLTF_TYPE_VEC4) {
        const glm::vec4 *outputData =
            reinterpret_cast<const glm::vec4 *>(output.rawData);
        for (size_t i = 0; i < output.accessor.count; ++i) {
          values.at(i) = outputData[i];
        }
      } else if (output.accessor.type == TINYGLTF_TYPE_SCALAR) {
        const float *outputData =
            reinterpret_cast<const float *>(output.rawData);
        for (size_t i = 0; i < output.accessor.count; ++i) {
          values.at(i) = glm::vec4(outputData[i], 0, 0, 0);
        }
      }
    }

    liquid::AssetData<liquid::AnimationAsset> animation;
    animation.name = pathDirectory.string() + "/" + gltfAnimation.name;
    animation.data.time = maxTime;

    int32_t targetNode = -1;
    int32_t targetSkin = -1;

    for (const auto &channel : gltfAnimation.channels) {
      const auto &sampler = samplers.at(channel.sampler);

      if (channel.target_node == -1) {
        // Ignore channel if target node is not specified
        continue;
      }

      auto target = liquid::KeyframeSequenceAssetTarget::Position;
      if (channel.target_path == "rotation") {
        target = liquid::KeyframeSequenceAssetTarget::Rotation;
      } else if (channel.target_path == "scale") {
        target = liquid::KeyframeSequenceAssetTarget::Scale;
      } else if (channel.target_path == "position") {
        target = liquid::KeyframeSequenceAssetTarget::Position;
      }

      uint32_t targetJoint = 0;

      auto it = skeletonData.jointSkinMap.find(channel.target_node);
      bool skinFound = it != skeletonData.jointSkinMap.end();
      if (targetSkin == -1 && skinFound) {
        targetSkin = static_cast<int32_t>(it->second);
        targetJoint =
            skeletonData.gltfToNormalizedJointMap.at(channel.target_node);
      } else if (skinFound) {
        LIQUID_ASSERT(
            it->second == targetSkin,
            "All channels in animation must point to the same target skin");
        targetJoint =
            skeletonData.gltfToNormalizedJointMap.at(channel.target_node);
      }

      if (targetSkin == -1 && targetNode == -1) {
        targetNode = channel.target_node;
      } else {
        LIQUID_ASSERT(targetNode == -1, "All channels in animation must either "
                                        "animate skin or node, not both");
        LIQUID_ASSERT(
            targetNode == -1 || targetNode == channel.target_node,
            "All channels in animation must point to the same target node");
      }

      LIQUID_ASSERT(targetSkin == -1 || targetNode == -1,
                    "A channel must point to a node or a skin");

      liquid::KeyframeSequenceAsset sequence;
      sequence.interpolation = sampler.interpolation;
      sequence.target = target;

      if (targetSkin >= 0) {
        sequence.joint = targetJoint;
        sequence.jointTarget = true;
      }

      for (size_t i = 0; i < sampler.times.size(); ++i) {
        sequence.keyframeTimes.push_back(sampler.times.at(i));
        sequence.keyframeValues.push_back(sampler.values.at(i));
      }
      animation.data.keyframes.push_back(sequence);
    }

    LIQUID_ASSERT(targetNode >= 0 || targetSkin >= 0,
                  "Animation must have a target node or skin");
    auto filePath = manager.createAnimationFromAsset(animation);
    auto handle = manager.loadAnimationFromFile(filePath.getData());

    if (targetSkin >= 0) {
      if (animationData.skinAnimationMap.find(targetSkin) ==
          animationData.skinAnimationMap.end()) {
        animationData.skinAnimationMap.insert(
            {static_cast<uint32_t>(targetSkin), {}});
      }
      animationData.skinAnimationMap.at(targetSkin).push_back(handle.getData());
    } else {
      if (animationData.nodeAnimationMap.find(targetSkin) ==
          animationData.nodeAnimationMap.end()) {
        animationData.nodeAnimationMap.insert(
            {static_cast<uint32_t>(targetNode), {}});
      }

      animationData.nodeAnimationMap.at(targetNode).push_back(handle.getData());
    }
  }
  return animationData;
}

void loadPrefabs(
    const tinygltf::Model &model, const std::filesystem::path &pathDirectory,
    liquid::AssetManager &manager,
    const GLTFToAsset<liquid::MeshAssetHandle> &meshes,
    const GLTFToAsset<liquid::SkinnedMeshAssetHandle> &skinnedMeshes,
    const SkeletonData &skeletons, const AnimationData &animations) {

  liquid::AssetData<liquid::PrefabAsset> prefab;
  prefab.name = pathDirectory.string() + "/" + pathDirectory.stem().string();
  prefab.type = liquid::AssetType::Prefab;

  auto &gltfNodes = model.scenes.at(model.defaultScene);

  std::unordered_map<int, bool> jointNodes;
  for (auto &skin : model.skins) {
    for (auto &joint : skin.joints) {
      jointNodes.insert({joint, true});
    }
  }

  std::list<std::pair<int, int>> nodesToProcess;

  for (auto &nodeIndex : gltfNodes.nodes) {
    if (jointNodes.find(nodeIndex) == jointNodes.end()) {
      nodesToProcess.push_back(std::make_pair(nodeIndex, -1));
    }
  }

  while (!nodesToProcess.empty()) {
    auto [nodeIndex, parentIndex] = nodesToProcess.front();
    nodesToProcess.pop_front();

    auto &node = model.nodes.at(nodeIndex);

    for (auto child : node.children) {
      if (jointNodes.find(child) == jointNodes.end()) {
        nodesToProcess.push_back(std::make_pair(child, nodeIndex));
      }
    }

    if (node.mesh < 0)
      continue;

    auto localEntityId = static_cast<uint32_t>(nodeIndex);

    liquid::PrefabTransformData transform{};
    auto data = loadTransformData(node);
    transform.position = data.localPosition;
    transform.rotation = data.localRotation;
    transform.scale = data.localScale;
    transform.parent = static_cast<int32_t>(parentIndex);

    prefab.data.transforms.push_back({localEntityId, transform});

    if (node.skin >= 0) {
      prefab.data.skinnedMeshes.push_back(
          {localEntityId, skinnedMeshes.map.at(node.mesh)});
      prefab.data.skeletons.push_back(
          {localEntityId, skeletons.skeletonMap.map.at(node.skin)});

      liquid::AnimatorComponent component;

      auto it = animations.skinAnimationMap.find(node.skin);
      if (it != animations.skinAnimationMap.end()) {
        component.animations = it->second;
      }

      if (component.animations.size() > 0) {
        prefab.data.animators.push_back({localEntityId, component});
      }
    } else {
      prefab.data.meshes.push_back({localEntityId, meshes.map.at(node.mesh)});

      liquid::AnimatorComponent component;

      auto it = animations.nodeAnimationMap.find(nodeIndex);
      if (it != animations.nodeAnimationMap.end()) {
        component.animations = it->second;
      }

      if (component.animations.size() > 0) {
        prefab.data.animators.push_back({localEntityId, component});
      }
    }
  }

  auto path = manager.createPrefabFromAsset(prefab);
  manager.loadPrefabFromFile(path.getData());
}

GLTFImporter::GLTFImporter(liquid::AssetManager &assetManager,
                           liquid::rhi::ResourceRegistry &deviceRegistry)
    : mAssetManager(assetManager), mDeviceRegistry(deviceRegistry) {}

liquid::Result<bool>
GLTFImporter::loadFromFile(const liquid::Path &filePath,
                           const std::filesystem::path &directory) {
  tinygltf::TinyGLTF loader;
  tinygltf::Model model;
  liquid::String error, warning;

  bool ret =
      loader.LoadASCIIFromFile(&model, &error, &warning, filePath.string());

  if (!warning.empty()) {
    return liquid::Result<bool>::Error(warning);
    // TODO: Show warning (in a dialog)
  }

  if (!error.empty()) {
    return liquid::Result<bool>::Error(error);
    // TODO: Show error (in a dialog)
  }

  if (!ret) {
    return liquid::Result<bool>::Error("Cannot load GLTF file");
  }

  auto baseName = filePath.filename().string();

  auto prefabPath = directory / baseName;
  uint32_t index = 1;
  auto tmpPath = prefabPath;
  while (std::filesystem::exists(tmpPath)) {
    tmpPath = prefabPath;
    liquid::String uniqueSuffix = "-" + std::to_string(index++);
    tmpPath += uniqueSuffix;
  }

  prefabPath = tmpPath;
  baseName = std::filesystem::path(prefabPath).filename().string();

  std::filesystem::create_directory(prefabPath);

  GLTFToAsset<liquid::MeshAssetHandle> meshMap;
  GLTFToAsset<liquid::SkinnedMeshAssetHandle> skinnedMeshMap;

  std::vector<liquid::String> warnings;

  auto &&textures = loadTextures(model, prefabPath, mAssetManager);
  auto &&materials = loadMaterials(model, prefabPath, mAssetManager, textures);
  auto &&skeletonData = loadSkeletons(model, prefabPath, mAssetManager);
  auto &&animationData =
      loadAnimations(model, prefabPath, mAssetManager, skeletonData);

  const auto &meshResult =
      loadMeshes(model, prefabPath, mAssetManager, materials,
                 skeletonData.skeletonMap, meshMap, skinnedMeshMap);

  warnings.insert(warnings.end(), meshResult.getWarnings().begin(),
                  meshResult.getWarnings().end());

  loadPrefabs(model, prefabPath, mAssetManager, meshMap, skinnedMeshMap,
              skeletonData, animationData);

  mAssetManager.getRegistry().syncWithDeviceRegistry(mDeviceRegistry);

  return liquid::Result<bool>::Ok(true, warnings);
}

} // namespace liquidator
