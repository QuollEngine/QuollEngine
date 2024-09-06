#pragma once

#include "quoll/renderer/MaterialAsset.h"
#include "quoll/renderer/MeshAsset.h"
#include "quoll/text/FontAsset.h"
#include "AssetData.h"

namespace quoll::default_objects {

MeshAsset createCube();

MaterialAsset createDefaultMaterial();

FontAsset createDefaultFont();

} // namespace quoll::default_objects
