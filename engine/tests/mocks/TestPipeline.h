#include "liquid/renderer/Pipeline.h"

class TestPipeline : public liquid::Pipeline {
public:
  TestPipeline(const liquid::String &name_) : name(name_) {}

  inline const liquid::String &getName() const { return name; }

private:
  liquid::String name;
};
