#pragma once

namespace quoll {

class Uuid {
public:
  static Uuid generate();

public:
  Uuid() = default;

  explicit Uuid(String uuid);

  inline String toString() const { return mUuid; }

  inline bool operator==(const Uuid &rhs) const { return mUuid == rhs.mUuid; }

  inline bool operator!=(const Uuid &rhs) const { return mUuid != rhs.mUuid; }

  bool isValid() const { return !mUuid.empty(); }

  bool isEmpty() const { return mUuid.empty(); }

  void updateWithString(String uuid);

private:
  String mUuid = "";
};

} // namespace quoll
