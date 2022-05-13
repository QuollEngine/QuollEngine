#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetManager.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<Path>
AssetManager::createMeshFromAsset(const AssetData<MeshAsset> &asset) {
  String extension = ".lqmesh";
  Path assetPath = (mAssetsPath / (asset.name + extension)).make_preferred();
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Mesh;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  auto numGeometries = static_cast<uint32_t>(asset.data.geometries.size());
  file.write(numGeometries);

  for (auto &geometry : asset.data.geometries) {
    auto numVertices = static_cast<uint32_t>(geometry.vertices.size());
    file.write(numVertices);
    std::vector<glm::vec3> positions(numVertices);
    std::vector<glm::vec3> normals(numVertices);
    std::vector<glm::vec4> tangents(numVertices);
    std::vector<glm::vec2> texCoords0(numVertices);
    std::vector<glm::vec2> texCoords1(numVertices);

    for (uint32_t i = 0; i < numVertices; ++i) {
      const auto &vertex = geometry.vertices.at(i);
      positions.at(i) = glm::vec3(vertex.x, vertex.y, vertex.z);
      normals.at(i) = glm::vec3(vertex.nx, vertex.ny, vertex.nz);
      tangents.at(i) = glm::vec4(vertex.tx, vertex.ty, vertex.tz, vertex.tw);
      texCoords0.at(i) = glm::vec2(vertex.u0, vertex.v0);
      texCoords1.at(i) = glm::vec2(vertex.u1, vertex.v1);
    }

    file.write(positions);
    file.write(normals);
    file.write(tangents);
    file.write(texCoords0);
    file.write(texCoords1);

    auto numIndices = static_cast<uint32_t>(geometry.indices.size());
    file.write(numIndices);
    file.write(geometry.indices);

    auto materialPath =
        getAssetRelativePath(mRegistry.getMaterials(), geometry.material);
    file.write(materialPath);
  }

  return Result<Path>::Ok(assetPath);
}

Result<MeshAssetHandle>
AssetManager::loadMeshDataFromInputStream(InputBinaryStream &stream,
                                          const Path &filePath) {
  std::vector<String> warnings;

  AssetData<MeshAsset> mesh{};
  mesh.path = filePath;
  mesh.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  mesh.name = mesh.relativePath.string();
  mesh.type = AssetType::Mesh;

  uint32_t numGeometries = 0;
  stream.read(numGeometries);

  mesh.data.geometries.resize(numGeometries);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    stream.read(numVertices);
    mesh.data.geometries.at(i).vertices.resize(numVertices);

    std::vector<glm::vec3> positions(numVertices);
    std::vector<glm::vec3> normals(numVertices);
    std::vector<glm::vec4> tangents(numVertices);
    std::vector<glm::vec2> texCoords0(numVertices);
    std::vector<glm::vec2> texCoords1(numVertices);

    stream.read(positions);
    stream.read(normals);
    stream.read(tangents);
    stream.read(texCoords0);
    stream.read(texCoords1);

    for (uint32_t v = 0; v < numVertices; ++v) {
      auto &vertex = mesh.data.geometries.at(i).vertices.at(v);
      vertex.x = positions.at(v).x;
      vertex.y = positions.at(v).y;
      vertex.z = positions.at(v).z;

      vertex.nx = normals.at(v).x;
      vertex.ny = normals.at(v).y;
      vertex.nz = normals.at(v).z;

      vertex.tx = tangents.at(v).x;
      vertex.ty = tangents.at(v).y;
      vertex.tz = tangents.at(v).z;
      vertex.tw = tangents.at(v).w;

      vertex.u0 = texCoords0.at(v).x;
      vertex.v0 = texCoords0.at(v).y;

      vertex.u1 = texCoords1.at(v).x;
      vertex.v1 = texCoords1.at(v).y;
    }

    uint32_t numIndices = 0;
    stream.read(numIndices);

    mesh.data.geometries.at(i).indices.resize(numIndices);
    stream.read(mesh.data.geometries.at(i).indices);

    String materialPathStr;
    stream.read(materialPathStr);

    const auto &res = getOrLoadMaterialFromPath(materialPathStr);
    if (res.hasData()) {
      mesh.data.geometries.at(i).material = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back("Mesh does not have material");
    }
  }

  return Result<MeshAssetHandle>::Ok(mRegistry.getMeshes().addAsset(mesh),
                                     warnings);
}

Result<MeshAssetHandle> AssetManager::loadMeshFromFile(const Path &filePath) {
  InputBinaryStream stream(filePath);

  const auto &result = checkAssetFile(stream, filePath, AssetType::Mesh);
  if (result.hasError()) {
    return Result<MeshAssetHandle>::Error(result.getError());
  }

  return loadMeshDataFromInputStream(stream, filePath);
}

Result<Path> AssetManager::createSkinnedMeshFromAsset(
    const AssetData<SkinnedMeshAsset> &asset) {

  String extension = ".lqmesh";
  Path assetPath = (mAssetsPath / (asset.name + extension)).make_preferred();
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::SkinnedMesh;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  auto numGeometries = static_cast<uint32_t>(asset.data.geometries.size());
  file.write(numGeometries);

  for (auto &geometry : asset.data.geometries) {
    auto numVertices = static_cast<uint32_t>(geometry.vertices.size());
    file.write(numVertices);
    std::vector<glm::vec3> positions(numVertices);
    std::vector<glm::vec3> normals(numVertices);
    std::vector<glm::vec4> tangents(numVertices);
    std::vector<glm::vec2> texCoords0(numVertices);
    std::vector<glm::vec2> texCoords1(numVertices);
    std::vector<glm::uvec4> joints(numVertices);
    std::vector<glm::vec4> weights(numVertices);

    for (uint32_t i = 0; i < numVertices; ++i) {
      const auto &vertex = geometry.vertices.at(i);
      positions.at(i) = glm::vec3(vertex.x, vertex.y, vertex.z);
      normals.at(i) = glm::vec3(vertex.nx, vertex.ny, vertex.nz);
      tangents.at(i) = glm::vec4(vertex.tx, vertex.ty, vertex.tz, vertex.tw);
      texCoords0.at(i) = glm::vec2(vertex.u0, vertex.v0);
      texCoords1.at(i) = glm::vec2(vertex.u1, vertex.v1);
      joints.at(i) = glm::uvec4(vertex.j0, vertex.j1, vertex.j2, vertex.j3);
      weights.at(i) = glm::vec4(vertex.w0, vertex.w1, vertex.w2, vertex.w3);
    }

    file.write(positions);
    file.write(normals);
    file.write(tangents);
    file.write(texCoords0);
    file.write(texCoords1);
    file.write(joints);
    file.write(weights);

    auto numIndices = static_cast<uint32_t>(geometry.indices.size());
    file.write(numIndices);
    file.write(geometry.indices);

    auto materialPath =
        getAssetRelativePath(mRegistry.getMaterials(), geometry.material);
    file.write(materialPath);
  }

  return Result<Path>::Ok(assetPath);
}

Result<SkinnedMeshAssetHandle>
AssetManager::loadSkinnedMeshDataFromInputStream(InputBinaryStream &stream,
                                                 const Path &filePath) {
  std::vector<String> warnings;

  AssetData<SkinnedMeshAsset> mesh{};
  mesh.path = filePath;
  mesh.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  mesh.name = mesh.relativePath.string();
  mesh.type = AssetType::Material;

  uint32_t numGeometries = 0;
  stream.read(numGeometries);

  mesh.data.geometries.resize(numGeometries);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    stream.read(numVertices);
    mesh.data.geometries.at(i).vertices.resize(numVertices);

    std::vector<glm::vec3> positions(numVertices);
    std::vector<glm::vec3> normals(numVertices);
    std::vector<glm::vec4> tangents(numVertices);
    std::vector<glm::vec2> texCoords0(numVertices);
    std::vector<glm::vec2> texCoords1(numVertices);
    std::vector<glm::uvec4> joints(numVertices);
    std::vector<glm::vec4> weights(numVertices);

    stream.read(positions);
    stream.read(normals);
    stream.read(tangents);
    stream.read(texCoords0);
    stream.read(texCoords1);
    stream.read(joints);
    stream.read(weights);

    for (uint32_t v = 0; v < numVertices; ++v) {
      auto &vertex = mesh.data.geometries.at(i).vertices.at(v);
      vertex.x = positions.at(v).x;
      vertex.y = positions.at(v).y;
      vertex.z = positions.at(v).z;

      vertex.nx = normals.at(v).x;
      vertex.ny = normals.at(v).y;
      vertex.nz = normals.at(v).z;

      vertex.tx = tangents.at(v).x;
      vertex.ty = tangents.at(v).y;
      vertex.tz = tangents.at(v).z;
      vertex.tw = tangents.at(v).w;

      vertex.u0 = texCoords0.at(v).x;
      vertex.v0 = texCoords0.at(v).y;

      vertex.u1 = texCoords1.at(v).x;
      vertex.v1 = texCoords1.at(v).y;

      vertex.j0 = joints.at(v).x;
      vertex.j1 = joints.at(v).y;
      vertex.j2 = joints.at(v).z;
      vertex.j3 = joints.at(v).w;

      vertex.w0 = weights.at(v).x;
      vertex.w1 = weights.at(v).y;
      vertex.w2 = weights.at(v).z;
      vertex.w3 = weights.at(v).w;
    }

    uint32_t numIndices = 0;
    stream.read(numIndices);

    mesh.data.geometries.at(i).indices.resize(numIndices);
    stream.read(mesh.data.geometries.at(i).indices);

    String materialPathStr;
    stream.read(materialPathStr);
    const auto &res = getOrLoadMaterialFromPath(materialPathStr);
    if (res.hasData()) {
      mesh.data.geometries.at(i).material = res.getData();
      warnings.insert(warnings.end(), res.getWarnings().begin(),
                      res.getWarnings().end());
    } else {
      warnings.push_back(res.getError());
    }
  }

  return Result<SkinnedMeshAssetHandle>::Ok(
      mRegistry.getSkinnedMeshes().addAsset(mesh), warnings);
}

Result<SkinnedMeshAssetHandle>
AssetManager::loadSkinnedMeshFromFile(const Path &filePath) {
  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::SkinnedMesh);
  if (header.hasError()) {
    return Result<SkinnedMeshAssetHandle>::Error(header.getError());
  }

  return loadSkinnedMeshDataFromInputStream(stream, filePath);
}

Result<MeshAssetHandle>
AssetManager::getOrLoadMeshFromPath(StringView relativePath) {
  if (relativePath.empty()) {
    return Result<MeshAssetHandle>::Ok(MeshAssetHandle::Invalid);
  }

  Path fullPath = (mAssetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : mRegistry.getMeshes().getAssets()) {
    if (asset.path == fullPath) {
      return Result<MeshAssetHandle>::Ok(handle);
    }
  }

  return loadMeshFromFile(fullPath);
}

Result<SkinnedMeshAssetHandle>
AssetManager::getOrLoadSkinnedMeshFromPath(StringView relativePath) {
  if (relativePath.empty()) {
    return Result<SkinnedMeshAssetHandle>::Ok(SkinnedMeshAssetHandle::Invalid);
  }

  Path fullPath = (mAssetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : mRegistry.getSkinnedMeshes().getAssets()) {
    if (asset.path == fullPath) {
      return Result<SkinnedMeshAssetHandle>::Ok(handle);
    }
  }

  return loadSkinnedMeshFromFile(fullPath);
}

} // namespace liquid
