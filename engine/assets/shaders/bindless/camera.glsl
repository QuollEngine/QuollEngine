struct CameraData {
  mat4 proj;
  mat4 view;
  mat4 viewProj;
};

RegisterUniform(CameraUniform, { CameraData camera; });

#ifdef Bindless
#define getCameraFromReg(register)                                             \
  GetBindlessResourceFromPC(CameraUniform, register).camera
#endif
