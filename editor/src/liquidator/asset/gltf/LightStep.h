#pragma once

#include "GLTFImportData.h"

namespace liquid::editor {

/**
 * @brief Load lights into asset registry
 *
 * Conforms to on GLTF 2.0 spec
 * https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
 *
 * Conforms to KHR lunctual lights extension
 * https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md
 *
 * @param importData GLTF import data
 */
void loadLights(GLTFImportData &importData);

} // namespace liquid::editor
