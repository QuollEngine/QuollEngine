#pragma once

#include "GLTFImportData.h"

namespace liquidator {

/**
 * @brief Load animations into asset registry
 *
 * Conforms to on GLTF 2.0 spec
 * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * @param importData GLTF import data
 */
void loadAnimations(GLTFImportData &importData);

} // namespace liquidator
