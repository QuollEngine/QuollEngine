#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetCache.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<Path> AssetCache::createMeshFromAsset(const AssetData<MeshAsset> &asset,
                                             const String &uuid) {
  auto assetPath = createAssetPath(uuid);

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Mesh;
  header.magic = AssetFileHeader::MagicConstant;
  header.name = asset.name;
  file.write(header);

  auto numGeometries = static_cast<uint32_t>(asset.data.geometries.size());
  file.write(numGeometries);

  for (auto &geometry : asset.data.geometries) {
    auto numVertices = static_cast<uint32_t>(geometry.positions.size());
    file.write(numVertices);
    file.write(geometry.positions);
    file.write(geometry.normals);
    file.write(geometry.tangents);
    file.write(geometry.texCoords0);
    file.write(geometry.texCoords1);

    auto numIndices = static_cast<uint32_t>(geometry.indices.size());
    file.write(numIndices);
    file.write(geometry.indices);
  }

  return Result<Path>::Ok(assetPath);
}

Result<MeshAssetHandle>
AssetCache::loadMeshDataFromInputStream(InputBinaryStream &stream,
                                        const Path &filePath,
                                        const AssetFileHeader &header) {
  std::vector<String> warnings;

  AssetData<MeshAsset> mesh{};
  mesh.name = header.name;
  mesh.path = filePath;
  mesh.type = AssetType::Mesh;
  mesh.uuid = filePath.stem().string();

  uint32_t numGeometries = 0;
  stream.read(numGeometries);

  mesh.data.geometries.resize(numGeometries);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    stream.read(numVertices);

    if (numVertices == 0) {
      return Result<MeshAssetHandle>::Error("Mesh geometry has no vertices");
    }

    auto &g = mesh.data.geometries.at(i);

    g.positions.resize(numVertices);
    g.normals.resize(numVertices);
    g.tangents.resize(numVertices);
    g.texCoords0.resize(numVertices);
    g.texCoords1.resize(numVertices);

    stream.read(g.positions);
    stream.read(g.normals);
    stream.read(g.tangents);
    stream.read(g.texCoords0);
    stream.read(g.texCoords1);

    uint32_t numIndices = 0;
    stream.read(numIndices);

    if (numIndices == 0) {
      return Result<MeshAssetHandle>::Error("Mesh does not have indices");
    }

    mesh.data.geometries.at(i).indices.resize(numIndices);
    stream.read(mesh.data.geometries.at(i).indices);
  }

  return Result<MeshAssetHandle>::Ok(mRegistry.getMeshes().addAsset(mesh),
                                     warnings);
}

Result<MeshAssetHandle> AssetCache::loadMeshFromFile(const Path &filePath) {
  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Mesh);
  if (header.hasError()) {
    return Result<MeshAssetHandle>::Error(header.getError());
  }

  return loadMeshDataFromInputStream(stream, filePath, header.getData());
}

Result<Path>
AssetCache::createSkinnedMeshFromAsset(const AssetData<MeshAsset> &asset,
                                       const String &uuid) {
  auto assetPath = createAssetPath(uuid);

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.magic = AssetFileHeader::MagicConstant;
  header.name = asset.name;
  header.type = AssetType::SkinnedMesh;
  file.write(header);

  auto numGeometries = static_cast<uint32_t>(asset.data.geometries.size());
  file.write(numGeometries);

  for (auto &geometry : asset.data.geometries) {
    auto numVertices = static_cast<uint32_t>(geometry.positions.size());
    file.write(numVertices);
    file.write(geometry.positions);
    file.write(geometry.normals);
    file.write(geometry.tangents);
    file.write(geometry.texCoords0);
    file.write(geometry.texCoords1);
    file.write(geometry.joints);
    file.write(geometry.weights);

    auto numIndices = static_cast<uint32_t>(geometry.indices.size());
    file.write(numIndices);
    file.write(geometry.indices);
  }

  return Result<Path>::Ok(assetPath);
}

Result<MeshAssetHandle>
AssetCache::loadSkinnedMeshDataFromInputStream(InputBinaryStream &stream,
                                               const Path &filePath,
                                               const AssetFileHeader &header) {
  std::vector<String> warnings;

  AssetData<MeshAsset> mesh{};
  mesh.name = header.name;
  mesh.path = filePath;
  mesh.type = AssetType::SkinnedMesh;
  mesh.uuid = filePath.stem().string();

  uint32_t numGeometries = 0;
  stream.read(numGeometries);

  mesh.data.geometries.resize(numGeometries);

  for (uint32_t i = 0; i < numGeometries; ++i) {
    uint32_t numVertices = 0;
    stream.read(numVertices);
    if (numVertices == 0) {
      return Result<MeshAssetHandle>::Error(
          "Skinned mesh geometry has no vertices");
    }

    auto &g = mesh.data.geometries.at(i);

    g.positions.resize(numVertices);
    g.normals.resize(numVertices);
    g.tangents.resize(numVertices);
    g.texCoords0.resize(numVertices);
    g.texCoords1.resize(numVertices);
    g.joints.resize(numVertices);
    g.weights.resize(numVertices);

    stream.read(g.positions);
    stream.read(g.normals);
    stream.read(g.tangents);
    stream.read(g.texCoords0);
    stream.read(g.texCoords1);
    stream.read(g.joints);
    stream.read(g.weights);

    uint32_t numIndices = 0;
    stream.read(numIndices);

    if (numIndices == 0) {
      return Result<MeshAssetHandle>::Error(
          "Skinned mesh geometry has no indices");
    }

    mesh.data.geometries.at(i).indices.resize(numIndices);
    stream.read(mesh.data.geometries.at(i).indices);
  }

  return Result<MeshAssetHandle>::Ok(
      mRegistry.getSkinnedMeshes().addAsset(mesh), warnings);
}

Result<MeshAssetHandle>
AssetCache::loadSkinnedMeshFromFile(const Path &filePath) {
  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::SkinnedMesh);
  if (header.hasError()) {
    return Result<MeshAssetHandle>::Error(header.getError());
  }

  return loadSkinnedMeshDataFromInputStream(stream, filePath, header.getData());
}

Result<MeshAssetHandle> AssetCache::getOrLoadMeshFromUuid(const String &uuid) {
  if (uuid.empty()) {
    return Result<MeshAssetHandle>::Ok(MeshAssetHandle::Null);
  }

  auto handle = mRegistry.getMeshes().findHandleByUuid(uuid);
  if (handle != MeshAssetHandle::Null) {
    return Result<MeshAssetHandle>::Ok(handle);
  }

  return loadMeshFromFile(getPathFromUuid(uuid));
}

Result<MeshAssetHandle>
AssetCache::getOrLoadSkinnedMeshFromUuid(const String &uuid) {
  if (uuid.empty()) {
    return Result<MeshAssetHandle>::Ok(MeshAssetHandle::Null);
  }

  auto handle = mRegistry.getSkinnedMeshes().findHandleByUuid(uuid);
  if (handle != MeshAssetHandle::Null) {
    return Result<MeshAssetHandle>::Ok(handle);
  }

  return loadSkinnedMeshFromFile(getPathFromUuid(uuid));
}

} // namespace liquid
