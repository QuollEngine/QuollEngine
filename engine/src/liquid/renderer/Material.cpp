#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "Material.h"

namespace liquid {

Material::Material(const std::vector<TextureHandle> &textures_,
                   const std::vector<std::pair<String, Property>> &properties_,
                   experimental::ResourceRegistry &registry_)
    : textures(textures_), registry(registry_) {

  for (size_t i = 0; i < properties_.size(); ++i) {
    auto &prop = properties_[i];
    properties.push_back(prop.second);
    propertyMap.insert({prop.first, i});
  }

  if (!properties.empty()) {
    auto size = updateBufferData();
    uniformBuffer = registry.addBuffer({BufferType::Uniform, size, data});
    descriptor.bind(0, uniformBuffer, DescriptorType::UniformBuffer);
  }

  descriptor.bind(1, textures, DescriptorType::CombinedImageSampler);
}

void Material::updateProperty(const String &name, const Property &value) {
  const auto &it = propertyMap.find(name);
  if (it == propertyMap.end() || (*it).second > properties.size()) {
    LOG_DEBUG("Property \"" << name
                            << "\" does not exist in material. Skipping...");
    return;
  }

  size_t index = (*it).second;

  if (properties.at(index).getType() != value.getType()) {
    LOG_DEBUG("Type of property \""
              << name << "\" does match the type of new property. Skipping...");
    return;
  }

  properties.at(index) = value;
  auto size = updateBufferData();
  registry.updateBuffer(uniformBuffer, {BufferType::Uniform, size, data});
}

size_t Material::updateBufferData() {
  if (data) {
    delete data;
  }

  size_t size = 0;
  size_t idx = 0;

  size_t maxValue = 0;
  for (auto &value : properties) {
    maxValue = maxValue > value.getSize() ? maxValue : value.getSize();
  }

  size = maxValue * properties.size();

  data = new char[size];

  for (auto &value : properties) {
    if (value.getType() == Property::INT32) {
      auto val = value.getValue<int32_t>();
      memcpy(data + idx, &val, maxValue);
    } else if (value.getType() == Property::UINT32) {
      auto val = value.getValue<uint32_t>();
      memcpy(data + idx, &val, maxValue);
    } else if (value.getType() == Property::REAL) {
      auto val = value.getValue<float>();
      memcpy(data + idx, &val, maxValue);
    } else if (value.getType() == Property::VECTOR2) {
      auto &val = value.getValue<glm::vec2>();
      memcpy(data + idx, &val, maxValue);
    } else if (value.getType() == Property::VECTOR3) {
      auto &val = value.getValue<glm::vec3>();
      memcpy(data + idx, &val, maxValue);
    } else if (value.getType() == Property::VECTOR4) {
      auto &val = value.getValue<glm::vec4>();
      memcpy(data + idx, &val, maxValue);
    } else if (value.getType() == Property::MATRIX4) {
      auto &val = value.getValue<glm::mat4>();
      memcpy(data + idx, &val, maxValue);
    }
    idx += maxValue;
  }

  return size;
}

} // namespace liquid
