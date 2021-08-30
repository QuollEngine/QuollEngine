#include "Material.h"
#include "core/EngineGlobals.h"

namespace liquid {

Material::Material(const SharedPtr<Shader> &vertexShader_,
                   const SharedPtr<Shader> &fragmentShader_,
                   const std::vector<SharedPtr<Texture>> &textures_,
                   const std::vector<std::pair<String, Property>> &properties_,
                   const CullMode &cullMode_,
                   ResourceAllocator *resourceAllocator,
                   ResourceManager *resourceManager)
    : vertexShader(vertexShader_), fragmentShader(fragmentShader_),
      textures(textures_), cullMode(cullMode_) {

  for (auto i = 0; i < properties_.size(); ++i) {
    auto &prop = properties_[i];
    properties.push_back(prop.second);
    propertyMap.insert(std::make_pair(prop.first, i));
  }

  if (!properties.empty()) {
    size_t maxValue = 0;
    for (auto &value : properties) {
      maxValue = maxValue > value.getSize() ? maxValue : value.getSize();
    }

    uniformBuffer.reset(
        resourceAllocator->createUniformBuffer(maxValue * properties.size()));

    updateBufferWithProperties();
  }

  resourceBinder = resourceManager->createMaterialResourceBinder(this);
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
  updateBufferWithProperties();
}

void Material::updateBufferWithProperties() {
  char *data = new char[uniformBuffer->getBufferSize()];

  size_t idx = 0;

  size_t maxValue = 0;
  for (auto &value : properties) {
    maxValue = maxValue > value.getSize() ? maxValue : value.getSize();
  }

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

  uniformBuffer->update(data);

  delete[] data;
}

} // namespace liquid
