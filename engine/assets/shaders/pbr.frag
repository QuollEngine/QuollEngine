#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec2 inTextureCoord[2];
layout(location = 3) in mat3 inTBN;

layout(location = 0) out vec4 outColor;

#include "bindless/base.glsl"
#include "bindless/camera.glsl"
#include "bindless/scene.glsl"
#include "bindless/shadows.glsl"
#include "bindless/lights.glsl"

layout(set = 3, binding = 0) uniform DrawParams {
  uint meshTransforms;
  uint skinnedMeshTransforms;
  uint skeletons;
  uint camera;
  uint scene;
  uint lights;
  uint shadows;
  uint pad0;
}
uDrawParams;

layout(set = 1, binding = 0) uniform sampler2D uGlobalTextures[];
layout(set = 1, binding = 0) uniform sampler2DArray uTextures2DArray[];
layout(set = 1, binding = 0) uniform samplerCube uTexturesCube[];

layout(std140, set = 2, binding = 0) uniform MaterialDataRaw {
  uint baseColorTexture[1];
  int baseColorTextureCoord[1];
  vec4 baseColorFactor;
  uint metallicRoughnessTexture[1];
  int metallicRoughnessTextureCoord[1];
  float metallicFactor[1];
  float roughnessFactor[1];
  uint normalTexture[1];
  int normalTextureCoord[1];
  float normalScale[1];
  uint occlusionTexture[1];
  int occlusionTextureCoord[1];
  float occlusionStrength[1];
  uint emissiveTexture[1];
  int emissiveTextureCoord[1];
  vec3 emissiveFactor;
}
uMaterialDataRaw;

/**
 * @brief PBR Material data
 */
struct MaterialData {
  /**
   * Index to base color texture
   */
  uint baseColorTexture;

  /**
   * Texture coordinates index
   * for base color texture
   *
   * Only two texture coordinate
   * groups are supported
   */
  int baseColorTextureCoord;

  /**
   * Base color factor
   *
   * If texture is not provided,
   * the factor is used as the
   * base color.
   *
   * If texture is provided
   * this the factor is used
   * to scale the texture
   * color.
   */
  vec4 baseColorFactor;

  /**
   * Index to metallic roughness texture
   */
  uint metallicRoughnessTexture;

  /**
   * Texture coordinates index
   * for metallic roughness texture
   *
   * Only two texture coordinate
   * groups are supported
   */
  int metallicRoughnessTextureCoord;

  /**
   * Metallic factor
   */
  float metallicFactor;

  /**
   * Roughness factor
   */
  float roughnessFactor;

  /**
   * Index to normal texture
   */
  uint normalTexture;

  /**
   * Texture coordinates index
   * for normal texture
   *
   * Only two texture coordinate
   * groups are supported
   */
  int normalTextureCoord;

  /**
   * Normal scale
   */
  float normalScale;

  /**
   * Index to occlusion texture
   */
  uint occlusionTexture;

  /**
   * Texture coordinates index
   * for occlusion texture
   *
   * Only two texture coordinate
   * groups are supported
   */
  int occlusionTextureCoord;

  /**
   * Occlusion strength
   */
  float occlusionStrength;

  /**
   * Index to emissive texture
   */
  uint emissiveTexture;

  /**
   * Texture coordinates index
   * for emissive texture
   *
   * Only two texture coordinate
   * groups are supported
   */
  int emissiveTextureCoord;

  /**
   * Emissive factor
   */
  vec3 emissiveFactor;
};

MaterialData uMaterialData = MaterialData(
    uMaterialDataRaw.baseColorTexture[0],
    uMaterialDataRaw.baseColorTextureCoord[0], uMaterialDataRaw.baseColorFactor,
    uMaterialDataRaw.metallicRoughnessTexture[0],
    uMaterialDataRaw.metallicRoughnessTextureCoord[0],
    uMaterialDataRaw.metallicFactor[0], uMaterialDataRaw.roughnessFactor[0],
    uMaterialDataRaw.normalTexture[0], uMaterialDataRaw.normalTextureCoord[0],
    uMaterialDataRaw.normalScale[0], uMaterialDataRaw.occlusionTexture[0],
    uMaterialDataRaw.occlusionTextureCoord[0],
    uMaterialDataRaw.occlusionStrength[0], uMaterialDataRaw.emissiveTexture[0],
    uMaterialDataRaw.emissiveTextureCoord[0], uMaterialDataRaw.emissiveFactor);

const float PI = 3.141592653589793;
const mat4 DepthBiasMatrix = mat4(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0,
                                  0.0, 1.0, 0.0, 0.5, 0.5, 0.0, 1.0);

#define IrradianceMap uTexturesCube[getScene().textures.x]
#define SpecularMap uTexturesCube[getScene().textures.y]
#define BrdfLut uGlobalTextures[getScene().textures.z]

/**
 * @brief Lambertian diffuse
 *
 * @param diffuseColor Diffuse color
 * @return Diffuse
 */
vec3 lambertianDiffuse(vec3 diffuseColor) { return diffuseColor / PI; }

/**
 * @brief Schlick Fresnel approximation
 *
 * @param reflectance0 Reflectance at normal incidence
 * @param VdotH Dot product of V and H
 * @return Fresnel
 */
vec3 schlickFresnel(vec3 reflectance0, float VdotH) {
  return reflectance0 + (1.0 - reflectance0) * pow(1 - VdotH, 5);
}

/**
 * @brief GGX/Trowbridge-Reitz normal distribution
 *
 * @param roughness Roughness
 * @param NdotH Dot product of N and H
 * @return Normal distribution
 */
float ggxNormalDistribution(float roughness, float NdotH) {
  float alpha2 = pow(roughness, 2);
  return (alpha2 / (PI * pow(pow(NdotH, 2) * (alpha2 - 1) + 1, 2)));
}

/**
 * @brief Schlick Specular Geometric Attenuation
 *
 * @param roughness Roughness
 * @param NdotV Dot product of N and V
 * @param NdotL Dot product of N and L
 * @return Attenuation
 */
float schlickSpecularGeometricAttenuation(float roughness, float NdotV,
                                          float NdotL) {
  float k = pow((roughness + 1), 2) / 8;
  float attenuationL = NdotL / (NdotL * (1 - k) + k);
  float attenuationV = NdotV / (NdotV * (1 - k) + k);

  return attenuationL * attenuationV;
}

/**
 * @brief Get normal
 *
 * If normal texture is provided,
 * calculates normal using
 * geometry tangent, bitangent, and
 * normal
 *
 * @return Normal
 */
vec3 getNormal() {
  if (uMaterialData.normalTexture > 0) {
    vec3 n = texture(uGlobalTextures[uMaterialData.normalTexture],
                     inTextureCoord[uMaterialData.normalTextureCoord])
                 .xyz;
    n = (n * 2.0 - 1.0) * uMaterialData.normalScale;
    return normalize(inTBN * n);
  } else {
    return normalize(inTBN[2]);
  }
}

/**
 * @brief Light calculation results
 */
struct LightCalculations {
  /**
   * Dot product of N and L
   */
  float NdotL;

  /**
   * Dot product of N and H
   */
  float NdotH;

  /**
   * Dot product of V dot H
   */
  float VdotH;

  /**
   * Light intensity
   */
  float intensity;
};

/**
 * Get light surface calculations for directional light
 *
 * @param light Light data
 * @param n Normal
 * @param v View
 * @return Light calculations
 */
LightCalculations getDirectionalLightSurfaceCalculations(LightItem light,
                                                         vec3 n, vec3 v) {
  vec3 direction = -light.data.xyz;
  vec3 l = normalize(direction);
  vec3 h = normalize(v + l);

  return LightCalculations(clamp(dot(n, l), 0.0, 1.0),
                           clamp(dot(n, h), 0.0, 1.0),
                           clamp(dot(v, h), 0.0, 1.0), light.data.w);
}

float calculateShadowFactorFromMap(vec3 shadowCoords, vec2 offset,
                                   uint cascadeIndex) {
  float closestDepth =
      texture(uTextures2DArray[getScene().textures.w],
              vec3(shadowCoords.x + offset.x, 1.0 - shadowCoords.y + offset.y,
                   cascadeIndex))
          .r;
  float currentDepth = shadowCoords.z;

  return closestDepth >= currentDepth - 0.0005 ? 1.0 : 0.0;
}

/**
 * Calculate shadow factor
 *
 * 1.0 => no shadow
 * < 1.0 => has shadow
 *
 * @param item Light item
 * @return Shadow factor
 */
float calculateShadowFactor(LightItem item) {
  if (item.shadowData.x == 0) {
    return 1.0;
  }

  uint cascadeIndex = item.shadowData.y;
  float viewZ = (getCamera().view * vec4(inWorldPosition, 1.0)).z;
  for (uint si = item.shadowData.y;
       si < item.shadowData.y + item.shadowData.z - 1; si++) {
    float splitDepth = getShadowMap(si).shadowData.x;

    if (viewZ < splitDepth) {
      cascadeIndex = si + 1;
    }
  }

  mat4 shadowMatrix = getShadowMap(cascadeIndex).shadowMatrix;
  vec4 fragLightPosition =
      DepthBiasMatrix * shadowMatrix * vec4(inWorldPosition, 1.0);

  vec3 shadowCoords = fragLightPosition.xyz / fragLightPosition.w;

  // Use percentage closer filtering
  if (getShadowMap(cascadeIndex).shadowData.y > 0.5) {
    ivec2 size = textureSize(uTextures2DArray[getScene().textures.w], 0).xy;
    float scale = 0.75;

    float dx = scale / float(size.x);
    float dy = scale / float(size.y);

    float shadowFactor = 0.0;
    uint count = 0;

    for (int x = -1; x <= 1; ++x) {
      for (int y = -1; y <= 1; ++y) {
        shadowFactor += calculateShadowFactorFromMap(
            shadowCoords, vec2(x * dx, y * dy), cascadeIndex);
        count++;
      }
    }

    return shadowFactor / count;
  }

  return calculateShadowFactorFromMap(shadowCoords, vec2(0.0, 0.0),
                                      cascadeIndex);
}

void main() {
  float metallic = uMaterialData.metallicFactor;
  float roughness = uMaterialData.roughnessFactor;

  if (uMaterialData.metallicRoughnessTexture > 0) {
    vec3 mrSample =
        texture(uGlobalTextures[uMaterialData.metallicRoughnessTexture],
                inTextureCoord[uMaterialData.metallicRoughnessTextureCoord])
            .xyz;
    roughness *= mrSample.g;
    metallic *= mrSample.b;
  }

  roughness = clamp(roughness, 0.0, 1.0);
  metallic = clamp(metallic, 0.0, 1.0);

  vec4 baseColor;
  if (uMaterialData.baseColorTexture > 0) {
    baseColor = texture(uGlobalTextures[uMaterialData.baseColorTexture],
                        inTextureCoord[uMaterialData.baseColorTextureCoord])
                    .xyzw *
                uMaterialData.baseColorFactor;
  } else {
    baseColor = uMaterialData.baseColorFactor;
  }

  const float dielectricSpecular = 0.04;
  vec3 diffuseColor =
      mix(baseColor.rgb * (1 - dielectricSpecular), vec3(0.0), metallic);

  vec3 F0 = mix(vec3(dielectricSpecular), baseColor.rgb, metallic);

  float alpha = roughness * roughness;

  vec3 cameraPos = vec3(inverse(getCamera().view)[3]);
  vec3 n = getNormal();
  vec3 v = normalize(cameraPos - inWorldPosition);
  vec3 color = vec3(0.0, 0.0, 0.0);

  float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);

  uint num = getScene().data.x;
  for (uint i = 0; i < num; i++) {
    LightCalculations calc;
    LightItem item = getLight(i);
    calc = getDirectionalLightSurfaceCalculations(item, n, v);

    const vec4 lightColor = item.color;
    const float lightIntensity = calc.intensity;

    float NdotL = calc.NdotL;
    float NdotH = calc.NdotH;
    float VdotH = calc.VdotH;

    if (NdotL < 0.001) {
      continue;
    }

    vec3 F = schlickFresnel(F0, VdotH);
    float G = schlickSpecularGeometricAttenuation(alpha, NdotV, NdotL);
    float D = ggxNormalDistribution(alpha, NdotH);

    vec3 diffuseBRDF = (vec3(1.0) - F) * (1 / PI) * diffuseColor;
    vec3 specularBRDF = F * D * G / (4 * NdotL * NdotV);

    float shadowFactor = calculateShadowFactor(item);

    color += vec3(lightColor) * shadowFactor * NdotL * lightIntensity *
             (diffuseBRDF + specularBRDF);
  }

  if (getScene().data.y == 1) {
    vec2 brdfLut = texture(BrdfLut, vec2(NdotV, roughness)).rg;

    vec3 kS = schlickFresnel(F0, NdotV);
    vec3 kD = (1.0 - kS) * (1.0 - metallic);

    color += getScene().color.rgb * (kD + kS * brdfLut.r + brdfLut.g);
  } else if (getScene().data.y == 2 && getScene().textures.x > 0) {
    vec2 brdfLut = texture(BrdfLut, vec2(NdotV, roughness)).rg;

    vec3 Fr = max(vec3(1.0 - roughness), F0) - F0;
    vec3 kS = F0 + Fr * pow(1 - NdotV, 5.0);
    vec3 FssEss = kS * brdfLut.r + brdfLut.g;
    float lod = roughness * float(textureQueryLevels(SpecularMap) - 1);
    vec3 reflection = normalize(reflect(-v, n));

    float Ems = 1.0 - (brdfLut.r + brdfLut.g);
    vec3 Favg = (F0 + (1.0 - F0) / 21.0);
    vec3 FmsEms = Ems * FssEss * Favg / (1.0 - Favg * Ems);
    vec3 kD = diffuseColor * (1.0 - FssEss + FmsEms);

    vec3 diffuse = textureLod(IrradianceMap, n, 0).rgb * kD;
    vec3 specular = textureLod(SpecularMap, reflection, lod).rgb * FssEss;

    color += diffuse + specular;
  }

  if (uMaterialData.occlusionTexture > 0) {
    float ao = texture(uGlobalTextures[uMaterialData.occlusionTexture],
                       inTextureCoord[uMaterialData.occlusionTextureCoord])
                   .r;
    color = mix(color, color * ao, uMaterialData.occlusionStrength);
  }

  if (uMaterialData.emissiveTexture > 0) {
    vec3 emissive = texture(uGlobalTextures[uMaterialData.emissiveTexture],
                            inTextureCoord[uMaterialData.emissiveTextureCoord])
                        .rgb *
                    uMaterialData.emissiveFactor;
    color += emissive;
  } else {
    color += uMaterialData.emissiveFactor;
  }

  outColor = vec4(color, baseColor.a);
}
