#pragma once

#include "core/Base.h"
#include "MaterialResourceBinder.h"

namespace liquid {

class Material;

class ResourceManager {
public:
  /**
   * @brief Create material resource binder
   *
   * @return Material resource binder
   */
  virtual SharedPtr<MaterialResourceBinder>
  createMaterialResourceBinder(Material *material) = 0;
};

} // namespace liquid
