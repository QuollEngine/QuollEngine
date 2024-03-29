#include "bindless-editor.glsl"

Buffer(16) SelectedEntity { uint selectedEntity; };

Buffer(16) EntitiesArray { uint entities[]; };

layout(set = 0, binding = 0) uniform DrawParams {
  SelectedEntity selectedEntity;

  Camera camera;

  TransformsArray spriteTransforms;
  EntitiesArray spriteEntities;

  TransformsArray meshTransforms;
  EntitiesArray meshEntities;

  TransformsArray skinnedMeshTransforms;
  EntitiesArray skinnedMeshEntities;
  SkeletonsArray skeletons;

  TransformsArray textTransforms;
  EntitiesArray textEntities;
  GlyphsArray glyphs;
}
uDrawParams;
