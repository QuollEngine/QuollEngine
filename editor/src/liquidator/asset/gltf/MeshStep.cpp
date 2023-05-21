#include "liquid/core/Base.h"

#include "MeshStep.h"
#include "Buffer.h"
#include "mikktspace/MikktspaceAdapter.h"

#include <meshoptimizer.h>

namespace liquid::editor {

/**
 * @brief Get vertex position in vec3
 *
 * @tparam TVertex Vertex type
 * @param v Vertex
 * @return GLM vec3 position
 */
template <class TVertex> inline glm::vec3 getVertexPosition(TVertex &v) {
  return glm::vec3(v.x, v.y, v.z);
}

/**
 * @brief Generate normals from vertex positions
 *
 * @tparam TVertex Vertex type
 * @param vertices Vertices
 * @param indices Indices
 */
template <class TVertex>
void generateNormals(std::vector<TVertex> &vertices,
                     std::vector<uint32_t> &indices) {
  for (auto &v : vertices) {
    v.nx = 0.0f;
    v.ny = 0.0f;
    v.nz = 0.0f;
  }

  for (size_t i = 0; i < indices.size(); i += 3) {
    auto &v0 = vertices.at(indices.at(i));
    auto &v1 = vertices.at(indices.at(i + 1));
    auto &v2 = vertices.at(indices.at(i + 2));

    auto edge1 = getVertexPosition(v0) - getVertexPosition(v1);
    auto edge2 = getVertexPosition(v0) - getVertexPosition(v2);

    auto normal = glm::cross(edge1, edge2);

    v0.nx += normal.x;
    v0.ny += normal.y;
    v0.nz += normal.z;

    v1.nx += normal.x;
    v1.ny += normal.y;
    v1.nz += normal.z;

    v2.nx += normal.x;
    v2.ny += normal.y;
    v2.nz += normal.z;
  }

  for (auto &v : vertices) {
    auto n = glm::normalize(glm::vec3(v.nx, v.ny, v.nz));
    v.nx = n.x;
    v.ny = n.y;
    v.nz = n.z;
  }
}

/**
 * @brief Generate tangents using Mikktspace
 *
 * @tparam TVertex Vertex type
 * @param vertices Vertices
 * @param indices Indices
 */
template <class TVertex>
void generateTangents(std::vector<TVertex> &vertices,
                      const std::vector<uint32_t> &indices) {
  MikktspaceAdapter<TVertex> adapter;

  adapter.generate(vertices, indices);
}

/**
 * @brief Optimize meshes using meshoptimizer
 *
 * @tparam TVertex Vertex type
 * @param vertices Vertices
 * @param indices Indices
 */
template <class TVertex>
void optimizeMeshes(std::vector<TVertex> &vertices,
                    std::vector<uint32_t> &indices) {
  static constexpr float OverdrawThreshold = 1.05f;

  meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(),
                              vertices.size());
  meshopt_optimizeOverdraw(indices.data(), indices.data(), indices.size(),
                           &vertices.at(0).x, vertices.size(), sizeof(TVertex),
                           OverdrawThreshold);
  meshopt_optimizeVertexFetch(vertices.data(), indices.data(), indices.size(),
                              vertices.data(), vertices.size(),
                              sizeof(TVertex));
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
Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>
loadStandardMeshAttributes(const tinygltf::Primitive &primitive, size_t i,
                           size_t p, const tinygltf::Model &model) {
  std::vector<uint32_t> indices;
  std::vector<TVertex> vertices;
  std::vector<String> warnings;

  String meshName =
      "Mesh #" + std::to_string(i) + ", Primitive #" + std::to_string(p);

  if (primitive.attributes.find("POSITION") == primitive.attributes.end()) {
    return Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::
        Error(meshName +
              " skipped because it does not have position attribute");
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
    return Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::
        Error(meshName + " skipped because it has invalid position format");
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
      return Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::
          Error(meshName + " skipped because it has invalid index format");
    }
  } else {
    indices.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
      indices.at(i) = static_cast<uint32_t>(i);
    }
  }

  // According to spec, normal attribute can only be vec3<float> and
  // all attributes of a primitive must have the same number of items
  bool validNormals =
      primitive.attributes.find("NORMAL") != primitive.attributes.end();

  if (validNormals) {
    auto &&normalMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("NORMAL"));

    validNormals =
        normalMeta.accessor.type == TINYGLTF_TYPE_VEC3 &&
        normalMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT &&
        normalMeta.accessor.count == vertexSize;

    if (validNormals) {
      auto *data = reinterpret_cast<const glm::vec3 *>(normalMeta.rawData);
      for (size_t i = 0; i < vertexSize; ++i) {
        vertices[i].nx = data[i].x;
        vertices[i].ny = data[i].y;
        vertices[i].nz = data[i].z;
      }
    }
  }

  if (!validNormals) {
    warnings.push_back("Normals for " + meshName + " are generated");
    generateNormals(vertices, indices);
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

  // According to spec, tangent attribute can only be vec4<float> and
  // all attributes of a primitive must have the same number of items
  bool validTangents =
      primitive.attributes.find("TANGENT") != primitive.attributes.end();

  if (validTangents) {
    auto &&tangentMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("TANGENT"));

    validTangents =
        tangentMeta.accessor.type == TINYGLTF_TYPE_VEC4 &&
        tangentMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT &&
        tangentMeta.accessor.count == vertexSize;

    if (validTangents) {
      auto *data = reinterpret_cast<const glm::vec4 *>(tangentMeta.rawData);
      for (size_t i = 0; i < vertexSize; ++i) {
        vertices[i].tx = data[i].x;
        vertices[i].ty = data[i].y;
        vertices[i].tz = data[i].z;
        vertices[i].tw = data[i].w;
      }
    }
  }

  if (!validTangents) {
    warnings.push_back("Tangents for " + meshName + " are generated");
    generateTangents(vertices, indices);
  }

  return Result<std::pair<std::vector<TVertex>, std::vector<uint32_t>>>::Ok(
      {vertices, indices}, warnings);
}

Result<bool> loadSkinnedMeshAttributes(const tinygltf::Primitive &primitive,
                                       size_t i, size_t p,
                                       const tinygltf::Model &model,
                                       std::vector<SkinnedVertex> &vertices) {
  String meshName = "Skinned mesh #" + std::to_string(i) + ", Primitive #" +
                    std::to_string(p);

  std::vector<String> warnings;

  bool validJoints =
      primitive.attributes.find("JOINTS_0") != primitive.attributes.end();

  if (validJoints) {
    auto &&jointMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("JOINTS_0"));

    validJoints = jointMeta.accessor.type == TINYGLTF_TYPE_VEC4;
    if (validJoints) {
      if (jointMeta.accessor.componentType ==
          TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
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
      } else {
        validJoints = false;
      }
    }
  }

  if (!validJoints) {
    warnings.push_back(meshName + " joints attribute is invalid");
  }

  bool validWeights =
      primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end();
  if (validWeights) {
    auto &&weightsMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("WEIGHTS_0"));

    validWeights =
        weightsMeta.accessor.type == TINYGLTF_TYPE_VEC4 &&
        weightsMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT;

    if (validWeights) {
      const auto *data =
          reinterpret_cast<const glm::vec4 *>(weightsMeta.rawData);

      for (size_t i = 0; i < weightsMeta.accessor.count; ++i) {
        vertices.at(i).w0 = data[i].x;
        vertices.at(i).w1 = data[i].y;
        vertices.at(i).w2 = data[i].z;
        vertices.at(i).w3 = data[i].w;
      }
    }
  }

  if (!validWeights) {
    warnings.push_back(meshName + " weights attribute is invalid");
  }

  return Result<bool>::Ok(true, warnings);
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

    AssetData<MeshAsset> mesh;
    AssetData<SkinnedMeshAsset> skinnedMesh;

    for (size_t p = 0; p < gltfMesh.primitives.size(); ++p) {
      const auto &primitive = gltfMesh.primitives.at(p);

      auto material = primitive.material >= 0
                          ? importData.materials.map.at(primitive.material)
                          : MaterialAssetHandle::Null;

      if (isSkinnedMesh) {
        auto &&result =
            loadStandardMeshAttributes<SkinnedVertex>(primitive, i, p, model);

        if (result.hasError()) {
          importData.warnings.push_back(result.getError());
          continue;
        }

        importData.warnings.insert(importData.warnings.end(),
                                   result.getWarnings().begin(),
                                   result.getWarnings().end());

        auto &vertices = result.getData().first;
        auto &indices = result.getData().second;

        auto &&skinnedResult =
            loadSkinnedMeshAttributes(primitive, i, p, model, vertices);

        if (skinnedResult.hasError()) {
          importData.warnings.push_back(result.getError());
          continue;
        }

        if (importData.optimize) {
          optimizeMeshes(vertices, indices);
        }

        importData.warnings.insert(importData.warnings.end(),
                                   skinnedResult.getWarnings().begin(),
                                   skinnedResult.getWarnings().end());

        if (vertices.size() > 0) {
          skinnedMesh.data.geometries.push_back({vertices, indices, material});
        }
      } else {
        auto &&result =
            loadStandardMeshAttributes<Vertex>(primitive, i, p, model);

        if (result.hasError()) {
          importData.warnings.push_back(result.getError());
          continue;
        }

        importData.warnings.insert(importData.warnings.end(),
                                   result.getWarnings().begin(),
                                   result.getWarnings().end());

        auto &vertices = result.getData().first;
        auto &indices = result.getData().second;

        if (importData.optimize) {
          optimizeMeshes(vertices, indices);
        }

        if (vertices.size() > 0) {
          mesh.data.geometries.push_back({vertices, indices, material});
        }
      }
    }

    if (isSkinnedMesh && !skinnedMesh.data.geometries.empty()) {
      auto assetName = gltfMesh.name.empty() ? "skinnedmesh" + std::to_string(i)
                                             : gltfMesh.name;

      skinnedMesh.name = targetPath.string() + "/" + assetName;
      skinnedMesh.type = AssetType::SkinnedMesh;

      auto path = assetCache.createSkinnedMeshFromAsset(skinnedMesh);
      auto handle = assetCache.loadSkinnedMeshFromFile(path.getData());
      importData.skinnedMeshes.map.insert_or_assign(i, handle.getData());
    } else if (!mesh.data.geometries.empty()) {
      auto assetName =
          gltfMesh.name.empty() ? "mesh " + std::to_string(i) : gltfMesh.name;

      mesh.name = targetPath.string() + "/" + assetName;
      mesh.type = AssetType::Mesh;

      auto path = assetCache.createMeshFromAsset(mesh);
      auto handle = assetCache.loadMeshFromFile(path.getData());
      importData.meshes.map.insert_or_assign(i, handle.getData());
    }
  }
}

} // namespace liquid::editor
