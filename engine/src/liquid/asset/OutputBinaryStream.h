#pragma once

#include "liquid/core/Uuid.h"
#include "AssetFileHeader.h"

namespace quoll {

/**
 * @brief Output binary stream
 */
class OutputBinaryStream {
public:
  /**
   * @brief Create output binary stream
   *
   * @param path Path to file
   */
  OutputBinaryStream(Path path);

  /**
   * @brief Close output binary stream
   */
  ~OutputBinaryStream();

  OutputBinaryStream(const OutputBinaryStream &) = delete;
  OutputBinaryStream(OutputBinaryStream &&) = delete;
  OutputBinaryStream &operator=(const OutputBinaryStream &) = delete;
  OutputBinaryStream &operator=(OutputBinaryStream &&) = delete;

  /**
   * @brief Check if stream is good
   *
   * @retval true Stream is good
   * @retval false Stream is bad
   */
  inline bool good() const { return mStream.good(); }

  /**
   * @brief Write data to file
   *
   * @tparam TPrimitive Primitive type
   * @param value Address of value
   * @param size Size of write
   */
  template <class TPrimitive>
  inline void write(const TPrimitive *value, std::size_t size) {
    mStream.write(reinterpret_cast<const char *>(value),
                  static_cast<std::streamsize>(size));
  }

  /**
   * @brief Write data to file
   *
   * @tparam TPrimitive Primitive type
   * @param value Value
   */
  template <class TPrimitive> inline void write(const TPrimitive &value) {
    write(&value, sizeof(TPrimitive));
  }

  /**
   * @brief Write vector data to file
   *
   * @tparam TPrimitive Vector data type
   * @param value Vector value
   */
  template <class TPrimitive>
  inline void write(const std::vector<TPrimitive> &value) {
    write(value.data(), sizeof(TPrimitive) * value.size());
  }

private:
  std::ofstream mStream;
};

/**
 * @brief Write string data to file
 *
 * @param value String value
 */
template <> inline void OutputBinaryStream::write(const String &value) {
  uint32_t length = static_cast<uint32_t>(value.length());
  write(&length, sizeof(uint32_t));
  write(value.c_str(), length);
}

/**
 * @brief Write string data to file
 *
 * @param value String value
 */
template <> inline void OutputBinaryStream::write(const Uuid &value) {
  write(value.toString());
}

/**
 * @brief Write GLM 2D vector to file
 *
 * @param value GLM 2D vector value
 */
template <> inline void OutputBinaryStream::write(const glm::vec2 &value) {
  write(glm::value_ptr(value), sizeof(glm::vec2));
}

/**
 * @brief Write GLM 3D vector to file
 *
 * @param value GLM 3D vector value
 */
template <> inline void OutputBinaryStream::write(const glm::vec3 &value) {
  write(glm::value_ptr(value), sizeof(glm::vec3));
}

/**
 * @brief Write GLM 4D vector to file
 *
 * @param value GLM 4D vector value
 */
template <> inline void OutputBinaryStream::write(const glm::vec4 &value) {
  write(glm::value_ptr(value), sizeof(glm::vec4));
}

/**
 * @brief Write GLM quat to file
 *
 * @param value GLM quat value
 */
template <> inline void OutputBinaryStream::write(const glm::quat &value) {
  write(glm::value_ptr(value), sizeof(glm::quat));
}

/**
 * @brief Write asset file header to file
 *
 * @param value Asset file header
 */
template <>
inline void OutputBinaryStream::write(const AssetFileHeader &value) {
  write(value.magic);
  write(value.name);
  write(value.type);
}

/**
 * @brief Write vector of dynamic strings to file
 *
 * @param value String vector
 */
template <>
inline void OutputBinaryStream::write(const std::vector<String> &value) {
  for (auto &v : value) {
    write(v);
  }
}

/**
 * @brief Write vector of dynamic strings to file
 *
 * @param value String vector
 */
template <>
inline void OutputBinaryStream::write(const std::vector<Uuid> &value) {
  for (auto &v : value) {
    write(v);
  }
}

} // namespace quoll
