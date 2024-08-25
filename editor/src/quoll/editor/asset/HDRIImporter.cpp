#include "quoll/core/Base.h"
#include "quoll/renderer/RenderStorage.h"
#include "quoll/renderer/TextureUtils.h"
#include "HDRIImporter.h"
#include <stb_image.h>

namespace quoll::editor {

static constexpr u32 CubemapSides = 6;

static constexpr rhi::Format TargetFormat = rhi::Format::Rgba16Float;
static constexpr u32 FormatSize = 4 * 2;

HDRIImporter::HDRIImporter(AssetCache &assetCache, RenderStorage &renderStorage)
    : mAssetCache(assetCache), mRenderStorage(renderStorage) {

  auto *device = mRenderStorage.getDevice();

  const auto shadersPath =
      std::filesystem::current_path() / "assets" / "shaders";

  {
    rhi::DescriptorLayoutBindingDescription binding0{};
    binding0.type = rhi::DescriptorLayoutBindingType::Static;
    binding0.binding = 0;
    binding0.name = "uInputTexture";
    binding0.shaderStage = rhi::ShaderStage::Compute;
    binding0.descriptorCount = 1;
    binding0.descriptorType = rhi::DescriptorType::SampledImage;

    rhi::DescriptorLayoutBindingDescription binding1{};
    binding1.type = rhi::DescriptorLayoutBindingType::Static;
    binding1.binding = 1;
    binding1.name = "uInputSampler";
    binding1.shaderStage = rhi::ShaderStage::Compute;
    binding1.descriptorCount = 1;
    binding1.descriptorType = rhi::DescriptorType::Sampler;

    rhi::DescriptorLayoutBindingDescription binding2{};
    binding2.type = rhi::DescriptorLayoutBindingType::Static;
    binding2.binding = 2;
    binding2.name = "uOutputTexture";
    binding2.shaderStage = rhi::ShaderStage::Compute;
    binding2.descriptorCount = 1;
    binding2.descriptorType = rhi::DescriptorType::StorageImage;

    auto layout = device->createDescriptorLayout(
        {{binding0, binding1, binding2}, "HDRI cubemap generator"});
    mDescriptorGenerateCubemap = device->createDescriptor(layout);
  }

  {
    auto shader = mRenderStorage.createShader(
        "hrdi.equirectangular-to-cubemap.compute",
        {shadersPath / "equirectangular-to-cubemap.comp.spv"});

    mPipelineGenerateCubemap =
        mRenderStorage.addPipeline(rhi::ComputePipelineDescription{
            shader, "HDRI equirectangular to cubemap"});

    mRenderStorage.getDevice()->createPipeline(
        mRenderStorage.getComputePipelineDescription(mPipelineGenerateCubemap),
        mPipelineGenerateCubemap);
  }

  {
    auto shader = mRenderStorage.createShader(
        "hrdi.generate-irradiance-map.compute",
        {shadersPath / "generate-irradiance-map.comp.spv"});

    mPipelineGenerateIrradianceMap =
        mRenderStorage.addPipeline(rhi::ComputePipelineDescription{
            shader, "HDRI generate irradiance map"});

    mRenderStorage.getDevice()->createPipeline(
        mRenderStorage.getComputePipelineDescription(
            mPipelineGenerateIrradianceMap),
        mPipelineGenerateIrradianceMap);
  }

  {
    auto shader = mRenderStorage.createShader(
        "hrdi.generate-specular-map.compute",
        {shadersPath / "generate-specular-map.comp.spv"});

    mPipelineGenerateSpecularMap = mRenderStorage.addPipeline(
        rhi::ComputePipelineDescription{shader, "HDRI generate specular map"});

    mRenderStorage.getDevice()->createPipeline(
        mRenderStorage.getComputePipelineDescription(
            mPipelineGenerateSpecularMap),
        mPipelineGenerateSpecularMap);
  }
}

Result<UUIDMap> HDRIImporter::loadFromPath(const Path &sourceAssetPath,
                                           const UUIDMap &uuids) {
  auto *device = mRenderStorage.getDevice();

  i32 width = 0;
  i32 height = 0;
  i32 channels = 0;

  auto *data = stbi_loadf(sourceAssetPath.string().c_str(), &width, &height,
                          &channels, STBI_rgb_alpha);

  if (!data) {
    return Result<UUIDMap>::Error(stbi_failure_reason());
  }

  auto unfilteredCubemap = convertEquirectangularToCubemap(
      data, static_cast<u32>(width), static_cast<u32>(height));
  stbi_image_free(data);

  auto irradianceMapName = sourceAssetPath.filename().string() + "/irradiance";
  auto irradianceCubemap = generateIrradianceMap(
      unfilteredCubemap, getOrCreateUuidFromMap(uuids, "irradiance"),
      irradianceMapName);

  if (irradianceCubemap.hasError()) {
    device->destroyTexture(unfilteredCubemap.texture);
    return Result<UUIDMap>::Error(irradianceCubemap.getError());
  }

  auto specularMapName = sourceAssetPath.filename().string() + "/specular";
  auto specularCubemap = generateSpecularMap(
      unfilteredCubemap, getOrCreateUuidFromMap(uuids, "specular"),
      specularMapName);

  if (specularCubemap.hasError()) {
    device->destroyTexture(unfilteredCubemap.texture);
    mAssetCache.getRegistry().remove(irradianceCubemap.getData());

    return Result<UUIDMap>::Error(specularCubemap.getError());
  }

  device->destroyTexture(unfilteredCubemap.texture);

  AssetData<EnvironmentAsset> environment{};
  environment.uuid = getOrCreateUuidFromMap(uuids, "root");
  environment.data.specularMap = specularCubemap.getData();
  environment.data.irradianceMap = irradianceCubemap.getData();

  auto createdFileRes = mAssetCache.createEnvironmentFromAsset(environment);

  if (createdFileRes.hasError()) {
    return Result<UUIDMap>::Error(createdFileRes.getError());
  }

  auto loadRes = mAssetCache.loadEnvironment(environment.uuid);
  if (loadRes.hasError()) {
    return Result<UUIDMap>::Error(loadRes.getError());
  }

  auto &registry = mAssetCache.getRegistry();

  UUIDMap output{
      {"root", registry.get(loadRes.getData()).uuid},
      {"irradiance", registry.get(environment.data.irradianceMap).uuid},
      {"specular", registry.get(environment.data.specularMap).uuid}};

  return Result<UUIDMap>::Ok(output);
}

rhi::TextureHandle
HDRIImporter::loadFromPathToDevice(const Path &sourceAssetPath,
                                   RenderStorage &renderStorage) {
  auto *device = mRenderStorage.getDevice();

  i32 width = 0;
  i32 height = 0;
  i32 channels = 0;

  auto *data = stbi_loadf(sourceAssetPath.string().c_str(), &width, &height,
                          &channels, STBI_rgb_alpha);

  if (!data) {
    return rhi::TextureHandle::Null;
  }

  rhi::TextureDescription hdriTextureDesc{};
  hdriTextureDesc.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
  hdriTextureDesc.format = rhi::Format::Rgba32Float;
  hdriTextureDesc.width = width;
  hdriTextureDesc.height = height;
  hdriTextureDesc.layerCount = 1;

  auto hdriTexture = renderStorage.createTexture(hdriTextureDesc);
  TextureUtils::copyDataToTexture(
      device, data, hdriTexture, rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
      {{0, static_cast<usize>(width) * height * 4 * sizeof(f32),
        static_cast<u32>(width), static_cast<u32>(height)}});

  return hdriTexture;
}

HDRIImporter::CubemapData
HDRIImporter::convertEquirectangularToCubemap(f32 *data, u32 width,
                                              u32 height) {
  auto *device = mRenderStorage.getDevice();

  rhi::TextureDescription hdriTextureDesc{};
  hdriTextureDesc.usage = rhi::TextureUsage::Color |
                          rhi::TextureUsage::TransferDestination |
                          rhi::TextureUsage::Sampled;
  hdriTextureDesc.format = rhi::Format::Rgba32Float;
  hdriTextureDesc.width = width;
  hdriTextureDesc.height = height;
  hdriTextureDesc.layerCount = 1;

  auto hdriTexture = mRenderStorage.createTexture(hdriTextureDesc, false);

  TextureUtils::copyDataToTexture(
      device, data, hdriTexture, rhi::ImageLayout::ShaderReadOnlyOptimal, 1,
      {{0, static_cast<usize>(width) * height * 4 * sizeof(f32),
        static_cast<u32>(width), static_cast<u32>(height)}});

  const u32 CubemapResolution = height / 2;
  const u32 CubemapFaceSize =
      CubemapResolution * CubemapResolution * FormatSize;
  const usize CubemapFullSize = CubemapFaceSize * CubemapSides;
  const u32 GroupCount = CubemapResolution / 32;

  auto numLevels =
      static_cast<u32>(std::floor(std::log2(CubemapResolution))) + 1;

  std::vector<TextureAssetMipLevel> levels(numLevels);
  u32 bufferOffset = 0;
  u32 mipWidth = CubemapResolution;
  u32 mipHeight = CubemapResolution;
  for (u32 i = 0; i < numLevels; ++i) {
    levels.at(i).offset = bufferOffset;
    levels.at(i).size =
        static_cast<usize>(mipWidth) * mipHeight * CubemapSides * FormatSize;
    levels.at(i).width = mipWidth;
    levels.at(i).height = mipHeight;

    bufferOffset += static_cast<u32>(levels.at(i).size);

    if (mipWidth > 1) {
      mipWidth /= 2;
    }

    if (mipHeight > 1) {
      mipHeight /= 2;
    }
  }

  usize totalSize = TextureUtils::getBufferSizeFromLevels(levels);

  // Unfiltered cubemap, prefiltered cubemap,
  // irradiance map and specular map all have the same
  // texture spec
  rhi::TextureDescription cubemapDesc;
  cubemapDesc.type = rhi::TextureType::Cubemap;
  cubemapDesc.format = TargetFormat;
  cubemapDesc.height = CubemapResolution;
  cubemapDesc.width = CubemapResolution;
  cubemapDesc.layerCount = CubemapSides;
  cubemapDesc.mipLevelCount = numLevels;
  cubemapDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Sampled |
                      rhi::TextureUsage::TransferSource |
                      rhi::TextureUsage::TransferDestination;

  auto unfilteredCubemap = mRenderStorage.createTexture(cubemapDesc);

  // Convert equirectangular texture to cubemap
  {
    std::array<rhi::TextureHandle, 1> hdriTextureData{hdriTexture};
    mDescriptorGenerateCubemap.write(0, hdriTextureData,
                                     rhi::DescriptorType::SampledImage);

    std::array<rhi::SamplerHandle, 1> samplerData{
        mRenderStorage.getDefaultSampler()};
    mDescriptorGenerateCubemap.write(1, samplerData);

    std::array<rhi::TextureHandle, 1> unfilteredCubemapData{unfilteredCubemap};
    mDescriptorGenerateCubemap.write(2, unfilteredCubemapData,
                                     rhi::DescriptorType::StorageImage);

    auto commandList = device->requestImmediateCommandList();

    std::array<rhi::ImageBarrier, 1> imageBarriers{{rhi::ImageBarrier{
        rhi::Access::None, rhi::Access::ShaderWrite, rhi::PipelineStage::None,
        rhi::PipelineStage::ComputeShader, rhi::ImageLayout::Undefined,
        rhi::ImageLayout::General, unfilteredCubemap}}};

    commandList.pipelineBarrier({}, imageBarriers, {});

    commandList.bindPipeline(mPipelineGenerateCubemap);
    commandList.bindDescriptor(mPipelineGenerateCubemap, 0,
                               mDescriptorGenerateCubemap);
    commandList.dispatch(GroupCount, GroupCount, CubemapSides);

    device->submitImmediate(commandList);
  }

  TextureUtils::generateMipMapsForTexture(
      device, unfilteredCubemap, rhi::ImageLayout::General, CubemapSides,
      numLevels, CubemapResolution, CubemapResolution);

  return CubemapData{unfilteredCubemap, levels};
}

Result<AssetHandle<TextureAsset>>
HDRIImporter::generateIrradianceMap(const CubemapData &unfilteredCubemap,
                                    const Uuid &uuid, const String &name) {
  auto *device = mRenderStorage.getDevice();

  const u32 GroupCount = unfilteredCubemap.levels.at(0).width / 32;

  rhi::TextureDescription cubemapDesc;
  cubemapDesc.type = rhi::TextureType::Cubemap;
  cubemapDesc.format = TargetFormat;
  cubemapDesc.height = unfilteredCubemap.levels.at(0).width;
  cubemapDesc.width = unfilteredCubemap.levels.at(0).width;
  cubemapDesc.layerCount = CubemapSides;
  cubemapDesc.mipLevelCount = 1;
  cubemapDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Sampled |
                      rhi::TextureUsage::TransferSource;
  auto irradianceCubemap = mRenderStorage.createTexture(cubemapDesc);

  std::array<rhi::TextureHandle, 1> unfilteredCubemapData{
      unfilteredCubemap.texture};
  mDescriptorGenerateCubemap.write(0, unfilteredCubemapData,
                                   rhi::DescriptorType::SampledImage);

  std::array<rhi::SamplerHandle, 1> samplerData{
      mRenderStorage.getDefaultSampler()};
  mDescriptorGenerateCubemap.write(1, samplerData);

  std::array<rhi::TextureHandle, 1> irradianceCubemapData{irradianceCubemap};
  mDescriptorGenerateCubemap.write(2, irradianceCubemapData,
                                   rhi::DescriptorType::StorageImage);

  auto commandList = device->requestImmediateCommandList();

  std::array<rhi::ImageBarrier, 2> imageBarriers{
      rhi::ImageBarrier{
          rhi::Access::None, rhi::Access::ShaderRead, rhi::PipelineStage::None,
          rhi::PipelineStage::ComputeShader, rhi::ImageLayout::General,
          rhi::ImageLayout::ShaderReadOnlyOptimal, unfilteredCubemap.texture},
      rhi::ImageBarrier{
          rhi::Access::None, rhi::Access::ShaderWrite, rhi::PipelineStage::None,
          rhi::PipelineStage::ComputeShader, rhi::ImageLayout::Undefined,
          rhi::ImageLayout::General, irradianceCubemap}};

  commandList.pipelineBarrier({}, imageBarriers, {});

  commandList.bindPipeline(mPipelineGenerateIrradianceMap);
  commandList.bindDescriptor(mPipelineGenerateIrradianceMap, 0,
                             mDescriptorGenerateCubemap);
  commandList.dispatch(GroupCount, GroupCount, CubemapSides);

  device->submitImmediate(commandList);

  auto levels = {unfilteredCubemap.levels.at(0)};

  AssetData<TextureAsset> asset{};
  asset.name = name;
  asset.uuid = uuid;
  asset.data.size = TextureUtils::getBufferSizeFromLevels(levels);
  asset.data.type = TextureAssetType::Cubemap;
  asset.data.width = unfilteredCubemap.levels.at(0).width;
  asset.data.height = unfilteredCubemap.levels.at(0).height;
  asset.data.layers = 1;
  asset.data.format = TargetFormat;
  asset.data.levels = levels;

  asset.data.data.resize(asset.data.size);

  TextureUtils::copyTextureToData(
      device, irradianceCubemap, rhi::ImageLayout::ShaderReadOnlyOptimal,
      CubemapSides, asset.data.levels, asset.data.data.data());
  device->destroyTexture(irradianceCubemap);

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);
  if (createdFileRes.hasError()) {
    return Result<AssetHandle<TextureAsset>>::Error(createdFileRes.getError());
  }

  return mAssetCache.loadTexture(asset.uuid);
}

Result<AssetHandle<TextureAsset>>
HDRIImporter::generateSpecularMap(const CubemapData &unfilteredCubemap,
                                  const Uuid &uuid, const String &name) {
  auto *device = mRenderStorage.getDevice();

  rhi::TextureDescription cubemapDesc;
  cubemapDesc.type = rhi::TextureType::Cubemap;
  cubemapDesc.format = TargetFormat;
  cubemapDesc.height = unfilteredCubemap.levels.at(0).width;
  cubemapDesc.width = unfilteredCubemap.levels.at(0).width;
  cubemapDesc.layerCount = CubemapSides;
  cubemapDesc.mipLevelCount = static_cast<u32>(unfilteredCubemap.levels.size());
  cubemapDesc.usage = rhi::TextureUsage::Color | rhi::TextureUsage::Storage |
                      rhi::TextureUsage::Sampled |
                      rhi::TextureUsage::TransferSource;

  auto specularCubemap = mRenderStorage.createTexture(cubemapDesc);

  std::vector<rhi::TextureHandle> textureViews(unfilteredCubemap.levels.size());
  for (usize level = 0; level < unfilteredCubemap.levels.size(); ++level) {
    rhi::TextureViewDescription description{};
    description.texture = specularCubemap;
    description.baseMipLevel = static_cast<u32>(level);
    description.layerCount = CubemapSides;

    textureViews.at(level) =
        mRenderStorage.createTextureView(description, false);
  }

  std::array<rhi::TextureHandle, 1> unfilteredCubemapData{
      unfilteredCubemap.texture};
  mDescriptorGenerateCubemap.write(0, unfilteredCubemapData,
                                   rhi::DescriptorType::SampledImage);

  std::array<rhi::SamplerHandle, 1> samplerData{
      mRenderStorage.getDefaultSampler()};
  mDescriptorGenerateCubemap.write(1, samplerData);

  for (usize mipLevel = 0; mipLevel < unfilteredCubemap.levels.size();
       ++mipLevel) {
    std::array<rhi::TextureHandle, 1> textureViewData{
        textureViews.at(mipLevel)};
    mDescriptorGenerateCubemap.write(2, textureViewData,
                                     rhi::DescriptorType::StorageImage);

    auto commandList = device->requestImmediateCommandList();
    commandList.bindPipeline(mPipelineGenerateSpecularMap);
    commandList.bindDescriptor(mPipelineGenerateSpecularMap, 0,
                               mDescriptorGenerateCubemap);

    auto &level = unfilteredCubemap.levels.at(mipLevel);
    u32 groupSize = level.width;
    f32 roughness = static_cast<f32>(mipLevel) /
                    static_cast<f32>(unfilteredCubemap.levels.size());

    glm::vec4 data{roughness, mipLevel, static_cast<f32>(level.width), 0.0f};
    commandList.pushConstants(mPipelineGenerateSpecularMap,
                              rhi::ShaderStage::Compute, 0, sizeof(glm::vec4),
                              glm::value_ptr(data));
    commandList.dispatch(groupSize, groupSize, CubemapSides);
    device->submitImmediate(commandList);
  }

  for (auto view : textureViews) {
    device->destroyTexture(view);
  }

  AssetData<TextureAsset> asset{};
  asset.name = name;
  asset.uuid = uuid;
  asset.data.size =
      TextureUtils::getBufferSizeFromLevels(unfilteredCubemap.levels);
  asset.data.type = TextureAssetType::Cubemap;
  asset.data.width = unfilteredCubemap.levels.at(0).width;
  asset.data.height = unfilteredCubemap.levels.at(0).height;
  asset.data.layers = 1;
  asset.data.format = TargetFormat;
  asset.data.levels = unfilteredCubemap.levels;

  asset.data.data.resize(asset.data.size);

  TextureUtils::copyTextureToData(
      device, specularCubemap, rhi::ImageLayout::ShaderReadOnlyOptimal,
      CubemapSides, asset.data.levels, asset.data.data.data());
  device->destroyTexture(specularCubemap);

  auto createdFileRes = mAssetCache.createTextureFromAsset(asset);
  if (createdFileRes.hasError()) {
    return Result<AssetHandle<TextureAsset>>::Error(createdFileRes.getError());
  }

  return mAssetCache.loadTexture(asset.uuid);
}

} // namespace quoll::editor
