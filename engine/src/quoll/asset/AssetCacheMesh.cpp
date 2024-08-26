#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "InputBinaryStream.h"
#include "OutputBinaryStream.h"

namespace quoll {

Result<Path>
AssetCache::createMeshFromAsset(const AssetData<MeshAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    QuollAssert(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

  auto metaRes = createAssetMeta(asset.type, asset.name, assetPath);
  if (!metaRes.hasData()) {
    return Result<Path>::Error("Cannot create mesh asset: " + asset.name);
  }

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = asset.type;
  header.magic = AssetFileHeader::MagicConstant;
  file.write(header);

  auto numGeometries = static_cast<u32>(asset.data.geometries.size());
  file.write(numGeometries);

  for (auto &geometry : asset.data.geometries) {
    auto numVertices = static_cast<u32>(geometry.positions.size());
    file.write(numVertices);
    file.write(geometry.positions);
    file.write(geometry.normals);
    file.write(geometry.tangents);
    file.write(geometry.texCoords0);
    file.write(geometry.texCoords1);

    if (asset.type == AssetType::SkinnedMesh) {
      file.write(geometry.joints);
      file.write(geometry.weights);
    }

    auto numIndices = static_cast<u32>(geometry.indices.size());
    file.write(numIndices);
    file.write(geometry.indices);
  }

  return Result<Path>::Ok(assetPath);
}

Result<AssetHandle<MeshAsset>>
AssetCache::loadMeshDataFromInputStream(const Path &path, const Uuid &uuid,
                                        const AssetMeta &meta) {
  InputBinaryStream stream(path);
  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant ||
      header.type != AssetType::Mesh && header.type != AssetType::SkinnedMesh) {
    return Result<AssetHandle<MeshAsset>>::Error("Invalid file format");
  }

  std::vector<String> warnings;

  AssetData<MeshAsset> mesh{};
  mesh.name = meta.name;
  mesh.type = meta.type;
  mesh.uuid = uuid;

  u32 numGeometries = 0;
  stream.read(numGeometries);

  mesh.data.geometries.resize(numGeometries);

  for (u32 i = 0; i < numGeometries; ++i) {
    u32 numVertices = 0;
    stream.read(numVertices);

    if (numVertices == 0) {
      return Result<AssetHandle<MeshAsset>>::Error(
          "Mesh geometry has no vertices");
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

    if (mesh.type == AssetType::SkinnedMesh) {
      g.joints.resize(numVertices);
      g.weights.resize(numVertices);
      stream.read(g.joints);
      stream.read(g.weights);
    }

    u32 numIndices = 0;
    stream.read(numIndices);

    if (numIndices == 0) {
      return Result<AssetHandle<MeshAsset>>::Error(
          "Mesh does not have indices");
    }

    mesh.data.geometries.at(i).indices.resize(numIndices);
    stream.read(mesh.data.geometries.at(i).indices);
  }

  return Result<AssetHandle<MeshAsset>>::Ok(mRegistry.add(mesh), warnings);
}

Result<AssetHandle<MeshAsset>> AssetCache::loadMesh(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Mesh && meta.type != AssetType::SkinnedMesh) {
    return Result<AssetHandle<MeshAsset>>::Error(
        "Asset type is not mesh or skinned mesh");
  }

  return loadMeshDataFromInputStream(getPathFromUuid(uuid), uuid, meta);
}

Result<AssetHandle<MeshAsset>> AssetCache::getOrLoadMesh(const Uuid &uuid) {
  if (uuid.isEmpty()) {
    return Result<AssetHandle<MeshAsset>>::Ok(AssetHandle<MeshAsset>());
  }

  auto handle = mRegistry.findHandleByUuid<MeshAsset>(uuid);
  if (handle) {
    return Result<AssetHandle<MeshAsset>>::Ok(handle);
  }

  return loadMesh(uuid);
}

} // namespace quoll
