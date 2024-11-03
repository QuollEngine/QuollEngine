#include "quoll/core/Base.h"
#include "mikktspace/MikktspaceAdapter.h"
#include "Buffer.h"
#include "MeshStep.h"
#include <meshoptimizer.h>

namespace quoll::editor {

void generateNormals(const std::vector<glm::vec3> &positions,
                     const std::vector<u32> &indices,
                     std::vector<glm::vec3> &normals) {
  for (auto &n : normals) {
    n.x = 0.0f;
    n.y = 0.0f;
    n.z = 0.0f;
  }

  for (usize i = 0; i < indices.size(); i += 3) {
    auto i0 = indices.at(i);
    auto i1 = indices.at(i + 1);
    auto i2 = indices.at(i + 2);

    const auto &p0 = positions.at(i0);
    const auto &p1 = positions.at(i1);
    const auto &p2 = positions.at(i2);

    auto &n0 = normals.at(i0);
    auto &n1 = normals.at(i1);
    auto &n2 = normals.at(i2);

    auto edge1 = p0 - p1;
    auto edge2 = p0 - p2;

    auto normal = glm::cross(edge1, edge2);

    n0.x += normal.x;
    n0.y += normal.y;
    n0.z += normal.z;

    n1.x += normal.x;
    n1.y += normal.y;
    n1.z += normal.z;

    n2.x += normal.x;
    n2.y += normal.y;
    n2.z += normal.z;
  }

  for (auto &n : normals) {
    auto normalized = glm::normalize(glm::vec3(n.x, n.y, n.z));
    n.x = normalized.x;
    n.y = normalized.y;
    n.z = normalized.z;
  }
}

void generateTangents(const std::vector<glm::vec3> &vertices,
                      const std::vector<glm::vec3> &normals,
                      const std::vector<glm::vec2> &texCoords,
                      const std::vector<u32> &indices,
                      std::vector<glm::vec4> &tangents) {
  MikktspaceAdapter adapter;

  adapter.generate(vertices, normals, texCoords, indices, tangents);
}

void optimizeMeshes(BaseGeometryAsset &g) {
  static constexpr f32 OverdrawThreshold = 1.05f;

  const usize vertexSize = g.positions.size();

  meshopt_optimizeVertexCache(g.indices.data(), g.indices.data(),
                              g.indices.size(), vertexSize);
  meshopt_optimizeOverdraw(g.indices.data(), g.indices.data(), g.indices.size(),
                           &g.positions.at(0).x, vertexSize, sizeof(glm::vec3),
                           OverdrawThreshold);

  std::vector<u32> remap(g.positions.size());
  meshopt_optimizeVertexFetchRemap(remap.data(), g.indices.data(),
                                   g.indices.size(), g.positions.size());

  meshopt_remapIndexBuffer(g.indices.data(), g.indices.data(), g.indices.size(),
                           remap.data());
  meshopt_remapVertexBuffer(g.positions.data(), g.positions.data(), vertexSize,
                            sizeof(glm::vec3), remap.data());
  meshopt_remapVertexBuffer(g.normals.data(), g.normals.data(), vertexSize,
                            sizeof(glm::vec3), remap.data());
  meshopt_remapVertexBuffer(g.tangents.data(), g.tangents.data(), vertexSize,
                            sizeof(glm::vec4), remap.data());
  meshopt_remapVertexBuffer(g.texCoords0.data(), g.texCoords0.data(),
                            vertexSize, sizeof(glm::vec2), remap.data());
  meshopt_remapVertexBuffer(g.texCoords1.data(), g.texCoords1.data(),
                            vertexSize, sizeof(glm::vec2), remap.data());

  if (!g.joints.empty()) {
    meshopt_remapVertexBuffer(g.joints.data(), g.joints.data(), vertexSize,
                              sizeof(glm::uvec4), remap.data());
    meshopt_remapVertexBuffer(g.weights.data(), g.weights.data(), vertexSize,
                              sizeof(glm::vec4), remap.data());
  }
}

Result<void> loadStandardMeshAttributes(const String &primitiveName,
                                        const tinygltf::Primitive &primitive,
                                        const tinygltf::Model &model,
                                        BaseGeometryAsset &geometry) {
  auto &indices = geometry.indices;
  std::vector<String> warnings;

  if (primitive.attributes.find("POSITION") == primitive.attributes.end()) {
    return Error(primitiveName +
                 " skipped because it does not have position attribute");
  }

  auto &&positionMeta =
      getBufferMetaForAccessor(model, primitive.attributes.at("POSITION"));
  const usize vertexSize = positionMeta.accessor.count;

  // According to spec, position attribute can only be vec3<f32>
  if (positionMeta.accessor.type == TINYGLTF_TYPE_VEC3 &&
      positionMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
    geometry.positions.resize(vertexSize);
    geometry.normals.resize(vertexSize);
    geometry.tangents.resize(vertexSize);
    geometry.texCoords0.resize(vertexSize);
    geometry.texCoords1.resize(vertexSize);

    auto *data = reinterpret_cast<const glm::vec3 *>(positionMeta.rawData);
    for (usize i = 0; i < vertexSize; ++i) {
      geometry.positions.at(i) = data[i];
    }
  } else {
    return Error(primitiveName +
                 " skipped because it has invalid position format");
  }

  if (primitive.indices >= 0) {
    auto &&indexMeta = getBufferMetaForAccessor(model, primitive.indices);
    indices.resize(indexMeta.accessor.count);
    if (indexMeta.accessor.componentType ==
            TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT &&
        indexMeta.accessor.type == TINYGLTF_TYPE_SCALAR) {
      const auto *data = reinterpret_cast<const u32 *>(indexMeta.rawData);
      for (usize i = 0; i < indexMeta.accessor.count; ++i) {
        indices[i] = data[i];
      }
    } else if (indexMeta.accessor.componentType ==
                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT &&
               indexMeta.accessor.type == TINYGLTF_TYPE_SCALAR) {
      const auto *data = reinterpret_cast<const u16 *>(indexMeta.rawData);
      for (usize i = 0; i < indexMeta.accessor.count; ++i) {
        indices[i] = data[i];
      }
    } else if (indexMeta.accessor.componentType ==
                   TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE &&
               indexMeta.accessor.type == TINYGLTF_TYPE_SCALAR) {
      const auto *data = reinterpret_cast<const u8 *>(indexMeta.rawData);
      for (usize i = 0; i < indexMeta.accessor.count; ++i) {
        indices[i] = data[i];
      }
    } else {
      return Error(primitiveName +
                   " skipped because it has invalid index format");
    }
  } else {
    indices.resize(geometry.positions.size());
    for (usize i = 0; i < geometry.positions.size(); ++i) {
      indices.at(i) = static_cast<u32>(i);
    }
  }

  // According to spec, normal attribute can only be vec3<f32> and
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
      for (usize i = 0; i < vertexSize; ++i) {
        geometry.normals.at(i) = data[i];
      }
    }
  }

  if (!validNormals) {
    warnings.push_back("Normals for " + primitiveName + " are generated");
    generateNormals(geometry.positions, geometry.indices, geometry.normals);
  }

  if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
    auto &&uvMeta =
        getBufferMetaForAccessor(model, primitive.attributes.at("TEXCOORD_0"));

    // According to spec, UV data is always in vec2 format
    // and all attributes of a primitive must have
    // the same number of items
    if (uvMeta.accessor.type == TINYGLTF_TYPE_VEC2 &&
        uvMeta.accessor.count == vertexSize) {
      // UV coordinates can be f32, ubyte, and ushort
      if (uvMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
        auto *data = reinterpret_cast<const glm::vec2 *>(uvMeta.rawData);
        for (usize i = 0; i < vertexSize; ++i) {
          geometry.texCoords0.at(i) = data[i];
        }
      } else if (uvMeta.accessor.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ||
                 uvMeta.accessor.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        // TODO: Convert integer coordinates to f32
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
      // UV coordinates can be f32, ubyte, and ushort
      if (uvMeta.accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
        auto *data = reinterpret_cast<const glm::vec2 *>(uvMeta.rawData);
        for (usize i = 0; i < vertexSize; ++i) {
          geometry.texCoords1.at(i) = data[i];
        }
      } else if (uvMeta.accessor.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ||
                 uvMeta.accessor.componentType ==
                     TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        // TODO: Convert integer coordinates to f32
        warnings.push_back("Integer based texture coordinates are not "
                           "supported for TEXCOORD1");
      }
    }
  }

  // According to spec, tangent attribute can only be vec4<f32> and
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
      for (usize i = 0; i < vertexSize; ++i) {
        geometry.tangents.at(i) = data[i];
      }
    }
  }

  if (!validTangents) {
    generateTangents(geometry.positions, geometry.normals, geometry.texCoords0,
                     geometry.indices, geometry.tangents);
  }

  return {warnings};
}

Result<void> loadSkinnedMeshAttributes(const String &primitiveName,
                                       const tinygltf::Primitive &primitive,
                                       const tinygltf::Model &model,
                                       BaseGeometryAsset &geometry) {

  std::vector<String> warnings;

  geometry.joints.resize(geometry.positions.size());
  geometry.weights.resize(geometry.positions.size());

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

        for (usize i = 0; i < jointMeta.accessor.count; ++i) {
          geometry.joints.at(i) = data[i];
        }
      } else if (jointMeta.accessor.componentType ==
                 TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        const auto *data =
            reinterpret_cast<const glm::u16vec4 *>(jointMeta.rawData);
        for (usize i = 0; i < jointMeta.accessor.count; ++i) {
          geometry.joints.at(i) = data[i];
        }
      } else {
        validJoints = false;
      }
    }
  }

  if (!validJoints) {
    warnings.push_back(primitiveName + " joints attribute is invalid");
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

      for (usize i = 0; i < weightsMeta.accessor.count; ++i) {
        geometry.weights.at(i) = data[i];
      }
    }
  }

  if (!validWeights) {
    warnings.push_back(primitiveName + " weights attribute is invalid");
  }

  return {warnings};
}

void loadMeshes(GLTFImportData &importData) {
  auto &assetCache = importData.assetCache;
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

    auto assetName =
        gltfMesh.name.empty() ? "mesh-" + std::to_string(i) : gltfMesh.name;
    assetName += ".mesh";

    std::vector<AssetRef<MaterialAsset>> materials;
    AssetData<MeshAsset> mesh;

    for (usize p = 0; p < gltfMesh.primitives.size(); ++p) {
      auto primitiveName = assetName + ", primitive #" + std::to_string(p);

      const auto &primitive = gltfMesh.primitives.at(p);

      auto material = primitive.material >= 0
                          ? importData.materials.map.at(primitive.material)
                          : importData.assetCache.getRegistry()
                                .getDefaultObjects()
                                .defaultMaterial;

      BaseGeometryAsset geometry;
      auto &&result =
          loadStandardMeshAttributes(primitiveName, primitive, model, geometry);

      if (!result) {
        importData.warnings.push_back(result.error());
        continue;
      }

      importData.warnings.insert(importData.warnings.end(),
                                 result.warnings().begin(),
                                 result.warnings().end());

      if (isSkinnedMesh) {
        auto &&skinnedResult = loadSkinnedMeshAttributes(
            primitiveName, primitive, model, geometry);

        if (!skinnedResult) {
          importData.warnings.push_back(result.error());
          continue;
        }

        if (importData.optimize) {
          optimizeMeshes(geometry);
        }

        importData.warnings.insert(importData.warnings.end(),
                                   skinnedResult.warnings().begin(),
                                   skinnedResult.warnings().end());
      }

      if (importData.optimize) {
        optimizeMeshes(geometry);
      }

      if (geometry.positions.size() > 0) {
        mesh.data.geometries.push_back(geometry);
        materials.push_back(material);
      }
    }

    if (mesh.data.geometries.empty()) {
      // Do nothing if there are no meshes in GLTF file
      return;
    }

    mesh.type = AssetType::Mesh;
    mesh.name = getGLTFAssetName(importData, assetName);
    mesh.uuid = getOrCreateGLTFUuid(importData, assetName);

    auto path = assetCache.createFromData(mesh);

    auto ref = assetCache.request<MeshAsset>(mesh.uuid);
    if (!ref) {
      importData.warnings.push_back(ref.error());
      return;
    }

    auto res = assetCache.waitForIdle(mesh.uuid);
    if (!res) {
      importData.warnings.push_back(res.error());
      return;
    }

    importData.meshes.map.insert_or_assign(i, ref.data());
    importData.meshMaterials.insert_or_assign(ref.data().handle(), materials);
    importData.outputUuids.insert_or_assign(assetName, mesh.uuid);
  }
}

} // namespace quoll::editor
