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
    return Error("Invalid uuid provided");
  }

  auto assetPath = getPathFromUuid(asset.uuid);

  auto metaRes = createAssetMeta(asset.type, asset.name, assetPath);
  if (!metaRes) {
    return Error("Cannot create mesh asset: " + asset.name);
  }

  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Error("File cannot be opened for writing: " + assetPath.string());
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

  return assetPath;
}

Result<MeshAsset> AssetCache::loadMeshDataFromInputStream(const Path &path,
                                                          AssetType type) {
  InputBinaryStream stream(path);
  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant || header.type != type) {
    return Error("Invalid file format");
  }

  std::vector<String> warnings;

  MeshAsset mesh{};

  u32 numGeometries = 0;
  stream.read(numGeometries);

  mesh.geometries.resize(numGeometries);

  for (u32 i = 0; i < numGeometries; ++i) {
    u32 numVertices = 0;
    stream.read(numVertices);

    if (numVertices == 0) {
      return Error("Mesh geometry has no vertices");
    }

    auto &g = mesh.geometries.at(i);

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

    if (type == AssetType::SkinnedMesh) {
      g.joints.resize(numVertices);
      g.weights.resize(numVertices);
      stream.read(g.joints);
      stream.read(g.weights);
    }

    u32 numIndices = 0;
    stream.read(numIndices);

    if (numIndices == 0) {
      return Error("Mesh does not have indices");
    }

    mesh.geometries.at(i).indices.resize(numIndices);
    stream.read(mesh.geometries.at(i).indices);
  }

  return {mesh, warnings};
}

Result<MeshAsset> AssetCache::loadMesh(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Mesh && meta.type != AssetType::SkinnedMesh) {
    return Error("Asset type is not mesh or skinned mesh");
  }

  return loadMeshDataFromInputStream(getPathFromUuid(uuid), meta.type);
}

} // namespace quoll
