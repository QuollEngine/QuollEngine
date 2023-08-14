#ifndef MATERIAL_GLSL
#define MATERIAL_GLSL

/**
 * @brief Single material
 */
Buffer(16) MaterialItem {
  uvec4 baseColorTexture;
  ivec4 baseColorTextureCoord;
  vec4 baseColorFactor;
  uvec4 metallicRoughnessTexture;
  ivec4 metallicRoughnessTextureCoord;
  vec4 metallicFactor;
  vec4 roughnessFactor;
  uvec4 normalTexture;
  ivec4 normalTextureCoord;
  vec4 normalScale;
  uvec4 occlusionTexture;
  ivec4 occlusionTextureCoord;
  vec4 occlusionStrength;
  uvec4 emissiveTexture;
  ivec4 emissiveTextureCoord;
  vec3 emissiveFactor;
};

Buffer(8) MaterialsArray { MaterialItem items[]; };

struct MaterialRangeItem {
  uint start;

  uint end;
};

Buffer(8) MaterialRangeArray { MaterialRangeItem items[]; };

#endif
