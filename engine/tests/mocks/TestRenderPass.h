#include "renderer/RenderPass.h"

class TestRenderPass : public liquid::RenderPass {
public:
  TestRenderPass(const liquid::String &name_) : name(name_) {}

  inline const liquid::String &getName() const { return name; }

private:
  liquid::String name;
};
