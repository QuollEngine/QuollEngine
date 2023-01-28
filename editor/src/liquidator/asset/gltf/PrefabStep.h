#pragma once

#include "GLTFImportData.h"

namespace liquid::editor {

/**
 * @brief Load prefabs into asset registry
 *
 * @param importData GLTF import data
 */
void loadPrefabs(GLTFImportData &importData);

} // namespace liquid::editor
