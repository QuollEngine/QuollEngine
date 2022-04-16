#pragma once

namespace liquid {

class InputBinaryStream {
public:
  /**
   * @brief Create input binary stream
   *
   * @param path Path to file
   */
  InputBinaryStream(const std::filesystem::path &path);

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
  template <class TPrimitive> void read(TPrimitive *value, size_t size) {
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
  uint32_t length = 0;
  read(&length, sizeof(uint32_t));

  value.resize(length);
  read(value.data(), length);
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
  for (size_t i = 0; i < value.size(); ++i) {
    uint32_t length = 0;
    read(&length, sizeof(uint32_t));
    value.at(i).resize(length);
    read(value.at(i).data(), length);
  }
}

} // namespace liquid
