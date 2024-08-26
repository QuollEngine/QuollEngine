#include "quoll/core/Base.h"
#include "quoll/core/Version.h"
#include "AssetCache.h"
#include "AssetFileHeader.h"
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

  auto metaRes = createAssetMeta(AssetType::Mesh, asset.name, assetPath);
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
  header.name = asset.name;
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
AssetCache::loadMeshDataFromInputStream(InputBinaryStream &stream,
                                        const Path &filePath,
                                        const AssetFileHeader &header) {
  std::vector<String> warnings;

  AssetData<MeshAsset> mesh{};
  mesh.name = header.name;
  mesh.path = filePath;
  mesh.type = header.type;
  mesh.uuid = Uuid(filePath.stem().string());

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
  auto filePath = getPathFromUuid(uuid);

  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::None);
  if (header.hasError()) {
    return Result<AssetHandle<MeshAsset>>::Error(header.getError());
  }

  if (header.getData().type != AssetType::Mesh &&
      header.getData().type != AssetType::SkinnedMesh) {
    return Result<AssetHandle<MeshAsset>>::Error(
        "Opened file is not a quoll asset: " + filePath.string());
  }

  return loadMeshDataFromInputStream(stream, filePath, header.getData());
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
