#pragma once

#include <yaml-cpp/yaml.h>

namespace YAML {

template <> struct convert<glm::vec2> {
  static Node encode(const glm::vec2 &value) {
    Node node;
    node.push_back(value.x);
    node.push_back(value.y);
    return node;
  }

  static bool decode(const Node &node, glm::vec2 &value) {
    if (!node.IsSequence() || node.size() != 2) {
      return false;
    }

    return convert<f32>::decode(node[0], value.x) &&
           convert<f32>::decode(node[1], value.y);
  }
};

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

    return convert<f32>::decode(node[0], value.x) &&
           convert<f32>::decode(node[1], value.y) &&
           convert<f32>::decode(node[2], value.z);
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

    return convert<f32>::decode(node[0], value.x) &&
           convert<f32>::decode(node[1], value.y) &&
           convert<f32>::decode(node[2], value.z) &&
           convert<f32>::decode(node[3], value.w);
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

    return convert<f32>::decode(node[0], value.x) &&
           convert<f32>::decode(node[1], value.y) &&
           convert<f32>::decode(node[2], value.z) &&
           convert<f32>::decode(node[3], value.w);
  }
};

template <> struct convert<quoll::Uuid> {
  static Node encode(const quoll::Uuid &value) {
    return convert<quoll::String>::encode(value.toString());
  }

  static bool decode(const Node &node, quoll::Uuid &value) {
    quoll::String str;
    bool status = convert<quoll::String>::decode(node, str);
    if (!status) {
      return status;
    }

    value.updateWithString(str);

    return status;
  }
};

} // namespace YAML
