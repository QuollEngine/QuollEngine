#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "AssetRegistry.h"
#include "DefaultObjects.h"

namespace quoll {

void AssetRegistry::createDefaultObjects() {
  auto cube = mMeshes.allocate({.type = quoll::AssetType::Mesh,
                                .name = "Cube",
                                .uuid = Uuid("quoll::engine/meshes/cube")});
  auto material =
      mMaterials.allocate({.type = quoll::AssetType::Material,
                           .name = "Default material",
                           .uuid = Uuid("quoll::engine/materials/default")});
  auto font = mFonts.allocate({
      .type = quoll::AssetType::Font,
      .name = "Roboto (default)",
      .uuid = Uuid("quoll::engine/fonts/Roboto-Regular"),
  });

  mMeshes.store(cube, default_objects::createCube());
  mMaterials.store(material, default_objects::createDefaultMaterial());
  mFonts.store(font, default_objects::createDefaultFont());

  mDefaultObjects.cube = AssetRef(mMeshes, cube);
  mDefaultObjects.defaultMaterial = AssetRef(mMaterials, material);
  mDefaultObjects.defaultFont = AssetRef(mFonts, font);
}

} // namespace quoll
