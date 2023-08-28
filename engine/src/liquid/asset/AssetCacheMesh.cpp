#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetCache.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<Path>
AssetCache::createMeshFromAsset(const AssetData<MeshAsset> &asset) {
  if (asset.uuid.isEmpty()) {
    LIQUID_ASSERT(false, "Invalid uuid provided");
    return Result<Path>::Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

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

    if (asset.type == AssetType::SkinnedMesh) {
      file.write(geometry.joints);
      file.write(geometry.weights);
    }

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
  mesh.type = header.type;
  mesh.uuid = Uuid(filePath.stem().string());

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

    if (mesh.type == AssetType::SkinnedMesh) {
      g.joints.resize(numVertices);
      g.weights.resize(numVertices);
      stream.read(g.joints);
      stream.read(g.weights);
    }

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

Result<MeshAssetHandle> AssetCache::loadMesh(const Uuid &uuid) {
  auto filePath = getPathFromUuid(uuid);

  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::None);
  if (header.hasError()) {
    return Result<MeshAssetHandle>::Error(header.getError());
  }

  if (header.getData().type != AssetType::Mesh &&
      header.getData().type != AssetType::SkinnedMesh) {
    return Result<MeshAssetHandle>::Error(
        "Opened file is not a liquid asset: " + filePath.string());
  }

  return loadMeshDataFromInputStream(stream, filePath, header.getData());
}

Result<MeshAssetHandle> AssetCache::getOrLoadMesh(const Uuid &uuid) {
  if (uuid.isEmpty()) {
    return Result<MeshAssetHandle>::Ok(MeshAssetHandle::Null);
  }

  auto handle = mRegistry.getMeshes().findHandleByUuid(uuid);
  if (handle != MeshAssetHandle::Null) {
    return Result<MeshAssetHandle>::Ok(handle);
  }

  return loadMesh(uuid);
}

} // namespace liquid
