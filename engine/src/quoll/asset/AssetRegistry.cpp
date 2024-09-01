#include "quoll/core/Base.h"
#include "quoll/core/Profiler.h"
#include "AssetRegistry.h"
#include "DefaultObjects.h"

namespace quoll {

void AssetRegistry::createDefaultObjects() {
  mDefaultObjects.cube =
      AssetRef(mMeshes, mMeshes.addAsset(default_objects::createCube()));
  mDefaultObjects.defaultMaterial =
      AssetRef(mMaterials,
               mMaterials.addAsset(default_objects::createDefaultMaterial()));

  mDefaultObjects.defaultFont =
      AssetRef(mFonts, mFonts.addAsset(default_objects::createDefaultFont()));
}

std::pair<AssetType, u32> AssetRegistry::getAssetByUuid(const Uuid &uuid) {
  QUOLL_PROFILE_EVENT("AssetRegistry::getAssetByUUID");
  for (auto &[handle, asset] : mTextures.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Texture, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mFonts.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Font, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mMaterials.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Material, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mMeshes.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Mesh, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mSkeletons.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Skeleton, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mAnimations.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Animation, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mAnimators.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Animator, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mAudios.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Audio, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mPrefabs.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Prefab, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mLuaScripts.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::LuaScript, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mEnvironments.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Environment, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mScenes.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::Scene, handle.getRawId()};
    }
  }

  for (auto &[handle, asset] : mInputMaps.getAssets()) {
    if (asset.uuid == uuid) {
      return {AssetType::InputMap, handle.getRawId()};
    }
  }

  return {AssetType::None, 0};
}

} // namespace quoll
