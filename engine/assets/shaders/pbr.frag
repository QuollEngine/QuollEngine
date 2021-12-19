#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 modelPosition;
layout(location = 1) in vec3 worldPosition;
layout(location = 2) in vec3 fragColor;
layout(location = 3) in vec2 textureCoord[2];
layout(location = 5) in vec3 normal;
layout(location = 6) in float tangentHand;
layout(location = 7) in mat3 TBN;
layout(location = 10) in mat4 modelMatrix;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewproj;
}
cameraData;

struct LightData {
  vec4 color;
  vec4 direction;
  uvec4 type;
  mat4 lightSpaceMatrix;
};

layout(std140, set = 1, binding = 1) uniform SceneData {
  LightData lights[16];
  uvec4 numLights;
  uvec4 hasIBL;
}
sceneData;

layout(set = 1, binding = 2) uniform sampler2DArray shadowmap;

layout(set = 1, binding = 3) uniform samplerCube iblMaps[2];

layout(set = 1, binding = 4) uniform sampler2D brdfLUT;

layout(std140, set = 2, binding = 0) uniform MaterialDataRaw {
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
materialDataRaw;

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

layout(set = 2, binding = 1) uniform sampler2D tex[8];

MaterialData materialData = MaterialData(
    materialDataRaw.baseColorTexture[0],
    materialDataRaw.baseColorTextureCoord[0], materialDataRaw.baseColorFactor,
    materialDataRaw.metallicRoughnessTexture[0],
    materialDataRaw.metallicRoughnessTextureCoord[0],
    materialDataRaw.metallicFactor[0], materialDataRaw.roughnessFactor[0],
    materialDataRaw.normalTexture[0], materialDataRaw.normalTextureCoord[0],
    materialDataRaw.normalScale[0], materialDataRaw.occlusionTexture[0],
    materialDataRaw.occlusionTextureCoord[0],
    materialDataRaw.occlusionStrength[0], materialDataRaw.emissiveTexture[0],
    materialDataRaw.emissiveTextureCoord[0], materialDataRaw.emissiveFactor);

const float PI = 3.141592653589793;

vec4 SRGBtoLinear(vec4 srgbIn) {
  vec3 bLess = step(vec3(0.04045), srgbIn.xyz);
  vec3 linOut =
      mix(srgbIn.xyz / vec3(12.92),
          pow((srgbIn.xyz + vec3(0.055)) / vec3(1.055), vec3(2.4)), bLess);
  return vec4(linOut, srgbIn.w);
}

vec4 LinearToSRGB(vec4 linearRGB) {
  bvec3 cutoff = lessThan(linearRGB.rgb, vec3(0.0031308));
  vec3 higher = vec3(1.055) * pow(linearRGB.rgb, vec3(1.0 / 2.4)) - vec3(0.055);
  vec3 lower = linearRGB.rgb * vec3(12.92);

  return vec4(mix(higher, lower, cutoff), linearRGB.a);
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
  mat3 tbn = TBN;
  if (tangentHand == 0) {
    vec3 posDx = dFdx(worldPosition);
    vec3 posDy = dFdy(worldPosition);
    vec3 texDx = dFdx(vec3(textureCoord[materialData.normalTextureCoord], 0.0));
    vec3 texDy = dFdy(vec3(textureCoord[materialData.normalTextureCoord], 0.0));

    vec3 N = normalize(normal);
    vec3 T = normalize(posDx * texDy.t - posDy * texDx.t);
    vec3 B = -normalize(cross(N, T));
    tbn = mat3(T, B, N);
  }

  if (materialData.normalTexture >= 0) {
    vec3 n = texture(tex[materialData.normalTexture],
                     textureCoord[materialData.normalTextureCoord])
                 .rgb *
             materialData.normalScale;
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

LightCalculations getDirectionalLightSurfaceCalculations(LightData light,
                                                         vec3 n, vec3 v) {

  vec3 direction = light.direction.xyz;
  vec3 l = normalize(direction);
  vec3 h = normalize(v + l);

  return LightCalculations(clamp(dot(n, l), 0.0, 1.0),
                           clamp(dot(n, h), 0.0, 1.0),
                           clamp(dot(v, h), 0.0, 1.0), light.direction.w);
}

float calculateShadow(vec4 fragLightPosition, vec3 lightDirection, uint layer) {
  vec3 shadowCoords = fragLightPosition.xyz / fragLightPosition.w;

  float closestDepth = texture(shadowmap, vec3(shadowCoords.xy, layer)).r;
  float currentDepth = shadowCoords.z;

  return closestDepth >= currentDepth - 0.0005 ? 1.0 : 0.0;
}

const mat4 depthBias = mat4(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                            1.0, 0.0, 0.5, 0.5, 0.0, 1.0);

void main() {
  uint num = sceneData.numLights.x;

  float metallic = materialData.metallicFactor;
  float roughness = materialData.roughnessFactor;

  if (materialData.metallicRoughnessTexture >= 0) {
    vec3 mrSample =
        texture(tex[materialData.metallicRoughnessTexture],
                textureCoord[materialData.metallicRoughnessTextureCoord])
            .xyz;
    roughness *= mrSample.g;
    metallic *= mrSample.b;
  }

  roughness = clamp(roughness, 0.0, 1.0);
  metallic = clamp(metallic, 0.0, 1.0);

  vec4 baseColor;
  if (materialData.baseColorTexture >= 0) {
    baseColor =
        SRGBtoLinear(texture(tex[materialData.baseColorTexture],
                             textureCoord[materialData.baseColorTextureCoord]))
            .xyzw *
        materialData.baseColorFactor;
  } else {
    baseColor = materialData.baseColorFactor;
  }

  const float dielectricSpecular = 0.04;
  vec3 diffuseColor =
      mix(baseColor.rgb * (1 - dielectricSpecular), vec3(0.0), metallic);

  vec3 f0 = mix(vec3(dielectricSpecular), baseColor.rgb, metallic);

  float alpha = roughness * roughness;

  vec3 cameraPos = vec3(cameraData.view[3]);
  vec3 n = getNormal();
  vec3 v = normalize(cameraPos - worldPosition);
  vec3 color = vec3(0.0, 0.0, 0.0);

  float NdotV = clamp(abs(dot(n, v)), 0.001, 1.0);

  for (int i = 0; i < num; i++) {
    LightCalculations calc;

    if (sceneData.lights[i].type.x == 0) {
      calc = getDirectionalLightSurfaceCalculations(sceneData.lights[i], n, v);
    } else {
      continue;
    }

    vec4 fragLightPosition = depthBias * sceneData.lights[i].lightSpaceMatrix *
                             modelMatrix * vec4(modelPosition.xyz, 1.0);

    const vec4 lightColor = sceneData.lights[i].color;
    const float lightIntensity = calc.intensity;

    float NdotL = calc.NdotL;
    float NdotH = calc.NdotH;
    float VdotH = calc.VdotH;

    vec3 F = schlickFresnel(f0, VdotH);
    float G = schlickSpecularGeometricAttenuation(alpha, NdotV, NdotL);
    float D = ggxNormalDistribution(alpha, NdotH);

    vec3 diffuseBRDF = (vec3(1.0) - F) * (1 / PI) * diffuseColor;
    vec3 specularBRDF = F * D * G / (4 * NdotL * NdotV);

    float shadow = calculateShadow(fragLightPosition,
                                   sceneData.lights[i].direction.xyz, i);
    color += vec3(lightColor) * shadow * NdotL * lightIntensity *
             (diffuseBRDF + specularBRDF);
  }

  if (sceneData.hasIBL.x == 1) {
    vec3 reflection = -normalize(reflect(v, n));
    vec3 diffuse = texture(iblMaps[0], n).rgb * diffuseColor;
    vec3 brdf = texture(brdfLUT, vec2(NdotV, 1.0 - roughness)).rgb;
    vec3 specular =
        texture(iblMaps[1], reflection).rgb * (f0 + brdf.x + brdf.y);

    color += diffuse + specular;
  }

  if (materialData.occlusionTexture >= 0) {
    float ao = texture(tex[materialData.occlusionTexture],
                       textureCoord[materialData.occlusionTextureCoord])
                   .r;
    color = mix(color, color * ao, materialData.occlusionStrength);
  }

  if (materialData.emissiveTexture >= 0) {
    vec3 emissive =
        SRGBtoLinear(texture(tex[materialData.emissiveTexture],
                             textureCoord[materialData.emissiveTextureCoord]))
            .rgb *
        materialData.emissiveFactor;
    color += emissive;
  }

  outColor = LinearToSRGB(vec4(color, baseColor.a));
}
