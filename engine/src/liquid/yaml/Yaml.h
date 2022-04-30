#include <yaml-cpp/yaml.h>

namespace YAML {

template <> struct convert<glm::vec3> {
  static Node encode(const glm::vec3 &value) {
    Node node;
    node.push_back(value.x);
    node.push_back(value.y);
    node.push_back(value.z);
    return node;
  }

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

template <> struct convert<glm::vec4> {
  static Node encode(const glm::vec4 &value) {
    Node node;
    node.push_back(value.x);
    node.push_back(value.y);
    node.push_back(value.z);
    node.push_back(value.w);
    return node;
  }

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

template <> struct convert<glm::quat> {
  static Node encode(const glm::quat &value) {
    Node node;
    node.push_back(value.x);
    node.push_back(value.y);
    node.push_back(value.z);
    node.push_back(value.w);
    return node;
  }

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
