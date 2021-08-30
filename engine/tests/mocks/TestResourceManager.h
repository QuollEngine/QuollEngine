#pragma once

#include "core/Base.h"
#include "renderer/ResourceManager.h"
#include "renderer/MaterialResourceBinder.h"

class TestMaterialResourceBinder : public liquid::MaterialResourceBinder {
public:
  TestMaterialResourceBinder(liquid::Material *material_)
      : material(material_) {}

  liquid::Material *material = nullptr;
};

class TestResourceManager : public liquid::ResourceManager {
public:
  liquid::SharedPtr<liquid::MaterialResourceBinder>
  createMaterialResourceBinder(liquid::Material *material) {
    return std::make_shared<TestMaterialResourceBinder>(material);
  }
};
