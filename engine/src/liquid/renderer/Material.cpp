#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "Material.h"

namespace liquid {

Material::Material(const std::vector<rhi::TextureHandle> &textures,
                   const std::vector<std::pair<String, Property>> &properties,
                   rhi::ResourceRegistry &registry)
    : mTextures(textures), mRegistry(registry) {

  for (size_t i = 0; i < properties.size(); ++i) {
    auto &prop = properties[i];
    mProperties.push_back(prop.second);
    mPropertyMap.insert({prop.first, i});
  }

  if (!mProperties.empty()) {
    auto size = updateBufferData();
    mBuffer = mRegistry.setBuffer({rhi::BufferType::Uniform, size, mData});
    mDescriptor.bind(0, mBuffer, rhi::DescriptorType::UniformBuffer);
  }

  mDescriptor.bind(1, mTextures, rhi::DescriptorType::CombinedImageSampler);
}

void Material::updateProperty(const String &name, const Property &value) {
  const auto &it = mPropertyMap.find(name);
  if (it == mPropertyMap.end() || (*it).second > mProperties.size()) {
    LOG_DEBUG("Property \"" << name
                            << "\" does not exist in material. Skipping...");
    return;
  }

  size_t index = (*it).second;

  if (mProperties.at(index).getType() != value.getType()) {
    LOG_DEBUG("Type of property \""
              << name << "\" does match the type of new property. Skipping...");
    return;
  }

  mProperties.at(index) = value;
  auto size = updateBufferData();
  mBuffer =
      mRegistry.setBuffer({rhi::BufferType::Uniform, size, mData}, mBuffer);
}

size_t Material::updateBufferData() {
  if (mData) {
    delete mData;
  }

  size_t size = 0;
  size_t idx = 0;

  size_t maxValue = 0;
  for (auto &value : mProperties) {
    maxValue = maxValue > value.getSize() ? maxValue : value.getSize();
  }

  size = maxValue * mProperties.size();

  mData = new char[size];

  for (auto &value : mProperties) {
    if (value.getType() == Property::INT32) {
      auto val = value.getValue<int32_t>();
      memcpy(mData + idx, &val, maxValue);
    } else if (value.getType() == Property::UINT32) {
      auto val = value.getValue<uint32_t>();
      memcpy(mData + idx, &val, maxValue);
    } else if (value.getType() == Property::REAL) {
      auto val = value.getValue<float>();
      memcpy(mData + idx, &val, maxValue);
    } else if (value.getType() == Property::VECTOR2) {
      auto &val = value.getValue<glm::vec2>();
      memcpy(mData + idx, &val, maxValue);
    } else if (value.getType() == Property::VECTOR3) {
      auto &val = value.getValue<glm::vec3>();
      memcpy(mData + idx, &val, maxValue);
    } else if (value.getType() == Property::VECTOR4) {
      auto &val = value.getValue<glm::vec4>();
      memcpy(mData + idx, &val, maxValue);
    } else if (value.getType() == Property::MATRIX4) {
      auto &val = value.getValue<glm::mat4>();
      memcpy(mData + idx, &val, maxValue);
    }
    idx += maxValue;
  }

  return size;
}

} // namespace liquid
