#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNearPoint;
layout(location = 1) in vec3 inFarPoint;
layout(location = 2) in mat4 inViewProj;

layout(location = 0) out vec4 outColor;

layout(std140, set = 1, binding = 0) uniform GridData { uvec4 gridLines; }
uGridData;

const vec3 LINE_COLOR = vec3(0.5, 0.5, 0.5);
const vec3 X_AXIS_COLOR = vec3(1.0, 0.0, 0.0);
const vec3 Z_AXIS_COLOR = vec3(0.0, 0.0, 1.0);

void main() {
  // y(t) = near.y + t * (far.y - near.y)
  // y(t) = 0 => t = -near.y / (inFarPoint.y - inNearPoint.y);
  float t = -inNearPoint.y / (inFarPoint.y - inNearPoint.y);

  // pos(t) = near + t * (far - near)
  vec3 fragPos = inNearPoint + t * (inFarPoint - inNearPoint);

  // Default color
  outColor = vec4(0.0);

  // Compute lines
  if (t > 0) {
    vec2 gridCoord = fragPos.xz;

    vec2 derivative = fwidth(gridCoord);
    vec2 grid = abs(fract(gridCoord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);

    float lineAlpha = 1.0 - min(line, 1.0);
    vec2 center = min(derivative, vec2(1.0, 1.0)) * 0.5;

    float nonAxisAlpha = min(lineAlpha, 0.3);

    vec4 color = vec4(0.0);
    // Set non-axis line color
    if (uGridData.gridLines.x == 1) {
      color = vec4(LINE_COLOR, nonAxisAlpha);
    }

    // Set Z axis color if X position is at zero
    if (uGridData.gridLines.y == 1 && fragPos.x > -center.x &&
        fragPos.x < center.x) {
      color = vec4(Z_AXIS_COLOR, lineAlpha);
    }

    // Set X axis color if Z position is at zero
    if (uGridData.gridLines.y == 1 && fragPos.z > -center.y &&
        fragPos.z < center.y) {
      color = vec4(X_AXIS_COLOR, lineAlpha);
    }

    outColor = color;

    if (outColor.w == 0.0) {
      discard;
    }
  }

  // Compute depth
  vec4 clipSpacePos = inViewProj * vec4(fragPos, 1.0);
  gl_FragDepth = clipSpacePos.z / clipSpacePos.w;
}
