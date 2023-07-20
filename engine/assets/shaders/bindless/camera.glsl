Buffer(64) Camera {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
  vec4 exposure;
};

#define getCamera() uDrawParams.camera
