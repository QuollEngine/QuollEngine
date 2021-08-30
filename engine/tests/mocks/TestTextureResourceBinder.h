#pragma once

class TestTextureResourceBinder : public liquid::TextureBinder {
public:
  enum Types { Texture2D, Texture3D, TextureCubemap, TextureShadowmap };

public:
  Types type;
  uint32_t width, height;
  unsigned char *data = nullptr;
};
