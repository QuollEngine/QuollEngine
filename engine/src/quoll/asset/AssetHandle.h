#pragma once

namespace quoll {

using AssetHandleType = u32;

template <typename TAssetData> class AssetHandle {
public:
  constexpr AssetHandle() = default;
  explicit constexpr AssetHandle(AssetHandleType handle) : mHandle(handle) {}

  constexpr bool operator==(const AssetHandle &rhs) const {
    return mHandle == rhs.mHandle;
  }

  constexpr bool operator!=(const AssetHandle &rhs) const {
    return mHandle != rhs.mHandle;
  }

  constexpr operator bool() const { return mHandle != 0; }

  constexpr AssetHandleType getRawId() const { return mHandle; }

private:
  AssetHandleType mHandle = 0;
};

} // namespace quoll

template <typename TAssetData>
struct std::hash<quoll::AssetHandle<TAssetData>> {
  size_t operator()(const auto &data) const {
    return std::hash<quoll::AssetHandleType>{}(data.getRawId());
  }
};

template <typename TAssetData>
struct std::less<quoll::AssetHandle<TAssetData>> {
  bool operator()(const auto &lhs, const auto &rhs) const {
    return lhs.getRawId() < rhs.getRawId();
  }
};
