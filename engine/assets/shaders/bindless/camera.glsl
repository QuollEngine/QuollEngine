Buffer(64) Camera {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
};

#define getCamera() uDrawParams.camera
