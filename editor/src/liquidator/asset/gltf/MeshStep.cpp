#include "liquid/core/Base.h"
// TODO: Remove engine logger
#include "liquid/core/Engine.h"
#include "MeshStep.h"

#include "Buffer.h"

namespace liquidator {

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

  liquid::String meshName =
      "Mesh #" + std::to_string(i) + ", Primitive #" + std::to_string(p);

  if (primitive.attributes.find("POSITION") == primitive.attributes.end()) {
    return liquid::
        Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::Error(
            meshName + " skipped because it does not have position attribute");
  }

  auto &&positionMeta =
      getBufferMetaForAccessor(model, primitive.attributes.at("POSITION"));
  size_t vertexSize = positionMeta.accessor.count;

  // According to spec, position attribute can only be vec3<float>
  if (positionMeta.accessor.type == TINYGLTF_TYPE_VEC3 &&
      positionMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
    vertices.resize(vertexSize);
    auto *data = reinterpret_cast<const glm::vec3 *>(positionMeta.rawData);
    for (size_t i = 0; i < vertexSize; ++i) {
      vertices[i].x = data[i].x;
      vertices[i].y = data[i].y;
      vertices[i].z = data[i].z;
    }
  } else {
    return liquid::
        Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::Error(
            meshName + " skipped because it has invalid position format");
  }

  bool hasIndices = primitive.indices >= 0;

  if (hasIndices) {
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
      hasIndices = false;
      indices.clear();
      warnings.push_back("Mesh primitive has invalid index format");
    }
  }

  if (!hasIndices) {
    indices.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
      indices.at(i) = static_cast<uint32_t>(i);
    }
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
    } else {
      warnings.push_back(meshName + " normals have invalid format");
    }
  } else {
    // TODO: Create flat normals
    warnings.push_back(meshName + " does not have normals");
  }

  if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
    auto &&tangentMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("TANGENT"));
    // According to spec, tangent attribute can only be vec4<float> and
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
    } else {
      warnings.push_back(meshName + " tangents have invalid format");
    }
  } else {
    // TODO: Calculate tangents using MikkTSpace algorithms
    warnings.push_back(meshName + " does not have tangents");
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
 * @param importData GLTF import data
 */
void loadMeshes(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
  const auto &targetPath = importData.targetPath;
  const auto &model = importData.model;

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
                          ? importData.materials.map.at(primitive.material)
                          : liquid::MaterialAssetHandle::Invalid;

      if (isSkinnedMesh) {
        auto &&result = loadStandardMeshAttributes<liquid::SkinnedVertex>(
            primitive, i, p, model);

        if (result.hasError()) {
          importData.warnings.push_back(result.getError());
          continue;
        }

        importData.warnings.insert(importData.warnings.end(),
                                   result.getWarnings().begin(),
                                   result.getWarnings().end());

        auto &vertices = result.getData().first;
        auto &indices = result.getData().second;

        if (primitive.attributes.find("JOINTS_0") !=
            primitive.attributes.end()) {

          auto &&jointMeta = getBufferMetaForAccessor(
              model, primitive.attributes.at("JOINTS_0"));

          if (jointMeta.accessor.type != TINYGLTF_TYPE_VEC4) {
            liquid::Engine::getLogger().warning()
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
        }
      } else {
        auto &&result =
            loadStandardMeshAttributes<liquid::Vertex>(primitive, i, p, model);

        if (result.hasError()) {
          importData.warnings.push_back(result.getError());
          continue;
        }

        importData.warnings.insert(importData.warnings.end(),
                                   result.getWarnings().begin(),
                                   result.getWarnings().end());

        auto &vertices = result.getData().first;
        auto &indices = result.getData().second;

        if (vertices.size() > 0) {
          mesh.data.geometries.push_back({vertices, indices, material});
        }
      }
    }

    if (isSkinnedMesh && !skinnedMesh.data.geometries.empty()) {
      skinnedMesh.name =
          targetPath.string() + "/skinnedmesh" + std::to_string(i);
      skinnedMesh.type = liquid::AssetType::SkinnedMesh;

      auto path = assetCache.createSkinnedMeshFromAsset(skinnedMesh);
      auto handle = assetCache.loadSkinnedMeshFromFile(path.getData());
      importData.skinnedMeshes.map.insert_or_assign(i, handle.getData());
    } else if (!mesh.data.geometries.empty()) {
      mesh.name = targetPath.string() + "/mesh" + std::to_string(i);
      mesh.type = liquid::AssetType::Mesh;

      auto path = assetCache.createMeshFromAsset(mesh);
      auto handle = assetCache.loadMeshFromFile(path.getData());
      importData.meshes.map.insert_or_assign(i, handle.getData());
    }
  }
}

} // namespace liquidator
