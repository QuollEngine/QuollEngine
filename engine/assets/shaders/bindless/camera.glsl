struct CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
};

RegisterUniform(CameraUniform, { CameraData camera; });

#define getCamera()                                                            \
  GetBindlessResource(CameraUniform, uDrawParams.camera).camera
