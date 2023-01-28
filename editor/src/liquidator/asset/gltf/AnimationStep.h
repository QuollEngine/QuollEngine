#pragma once

#include "GLTFImportData.h"

namespace liquid::editor {

/**
 * @brief Load animations into asset registry
 *
 * Conforms to on GLTF 2.0 spec
 * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * @param importData GLTF import data
 */
void loadAnimations(GLTFImportData &importData);

} // namespace liquid::editor
