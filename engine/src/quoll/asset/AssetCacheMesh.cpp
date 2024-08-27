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

  auto numGeometries = asset.data.geometries.size();
  file.write(numGeometries);

  for (auto &geometry : asset.data.geometries) {
    file.write(geometry.positions.size());
    file.write(geometry.positions);
    file.write(geometry.normals.size());
    file.write(geometry.normals);
    file.write(geometry.tangents.size());
    file.write(geometry.tangents);
    file.write(geometry.texCoords0.size());
    file.write(geometry.texCoords0);
    file.write(geometry.texCoords1.size());
    file.write(geometry.texCoords1);
    file.write(geometry.joints.size());
    file.write(geometry.joints);
    file.write(geometry.weights.size());
    file.write(geometry.weights);
    file.write(geometry.indices.size());
    file.write(geometry.indices);
  }

  return assetPath;
}

Result<MeshAsset> AssetCache::loadMeshDataFromInputStream(const Path &path) {
  InputBinaryStream stream(path);
  AssetFileHeader header;
  stream.read(header);
  if (header.magic != AssetFileHeader::MagicConstant ||
      header.type != AssetType::Mesh) {
    return Error("Invalid file format");
  }

  std::vector<String> warnings;

  MeshAsset mesh{};

  u32 numGeometries = 0;
  stream.read(numGeometries);

  mesh.geometries.resize(numGeometries);

  for (u32 i = 0; i < numGeometries; ++i) {
    auto &g = mesh.geometries.at(i);

    u32 size = 0;
    stream.read(size);
    if (size == 0) {
      return Error("Mesh has no vertices");
    }

    g.positions.resize(size);
    stream.read(g.positions);

    stream.read(size);
    g.normals.resize(size);
    stream.read(g.normals);

    stream.read(size);
    g.tangents.resize(size);
    stream.read(g.tangents);

    stream.read(size);
    g.texCoords0.resize(size);
    stream.read(g.texCoords0);

    stream.read(size);
    g.texCoords1.resize(size);
    stream.read(g.texCoords1);

    stream.read(size);
    g.joints.resize(size);
    stream.read(g.joints);

    stream.read(size);
    g.weights.resize(size);
    stream.read(g.weights);

    stream.read(size);
    if (size == 0) {
      return Error("Mesh does not have indices");
    }

    mesh.geometries.at(i).indices.resize(size);
    stream.read(mesh.geometries.at(i).indices);
  }

  return {mesh, warnings};
}

Result<MeshAsset> AssetCache::loadMesh(const Uuid &uuid) {
  auto meta = getAssetMeta(uuid);
  if (meta.type != AssetType::Mesh) {
    return Error("Asset type is not mesh");
  }

  return loadMeshDataFromInputStream(getPathFromUuid(uuid));
}

} // namespace quoll
