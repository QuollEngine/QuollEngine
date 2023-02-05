const float Pi = 3.1415926535897932384626433832795;
const float TwoPi = Pi * 2.0;

vec3 cubeCoordToWorldUnnormalized(ivec3 cubeCoord, float cubemapWidth) {
  vec2 texCoord = vec2(cubeCoord.xy) / cubemapWidth;
  texCoord = texCoord * 2.0 - 1.0;

  switch (cubeCoord.z) {
  case 0: // posx
    return vec3(1.0, -texCoord.y, -texCoord.x);
  case 1: // negx
    return vec3(-1.0, -texCoord.y, texCoord.x);
  case 2: // posy
    return vec3(texCoord.x, 1.0, texCoord.y);
  case 3: // negy
    return vec3(texCoord.x, -1.0, -texCoord.y);
  case 4: // posz
    return vec3(texCoord.x, -texCoord.y, 1.0);
  case 5: // negz
    return vec3(-texCoord.x, -texCoord.y, -1.0);
  }
  return vec3(0.0);
}

vec3 cubeCoordToWorld(ivec3 cubeCoord, float cubemapWidth) {
  return normalize(cubeCoordToWorldUnnormalized(cubeCoord, cubemapWidth));
}

/**
 * @brief Compute Van der Corput radical inverse
 *
 * @param bits Bits
 * @return Radical inverse
 *
 * @ref http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
 */
float radicalInverseVdC(uint bits) {
  bits = (bits << 16u) | (bits >> 16u);
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

// Sample i-th point from Hammersley point set of NumSamples points total.
vec2 sampleHammersley(uint i, uint numSamples) {
  return vec2(float(i) / float(numSamples), radicalInverseVdC(i));
}

vec3 importanceSampleGGX(vec2 u, vec3 normal, float roughness) {
  float alpha = roughness * roughness;

  float cosTheta =
      clamp(sqrt((1.0 - u.y) / (1.0 + (alpha * alpha - 1.0) * u.y)), 0.0, 1.0);
  float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
  float phi = TwoPi * u.x;

  vec3 H = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

  vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 tangent = normalize(cross(up, normal));
  vec3 bitangent = cross(normal, tangent);

  return normalize(tangent * H.x + bitangent * H.y + normal * H.z);
}
