#pragma once

#include "quoll/renderer/MaterialAsset.h"
#include "quoll/renderer/MeshAsset.h"
#include "quoll/text/FontAsset.h"
#include "AssetData.h"

namespace quoll::default_objects {

AssetData<MeshAsset> createCube();

AssetData<MaterialAsset> createDefaultMaterial();

AssetData<FontAsset> createDefaultFont();

} // namespace quoll::default_objects
