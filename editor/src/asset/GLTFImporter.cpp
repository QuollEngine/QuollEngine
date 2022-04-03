#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "GLTFImporter.h"

#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_USE_CPP14

#include <json/json.hpp>
#include <stb/stb_image.h>
#include <tinygltf/tiny_gltf.h>

namespace liquidator {

using Logger = liquid::Logger;

template <class THandle> struct GLTFToAsset { std::map<size_t, THandle> map; };

struct BufferMeta {
  tinygltf::Accessor accessor;
  tinygltf::BufferView bufferView;
  const unsigned char *rawData;
};

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
 * @brief Load textures into registry
 *
 * @param model GLTF model
 * @param fileName File name
 * @param registry Asset registry
 * @return Asset map
 */
static GLTFToAsset<liquid::TextureAssetHandle>
loadTextures(const tinygltf::Model &model, const liquid::String &fileName,
             liquid::AssetRegistry &registry) {
  std::map<size_t, liquid::TextureAssetHandle> map;
  for (size_t i = 0; i < model.textures.size(); ++i) {
    // TODO: Support creating different samplers
    auto &image = model.images.at(model.textures.at(i).source);
    liquid::AssetData<liquid::TextureAsset> texture{};
    texture.name = image.uri;
    texture.type = liquid::AssetType::Texture;
    texture.size = image.width * image.height * 4;
    texture.data.data = new char[texture.size];
    texture.data.width = image.width;
    texture.data.height = image.height;
    memcpy(texture.data.data, image.image.data(), texture.size);

    auto handle = registry.getTextures().addAsset(texture);
    map.insert_or_assign(i, handle);
  }

  return {map};
}

/**
 * @brief Load materials into registry
 *
 * @param model GLTF model
 * @param fileName File name
 * @param registry Asset registry
 * @param textures Texture map
 * @return Asset map
 */
static GLTFToAsset<liquid::MaterialAssetHandle>
loadMaterials(const tinygltf::Model &model, const liquid::String &fileName,
              liquid::AssetRegistry &registry,
              const GLTFToAsset<liquid::TextureAssetHandle> &textures) {
  std::map<size_t, liquid::MaterialAssetHandle> map;

  for (size_t i = 0; i < model.materials.size(); ++i) {
    auto &gltfMaterial = model.materials.at(i);

    liquid::AssetData<liquid::MaterialAsset> material;
    material.name = fileName + ":material " + std::to_string(i);
    material.type = liquid::AssetType::Material;

    if (gltfMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0) {
      material.data.baseColorTexture = textures.map.at(
          gltfMaterial.pbrMetallicRoughness.baseColorTexture.index);
    }
    material.data.baseColorTextureCoord =
        gltfMaterial.pbrMetallicRoughness.baseColorTexture.texCoord;
    auto &colorFactor = gltfMaterial.pbrMetallicRoughness.baseColorFactor;
    material.data.baseColorFactor = glm::vec4{colorFactor[0], colorFactor[1],
                                              colorFactor[2], colorFactor[3]};

    if (gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
      material.data.metallicRoughnessTexture = textures.map.at(
          gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index);
    }
    material.data.metallicRoughnessTextureCoord =
        gltfMaterial.pbrMetallicRoughness.baseColorTexture.texCoord;
    material.data.metallicFactor =
        static_cast<float>(gltfMaterial.pbrMetallicRoughness.metallicFactor);
    material.data.roughnessFactor =
        static_cast<float>(gltfMaterial.pbrMetallicRoughness.roughnessFactor);

    if (gltfMaterial.normalTexture.index >= 0) {
      material.data.normalTexture =
          textures.map.at(gltfMaterial.normalTexture.index);
    }
    material.data.normalTextureCoord = gltfMaterial.normalTexture.texCoord;
    material.data.normalScale =
        static_cast<float>(gltfMaterial.normalTexture.scale);

    if (gltfMaterial.occlusionTexture.index >= 0) {
      material.data.occlusionTexture =
          textures.map.at(gltfMaterial.occlusionTexture.index);
    }
    material.data.occlusionTextureCoord =
        gltfMaterial.occlusionTexture.texCoord;
    material.data.occlusionStrength =
        static_cast<float>(gltfMaterial.occlusionTexture.strength);

    if (gltfMaterial.emissiveTexture.index >= 0) {
      material.data.emissiveTexture =
          textures.map.at(gltfMaterial.emissiveTexture.index);
    }
    material.data.emissiveTextureCoord = gltfMaterial.emissiveTexture.texCoord;
    auto &emissiveFactor = gltfMaterial.emissiveFactor;
    material.data.emissiveFactor =
        glm::vec3{emissiveFactor[0], emissiveFactor[1], emissiveFactor[2]};

    auto handle = registry.getMaterials().addAsset(material);
    map.insert_or_assign(i, handle);
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
std::pair<std::vector<TVertex>, std::vector<uint32_t>>
loadStandardMeshAttributes(const tinygltf::Primitive &primitive, size_t i,
                           size_t p, const tinygltf::Model &model) {
  std::vector<uint32_t> indices;
  std::vector<TVertex> vertices;

  if (primitive.attributes.find("POSITION") == primitive.attributes.end()) {
    liquid::engineLogger.log(Logger::Warning)
        << "Mesh #" << i << ", Primitive #" << p
        << " does not have a position attribute. Skipping...";
    return {};
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
      return {};
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
    return {};
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
    liquid::engineLogger.log(Logger::Warning)
        << "Calculating flat normals is not supported";
    // TODO: Calculate flat normals
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
      }
    }
  }

  return {vertices, indices};
}

/**
 * @brief Reads mesh data and creates mesh instances
 *
 * Conforms to on GLTF 2.0 spec
 * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * @param model TinyGLTF model
 * @param fileName File name
 * @param registry Asset registry
 * @param materials Material map
 */
static void
loadMeshes(const tinygltf::Model &model, const liquid::String &fileName,
           liquid::AssetRegistry &registry,
           const GLTFToAsset<liquid::MaterialAssetHandle> &materials) {
  for (auto i = 0; i < model.meshes.size(); ++i) {
    const auto &gltfMesh = model.meshes.at(i);

    // TODO: Support multiple primitives
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

    liquid::AssetData<liquid::MeshAsset<liquid::Vertex>> mesh;
    liquid::AssetData<liquid::MeshAsset<liquid::SkinnedVertex>> skinnedMesh;

    for (size_t p = 0; p < gltfMesh.primitives.size(); ++p) {
      const auto &primitive = gltfMesh.primitives.at(p);

      auto material = primitive.material >= 0
                          ? materials.map.at(primitive.material)
                          : liquid::MaterialAssetHandle::Invalid;

      if (isSkinnedMesh) {
        auto &&[vertices, indices] =
            loadStandardMeshAttributes<liquid::SkinnedVertex>(primitive, i, p,
                                                              model);

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
          skinnedMesh.name = fileName + ":mesh" + std::to_string(i);
          skinnedMesh.type = liquid::AssetType::SkinnedMesh;
          registry.getSkinnedMeshes().addAsset(skinnedMesh);
        }
      } else {
        const auto &[vertices, indices] =
            loadStandardMeshAttributes<liquid::Vertex>(primitive, i, p, model);
        if (vertices.size() > 0) {
          mesh.name = fileName + ":mesh" + std::to_string(i);
          mesh.type = liquid::AssetType::Mesh;
          mesh.data.geometries.push_back({vertices, indices, material});
          registry.getMeshes().addAsset(mesh);
        }
      }
    }
  }
}

GLTFImporter::GLTFImporter(liquid::AssetRegistry &assetRegistry,
                           liquid::rhi::ResourceRegistry &deviceRegistry)
    : mAssetRegistry(assetRegistry), mDeviceRegistry(deviceRegistry) {}

void GLTFImporter::loadFromFile(const liquid::String &filename) {
  tinygltf::TinyGLTF loader;
  tinygltf::Model model;
  liquid::String error, warning;

  bool ret = loader.LoadASCIIFromFile(&model, &error, &warning, filename);

  if (!warning.empty()) {
    // TODO: Show warning (in a dialog)
  }

  if (!error.empty()) {
    // TODO: Show error (in a dialog)
  }

  if (!ret) {
    // TODO: Show error (in a dialog)
  }

  auto baseName = std::filesystem::path(filename).filename().string();

  auto &&textures = loadTextures(model, baseName, mAssetRegistry);
  auto &&materials = loadMaterials(model, baseName, mAssetRegistry, textures);
  loadMeshes(model, baseName, mAssetRegistry, materials);

  mAssetRegistry.syncWithDeviceRegistry(mDeviceRegistry);
}

} // namespace liquidator
