#pragma once

#include "AssetData.h"
#include "FontAsset.h"
#include "MaterialAsset.h"
#include "MeshAsset.h"

namespace quoll::default_objects {

AssetData<MeshAsset> createCube();

AssetData<MaterialAsset> createDefaultMaterial();

AssetData<FontAsset> createDefaultFont();

} // namespace quoll::default_objects
