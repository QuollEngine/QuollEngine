#pragma once

class TestTextureResourceBinder : public liquid::TextureBinder {
public:
  enum Types { Texture2D, Texture3D, TextureCubemap, TextureFramebuffer };

public:
  Types type;
  uint32_t width, height;
  unsigned char *data = nullptr;
  uint32_t format;
};
