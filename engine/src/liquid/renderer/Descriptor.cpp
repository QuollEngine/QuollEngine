#include "liquid/core/Base.h"
#include "Descriptor.h"

namespace liquid {

Descriptor &Descriptor::bind(uint32_t binding,
                             const std::vector<TextureHandle> &textures,
                             DescriptorType type) {
  LIQUID_ASSERT(type == DescriptorType::CombinedImageSampler,
                "Descriptor type for binding " + std::to_string(binding) +
                    " must be combined image sampler");
  bindings.insert({binding, DescriptorBinding{type, textures}});
  std::stringstream ss;
  ss << "b:" << binding << ";t:" << static_cast<uint32_t>(type) << ";";
  for (auto &x : textures) {
    ss << "d:" << x << ";";
  }
  ss << "|";
  hashCode += ss.str();
  return *this;
}

Descriptor &Descriptor::bind(uint32_t binding, BufferHandle buffer,
                             DescriptorType type) {
  LIQUID_ASSERT(type == DescriptorType::UniformBuffer,
                "Descriptor type for binding " + std::to_string(binding) +
                    " must be uniform buffer");

  bindings.insert({binding, DescriptorBinding{type, buffer}});
  std::stringstream ss;
  ss << "b:" << binding << ";t:" << static_cast<uint32_t>(type)
     << ";d:" << buffer << "|";
  hashCode += ss.str();
  return *this;
}

} // namespace liquid
