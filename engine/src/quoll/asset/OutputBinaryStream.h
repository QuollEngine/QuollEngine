#pragma once

#include "quoll/core/Uuid.h"
#include "AssetFileHeader.h"

namespace quoll {

class OutputBinaryStream : NoCopyMove {
public:
  OutputBinaryStream(Path path);

  ~OutputBinaryStream();

  inline bool good() const { return mStream.good(); }

  template <class TPrimitive>
  inline void write(const TPrimitive *value, usize size) {
    mStream.write(reinterpret_cast<const char *>(value),
                  static_cast<std::streamsize>(size));
  }

  template <class TPrimitive> inline void write(const TPrimitive &value) {
    write(&value, sizeof(TPrimitive));
  }

  template <class TPrimitive>
  inline void write(const std::vector<TPrimitive> &value) {
    write(value.data(), sizeof(TPrimitive) * value.size());
  }

private:
  std::ofstream mStream;
};

template <> inline void OutputBinaryStream::write(const String &value) {
  u32 length = static_cast<u32>(value.length());
  write(&length, sizeof(u32));
  write(value.c_str(), length);
}

template <> inline void OutputBinaryStream::write(const Uuid &value) {
  write(value.toString());
}

template <> inline void OutputBinaryStream::write(const glm::vec2 &value) {
  write(glm::value_ptr(value), sizeof(glm::vec2));
}

template <> inline void OutputBinaryStream::write(const glm::vec3 &value) {
  write(glm::value_ptr(value), sizeof(glm::vec3));
}

template <> inline void OutputBinaryStream::write(const glm::vec4 &value) {
  write(glm::value_ptr(value), sizeof(glm::vec4));
}

template <> inline void OutputBinaryStream::write(const glm::quat &value) {
  write(glm::value_ptr(value), sizeof(glm::quat));
}

template <>
inline void OutputBinaryStream::write(const AssetFileHeader &value) {
  write(value.magic);
  write(value.name);
  write(value.type);
}

template <>
inline void OutputBinaryStream::write(const std::vector<String> &value) {
  for (auto &v : value) {
    write(v);
  }
}

template <>
inline void OutputBinaryStream::write(const std::vector<Uuid> &value) {
  for (auto &v : value) {
    write(v);
  }
}

} // namespace quoll
