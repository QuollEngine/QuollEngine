#include <yaml-cpp/yaml.h>

namespace YAML {

/**
 * @brief GLM 3D vector Yaml serializer
 */
template <> struct convert<glm::vec3> {
  /**
   * @brief Encode GLM 3D vector to Yaml
   *
   * @param value GLM 3D vector
   * @return Yaml node
   */
  static Node encode(const glm::vec3 &value) {
    Node node;
    node.push_back(value.x);
    node.push_back(value.y);
    node.push_back(value.z);
    return node;
  }

  /**
   * @brief Decode Yaml to GLM 3D vector
   *
   * @param node Yaml node
   * @param value GLM 3D vector
   * @retval true Decoding successful
   * @retval false Decoding failed
   */
  static bool decode(const Node &node, glm::vec3 &value) {
    if (!node.IsSequence() || node.size() != 3) {
      return false;
    }

    value.x = node[0].as<float>();
    value.y = node[1].as<float>();
    value.z = node[2].as<float>();
    return true;
  }
};

/**
 * @brief GLM 4D vector Yaml serializer
 */
template <> struct convert<glm::vec4> {
  /**
   * @brief Encode GLM 4D vector to YAML
   *
   * @param value GLM 4D vector
   * @return Yaml node
   */
  static Node encode(const glm::vec4 &value) {
    Node node;
    node.push_back(value.x);
    node.push_back(value.y);
    node.push_back(value.z);
    node.push_back(value.w);
    return node;
  }

  /**
   * @brief Decode Yaml to GLM 4D vector
   *
   * @param node Yaml node
   * @param value GLM 4D vector
   * @retval true Decoding successful
   * @retval false Decoding failed
   */
  static bool decode(const Node &node, glm::vec4 &value) {
    if (!node.IsSequence() || node.size() != 4) {
      return false;
    }

    value.x = node[0].as<float>();
    value.y = node[1].as<float>();
    value.z = node[2].as<float>();
    value.w = node[3].as<float>();
    return true;
  }
};

/**
 * @brief GLM quaternion Yaml serializer
 */
template <> struct convert<glm::quat> {
  /**
   * @brief Encode GLM quaternion to YAML
   *
   * @param value GLM quaternion
   * @return Yaml node
   */
  static Node encode(const glm::quat &value) {
    Node node;
    node.push_back(value.x);
    node.push_back(value.y);
    node.push_back(value.z);
    node.push_back(value.w);
    return node;
  }

  /**
   * @brief Decode Yaml to GLM quaternion
   *
   * @param node Yaml node
   * @param value GLM quaternion
   * @retval true Decoding successful
   * @retval false Decoding failed
   */
  static bool decode(const Node &node, glm::quat &value) {
    if (!node.IsSequence() || node.size() != 4) {
      return false;
    }

    value.x = node[0].as<float>();
    value.y = node[1].as<float>();
    value.z = node[2].as<float>();
    value.w = node[3].as<float>();
    return true;
  }
};

} // namespace YAML
