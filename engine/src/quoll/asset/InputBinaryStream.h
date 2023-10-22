#pragma once

#include "quoll/core/Uuid.h"
#include "AssetFileHeader.h"
#include "AssetMeta.h"

namespace quoll {

/**
 * @brief Input binary stream
 */
class InputBinaryStream {
public:
  /**
   * @brief Create input binary stream
   *
   * @param path Path to file
   */
  InputBinaryStream(const Path &path);

  InputBinaryStream(const InputBinaryStream &) = delete;
  InputBinaryStream(InputBinaryStream &&) = delete;
  InputBinaryStream &operator=(const InputBinaryStream &) = delete;
  InputBinaryStream &operator=(InputBinaryStream &&) = delete;

  /**
   * @brief Close input binary stream
   */
  ~InputBinaryStream();

  /**
   * @brief Check if stream is good
   *
   * @retval true Stream is good
   * @retval false Stream is bad
   */
  inline bool good() const { return mStream.good(); }

  /**
   * @brief Read binary data into value
   *
   * @tparam TPrimitive Primitive type
   * @param value Address to value
   * @param size Size to read
   */
  template <class TPrimitive> void read(TPrimitive *value, usize size) {
    mStream.read(reinterpret_cast<char *>(value),
                 static_cast<std::streamsize>(size));
  }

  /**
   * @brief Read binary data into value
   *
   * @tparam TPrimitive Primitive type
   * @param value Value
   */
  template <class TPrimitive> void read(TPrimitive &value) {
    read(&value, sizeof(TPrimitive));
  }

  /**
   * @brief Read binary data into vector
   *
   * @tparam TPrimitive Vector data type
   * @param value Vector value
   */
  template <class TPrimitive> inline void read(std::vector<TPrimitive> &value) {
    read(value.data(), sizeof(TPrimitive) * value.size());
  }

private:
  std::ifstream mStream;
};

/**
 * @brief Read binary data into string
 *
 * @param value String value
 */
template <> inline void InputBinaryStream::read(String &value) {
  u32 length = 0;
  read(&length, sizeof(u32));

  value.resize(length);
  read(value.data(), length);
}

/**
 * @brief Read binary data into uuid
 *
 * @param value Uuid value
 */
template <> inline void InputBinaryStream::read(Uuid &value) {
  String str;
  read(str);
  value.updateWithString(str);
}

/**
 * @brief Read binary data into GLM 2D vector
 *
 * @param value GLM 2D vector value
 */
template <> inline void InputBinaryStream::read(glm::vec2 &value) {
  read(glm::value_ptr(value), sizeof(glm::vec2));
}

/**
 * @brief Read binary data into GLM 3D vector
 *
 * @param value GLM 3D vector value
 */
template <> inline void InputBinaryStream::read(glm::vec3 &value) {
  read(glm::value_ptr(value), sizeof(glm::vec3));
}

/**
 * @brief Read binary data into GLM 4D vector
 *
 * @param value GLM 4D vector value
 */
template <> inline void InputBinaryStream::read(glm::vec4 &value) {
  read(glm::value_ptr(value), sizeof(glm::vec4));
}

/**
 * @brief Read binary data into GLM quat
 *
 * @param value GLM quat value
 */
template <> inline void InputBinaryStream::read(glm::quat &value) {
  read(glm::value_ptr(value), sizeof(glm::quat));
}

/**
 * @brief Read binary data into vector of strings
 *
 * @param value String vector
 */
template <> inline void InputBinaryStream::read(std::vector<String> &value) {
  for (usize i = 0; i < value.size(); ++i) {
    read<String>(value.at(i));
  }
}

/**
 * @brief Read binary data into vector of uuids
 *
 * @param value String vector
 */
template <> inline void InputBinaryStream::read(std::vector<Uuid> &value) {
  for (usize i = 0; i < value.size(); ++i) {
    read<Uuid>(value.at(i));
  }
}

/**
 * @brief Read binary data into asset file header
 *
 * @param header Asset file header
 */
template <> inline void InputBinaryStream::read(AssetFileHeader &header) {
  read(header.magic);
  read(header.name);
  read(header.type);
}

/**
 * @brief Read binary data into asset meta
 *
 * @param meta Asset meta
 */
template <> inline void InputBinaryStream::read(AssetMeta &meta) {
  read(meta.type);
  read(meta.name);
}

} // namespace quoll
