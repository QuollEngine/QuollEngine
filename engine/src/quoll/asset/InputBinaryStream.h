#pragma once

#include "quoll/core/Uuid.h"
#include "AssetFileHeader.h"
#include "AssetMeta.h"

namespace quoll {

class InputBinaryStream : NoCopyMove {
public:
  InputBinaryStream(const Path &path);

  ~InputBinaryStream();

  inline bool good() const { return mStream.good(); }

  template <class TPrimitive> void read(TPrimitive *value, usize size) {
    mStream.read(reinterpret_cast<char *>(value),
                 static_cast<std::streamsize>(size));
  }

  template <class TPrimitive> void read(TPrimitive &value) {
    read(&value, sizeof(TPrimitive));
  }

  template <class TPrimitive> inline void read(std::vector<TPrimitive> &value) {
    read(value.data(), sizeof(TPrimitive) * value.size());
  }

private:
  std::ifstream mStream;
};

template <> inline void InputBinaryStream::read(String &value) {
  u32 length = 0;
  read(&length, sizeof(u32));

  value.resize(length);
  read(value.data(), length);
}

template <> inline void InputBinaryStream::read(Uuid &value) {
  String str;
  read(str);
  value.updateWithString(str);
}

template <> inline void InputBinaryStream::read(glm::vec2 &value) {
  read(glm::value_ptr(value), sizeof(glm::vec2));
}

template <> inline void InputBinaryStream::read(glm::vec3 &value) {
  read(glm::value_ptr(value), sizeof(glm::vec3));
}

template <> inline void InputBinaryStream::read(glm::vec4 &value) {
  read(glm::value_ptr(value), sizeof(glm::vec4));
}

template <> inline void InputBinaryStream::read(glm::quat &value) {
  read(glm::value_ptr(value), sizeof(glm::quat));
}

template <> inline void InputBinaryStream::read(std::vector<String> &value) {
  for (usize i = 0; i < value.size(); ++i) {
    read<String>(value.at(i));
  }
}

template <> inline void InputBinaryStream::read(std::vector<Uuid> &value) {
  for (usize i = 0; i < value.size(); ++i) {
    read<Uuid>(value.at(i));
  }
}

template <> inline void InputBinaryStream::read(AssetFileHeader &header) {
  read(header.magic);
  read(header.name);
  read(header.type);
}

template <> inline void InputBinaryStream::read(AssetMeta &meta) {
  read(meta.type);
  read(meta.name);
}

} // namespace quoll
