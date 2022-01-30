#pragma once

namespace liquid {

class Texture;
class HardwareBuffer;

enum class DescriptorType {
  UniformBuffer,
  CombinedImageSampler,
};

struct DescriptorBinding {
  DescriptorType type;
  std::variant<std::vector<SharedPtr<Texture>>, SharedPtr<HardwareBuffer>> data;
};

class Descriptor {
public:
  /**
   * @brief Bind texture descriptor
   *
   * @param binding Binding number
   * @param textures List of textures
   * @param type Descriptor type
   * @return Current object
   */
  Descriptor &bind(uint32_t binding,
                   const std::vector<SharedPtr<Texture>> &textures,
                   DescriptorType type);
  /**
   * @brief Bind buffer descriptor
   *
   * @param binding Binding number
   * @param buffer Hardware buffer
   * @param type Descriptor type
   * @return Current object
   */
  Descriptor &bind(uint32_t binding, const SharedPtr<HardwareBuffer> &buffer,
                   DescriptorType type);

  /**
   * @brief Get bindings
   *
   * @return Bindings
   */
  inline const std::map<uint32_t, DescriptorBinding> &getBindings() const {
    return bindings;
  }

  /**
   * @brief Get hash code
   *
   * @return Hash code
   */
  inline const String &getHashCode() const { return hashCode; }

private:
  std::map<uint32_t, DescriptorBinding> bindings;
  String hashCode;
};

} // namespace liquid
