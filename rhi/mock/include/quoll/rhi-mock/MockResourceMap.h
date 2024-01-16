#pragma once

namespace quoll::rhi {

template <class THandle, class TResource> class MockResourceMap {
public:
  THandle insert(const TResource &resource) {
    auto handle = static_cast<THandle>(mIncrement);
    mResources.insert_or_assign(handle, std::move(resource));

    mIncrement++;
    return handle;
  }

  THandle insert(TResource &&resource) {
    auto handle = static_cast<THandle>(mIncrement);
    mResources.insert_or_assign(handle, std::move(resource));
    incrementEmplacement(handle);

    mIncrement++;
    return handle;
  }

  void insert(const TResource &resource, THandle handle) {
    mResources.insert_or_assign(handle, std::move(resource));
    incrementEmplacement(handle);
  }

  void insert(TResource &&resource, THandle handle) {
    mResources.insert_or_assign(handle, std::move(resource));
    incrementEmplacement(handle);
  }

  void replace(THandle handle, TResource &&resource) {
    mResources.emplace(handle, std::move(resource));
    incrementEmplacement(handle);
  }

  const TResource &at(THandle handle) const { return mResources.at(handle); }

  void clear() {
    mResources.clear();
    mEmplacements.clear();
    mIncrement = 1;
  }

  void erase(THandle handle) { mResources.erase(handle); }

  bool exists(THandle handle) const {
    return mResources.find(handle) != mResources.end();
  }

  u32 getEmplaced(THandle handle) const { return mEmplacements.at(handle); }

private:
  void incrementEmplacement(THandle handle) {
    auto it = mEmplacements.find(handle);
    if (it != mEmplacements.end()) {
      mEmplacements.at(handle)++;
    } else {
      mEmplacements.insert({handle, 1});
    }
  }

private:
  u32 mIncrement = 1;
  std::unordered_map<THandle, TResource> mResources;
  std::unordered_map<THandle, u32> mEmplacements;
};

} // namespace quoll::rhi
