#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 inModelPosition;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec2 inTextureCoord[2];
layout(location = 4) in vec3 inNormal;
layout(location = 5) in float inTangentHand;
layout(location = 6) in mat3 inTBN;
layout(location = 9) in mat4 inModelMatrix;

layout(location = 0) out vec4 outColor;

layout(set = 2, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
}
uCameraData;

struct LightData {
  vec4 color;
  vec4 direction;
  uvec4 type;
  mat4 lightSpaceMatrix;
};

layout(std140, set = 2, binding = 1) uniform SceneData {
  LightData lights[16];
  uvec4 numLights;
  uvec4 hasIBL;
}
uSceneData;

layout(set = 2, binding = 2) uniform sampler2DArray uShadowmap;
layout(set = 2, binding = 3) uniform samplerCube uIblMaps[2];
layout(set = 2, binding = 4) uniform sampler2D uBrdfLUT;

layout(std140, set = 3, binding = 0) uniform MaterialDataRaw {
  int baseColorTexture[1];
  int baseColorTextureCoord[1];
  vec4 baseColorFactor;
  int metallicRoughnessTexture[1];
  int metallicRoughnessTextureCoord[1];
  float metallicFactor[1];
  float roughnessFactor[1];
  int normalTexture[1];
  int normalTextureCoord[1];
  float normalScale[1];
  int occlusionTexture[1];
  int occlusionTextureCoord[1];
  float occlusionStrength[1];
  int emissiveTexture[1];
  int emissiveTextureCoord[1];
  vec3 emissiveFactor;
}
uMaterialDataRaw;

struct MaterialData {
  int baseColorTexture;
  int baseColorTextureCoord;
  vec4 baseColorFactor;
  int metallicRoughnessTexture;
  int metallicRoughnessTextureCoord;
  float metallicFactor;
  float roughnessFactor;
  int normalTexture;
  int normalTextureCoord;
  float normalScale;
  int occlusionTexture;
  int occlusionTextureCoord;
  float occlusionStrength;
  int emissiveTexture;
  int emissiveTextureCoord;
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

layout(set = 3, binding = 1) uniform sampler2D uTextures[8];

const float PI = 3.141592653589793;
const mat4 DEPTH_BIAS = mat4(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                             1.0, 0.0, 0.5, 0.5, 0.0, 1.0);

/**
 * sRGB to Linear color
 *
 * @param srgbColor Color in SRGB color space
 * @return Color in linear color space
 */
vec4 srgbToLinear(vec4 srgbColor) {
  vec3 bLess = step(vec3(0.04045), srgbColor.xyz);
  vec3 linearOut =
      mix(srgbColor.xyz / vec3(12.92),
          pow((srgbColor.xyz + vec3(0.055)) / vec3(1.055), vec3(2.4)), bLess);
  return vec4(linearOut, srgbColor.w);
}

/**
 * Linear color to sRGB
 *
 * @param linearColor Color in linear color space
 * @return Color in sRGB color space
 */
vec4 linearToSrgb(vec4 linearColor) {
  bvec3 cutoff = lessThan(linearColor.rgb, vec3(0.0031308));
  vec3 higher =
      vec3(1.055) * pow(linearColor.rgb, vec3(1.0 / 2.4)) - vec3(0.055);
  vec3 lower = linearColor.rgb * vec3(12.92);

  return vec4(mix(higher, lower, cutoff), linearColor.a);
}

/**
 * Lambertian diffuse
 *
 * @param diffuseColor Diffuse color
 * @return Diffuse
 */
vec3 lambertianDiffuse(vec3 diffuseColor) { return diffuseColor / PI; }

/**
 * Schlick Fresnel approximation
 *
 * @param reflectance0 Reflectance at normal incidence
 * @param VdotH Dot product of V and H
 * @return Fresnel
 */
vec3 schlickFresnel(vec3 reflectance0, float VdotH) {
  return reflectance0 + (1.0 - reflectance0) * pow(1 - VdotH, 5);
}

/**
 * GGX/Trowbridge-Reitz normal distribution
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
 * Schlick Specular Geometric Attenuation
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
 * Get normals from world position
 *
 * Checking for tangent existence is based on
 * w attribute of tangent vector, which determines
 * handedness of tangent. This value can only be +1
 * and -1. So, if this value is 0, we can identify that
 * tangent does not exist for the vertex.
 *
 * @return Normal
 */
vec3 getNormal() {
  mat3 tbn = inTBN;
  if (inTangentHand == 0) {
    vec3 posDx = dFdx(inWorldPosition);
    vec3 posDy = dFdy(inWorldPosition);
    vec3 texDx =
        dFdx(vec3(inTextureCoord[uMaterialData.normalTextureCoord], 0.0));
    vec3 texDy =
        dFdy(vec3(inTextureCoord[uMaterialData.normalTextureCoord], 0.0));

    vec3 N = normalize(inNormal);
    vec3 T = normalize(posDx * texDy.t - posDy * texDx.t);
    vec3 B = -normalize(cross(N, T));
    tbn = mat3(T, B, N);
  }

  if (uMaterialData.normalTexture >= 0) {
    vec3 n = texture(uTextures[uMaterialData.normalTexture],
                     inTextureCoord[uMaterialData.normalTextureCoord])
                 .rgb *
             uMaterialData.normalScale;
    return normalize(tbn * n);
  } else {
    return normalize(tbn[2]);
  }
}

struct LightCalculations {
  float NdotL;
  float NdotH;
  float VdotH;
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
LightCalculations getDirectionalLightSurfaceCalculations(LightData light,
                                                         vec3 n, vec3 v) {
  vec3 direction = -light.direction.xyz;
  vec3 l = normalize(direction);
  vec3 h = normalize(v + l);

  return LightCalculations(clamp(dot(n, l), 0.0, 1.0),
                           clamp(dot(n, h), 0.0, 1.0),
                           clamp(dot(v, h), 0.0, 1.0), light.direction.w);
}

/**
 * Calculate shadow factor
 *
 * @param fragLightPosition Light position
 * @param layer Shadowmap layer
 * @return Shadow factor
 */
float calculateShadow(vec4 fragLightPosition, uint layer) {
  vec3 shadowCoords = fragLightPosition.xyz / fragLightPosition.w;

  float closestDepth = texture(uShadowmap, vec3(shadowCoords.xy, layer)).r;
  float currentDepth = shadowCoords.z;

  return closestDepth >= currentDepth - 0.0005 ? 1.0 : 0.0;
}

void main() {
  uint num = uSceneData.numLights.x;

  float metallic = uMaterialData.metallicFactor;
  float roughness = uMaterialData.roughnessFactor;

  if (uMaterialData.metallicRoughnessTexture >= 0) {
    vec3 mrSample =
        texture(uTextures[uMaterialData.metallicRoughnessTexture],
                inTextureCoord[uMaterialData.metallicRoughnessTextureCoord])
            .xyz;
    roughness *= mrSample.g;
    metallic *= mrSample.b;
  }

  roughness = clamp(roughness, 0.0, 1.0);
  metallic = clamp(metallic, 0.0, 1.0);

  vec4 baseColor;
  if (uMaterialData.baseColorTexture >= 0) {
    baseColor =
        srgbToLinear(
            texture(uTextures[uMaterialData.baseColorTexture],
                    inTextureCoord[uMaterialData.baseColorTextureCoord]))
            .xyzw *
        uMaterialData.baseColorFactor;
  } else {
    baseColor = uMaterialData.baseColorFactor;
  }

  const float dielectricSpecular = 0.04;
  vec3 diffuseColor =
      mix(baseColor.rgb * (1 - dielectricSpecular), vec3(0.0), metallic);

  vec3 f0 = mix(vec3(dielectricSpecular), baseColor.rgb, metallic);

  float alpha = roughness * roughness;

  vec3 cameraPos = vec3(uCameraData.view[3]);
  vec3 n = getNormal();
  vec3 v = normalize(cameraPos - inWorldPosition);
  vec3 color = vec3(0.0, 0.0, 0.0);

  float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);

  for (int i = 0; i < num; i++) {
    LightCalculations calc;

    if (uSceneData.lights[i].type.x == 0) {
      calc = getDirectionalLightSurfaceCalculations(uSceneData.lights[i], n, v);
    } else {
      continue;
    }

    vec4 fragLightPosition = DEPTH_BIAS *
                             uSceneData.lights[i].lightSpaceMatrix *
                             inModelMatrix * vec4(inModelPosition.xyz, 1.0);

    const vec4 lightColor = uSceneData.lights[i].color;
    const float lightIntensity = calc.intensity;

    float NdotL = calc.NdotL;
    float NdotH = calc.NdotH;
    float VdotH = calc.VdotH;

    vec3 F = schlickFresnel(f0, VdotH);
    float G = schlickSpecularGeometricAttenuation(alpha, NdotV, NdotL);
    float D = ggxNormalDistribution(alpha, NdotH);

    vec3 diffuseBRDF = (vec3(1.0) - F) * (1 / PI) * diffuseColor;
    vec3 specularBRDF = F * D * G / (4 * NdotL * NdotV);

    float shadow = calculateShadow(fragLightPosition, i);
    color += vec3(lightColor) * shadow * NdotL * lightIntensity *
             (diffuseBRDF + specularBRDF);
  }

  if (uSceneData.hasIBL.x == 1) {
    vec3 reflection = -normalize(reflect(v, n));
    vec3 diffuse = texture(uIblMaps[0], n).rgb * diffuseColor;
    vec3 brdf = texture(uBrdfLUT, vec2(NdotV, 1.0 - roughness)).rgb;
    vec3 specular =
        texture(uIblMaps[1], reflection).rgb * (f0 + brdf.x + brdf.y);

    color += diffuse + specular;
  }

  if (uMaterialData.occlusionTexture >= 0) {
    float ao = texture(uTextures[uMaterialData.occlusionTexture],
                       inTextureCoord[uMaterialData.occlusionTextureCoord])
                   .r;
    color = mix(color, color * ao, uMaterialData.occlusionStrength);
  }

  if (uMaterialData.emissiveTexture >= 0) {
    vec3 emissive =
        srgbToLinear(
            texture(uTextures[uMaterialData.emissiveTexture],
                    inTextureCoord[uMaterialData.emissiveTextureCoord]))
            .rgb *
        uMaterialData.emissiveFactor;
    color += emissive;
  }

  outColor = linearToSrgb(vec4(color, baseColor.a));
}
