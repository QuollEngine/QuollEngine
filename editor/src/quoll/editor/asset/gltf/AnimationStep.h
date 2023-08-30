#pragma once

#include "GLTFImportData.h"

namespace quoll::editor {

/**
 * @brief Load animations into asset registry
 *
 * Conforms to on GLTF 2.0 spec
 * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * @param importData GLTF import data
 */
void loadAnimations(GLTFImportData &importData);

} // namespace quoll::editor
